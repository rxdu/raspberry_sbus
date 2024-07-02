#include "rcdrivers/crsf/CRSFDecoder.h"

static rcdrivers_err_t verifyCrsfCrc8(const uint8_t packet[])
{
    return RCDRIVERS_FAIL;
}

CRSFDecoder::CRSFDecoder()
        : _state(State::WAIT_FOR_HEADER)
        , _packetPos(0)
        , _packetCb(nullptr)
{}

rcdrivers_err_t CRSFDecoder::feed(const uint8_t buf[], int bufSize, bool *hadDesyncOut)
{
    bool hadDesync = false;

    int headerByte = -1;

    for (int i = 0; i < bufSize; i++)
    {
        switch (_state)
        {
            case State::WAIT_FOR_HEADER:
            case State::HEADER_SKIP:
                if (buf[i] == CRSF_SYNC_BYTE || buf[i] == CRSF_SYNC_BYTE_EDGETX)
                {
                    if (_state == State::HEADER_SKIP)
                    {
                        // skip this header
                        _state = State::WAIT_FOR_HEADER;
                        break;
                    }

                    // remember this as the last header
                    headerByte = i;

                    _packetBuf[0] = CRSF_SYNC_BYTE;
                    _packetPos = 1;
                    _state = State::PACKET;
                }
                break;

            case State::PACKET:
                _packetBuf[_packetPos] = buf[i];
                _packetPos++;

                if (packetReceivedWhole())
                {
                    if (verifyPacket() == RCDRIVERS_OK &&
                        decodePacket() == RCDRIVERS_OK)
                    {
                        hadDesync = false;  // clear desync if last packet was ok
                        notifyCallback();

                        // receive next packet
                        _state = State::WAIT_FOR_HEADER;
                    }
                    // packet error but we found other possible headers
                    else if (headerByte >= 0)
                    {
                        // retry scanning after last header
                        i = headerByte;
                        _state = State::WAIT_FOR_HEADER;
                    }
                    else // out of headers to scan
                    {
                        hadDesync = true;
                        /*
                         * SBUS header is '15' and packet end is '0'.
                         * In case the packet looks like this:
                         * 15 .. 15 .. 00 15 .. 15 .. 00
                         * |------------| |------------|
                         * We could have locked on like this:
                         * 15 .. 15 .. 00 15 .. 15 .. 00
                         *       |------------| |------------|
                         * In this situation we would loop forever.
                         * So if a desync happens it is safer to skip the next header
                         * which makes sure we are always moving inside each packet
                         * and not stuck a on single match.
                         *
                         *
                         * Actual example (observed when my transmitter was turned off):
                         *
                         * First match:
                         * 15 124 224 3 31 248 192 7 62 240 129 15 124 12 0 15 224 3 31 44 194 199 10 86 128
                         * ^Found header                          Real end^ ^Real header                 ^End mismatch
                         *                                      ^
                         *                                      |
                         *                          (this becomes new header)
                         * Next match:
                         * 15 124 12 0 15 224 3 31 44 194 199 10 86 128 15 124 224 3 31 248 192 7 62 240 129
                         *   Real end^ ^Real header
                         * You can see the decoder grabbed the next '15'.
                         * In the next step it would grab the next(er) '15' which would end up being the actual header
                         * and decoding would succeed.
                         */
                        _state = State::HEADER_SKIP;
                    }

                    _packetPos = 0;
                }
                break;
        }
    }

    if (hadDesyncOut)
        *hadDesyncOut = hadDesync;

    return RCDRIVERS_OK;
}

bool CRSFDecoder::packetReceivedWhole()
{
    return (_packetPos > CRSF_PACKET_LEN_BYTE) &&
           (_packetPos >= (_packetBuf[CRSF_PACKET_LEN_BYTE] + 2));
}

rcdrivers_err_t CRSFDecoder::verifyPacket()
{
    if ((_packetBuf[0] == CRSF_SYNC_BYTE || _packetBuf[0] == CRSF_SYNC_BYTE_EDGETX) &&
        verifyCrsfCrc8(_packetBuf) == RCDRIVERS_OK)
        return RCDRIVERS_OK;
    else
        return RCDRIVERS_FAIL;
}

rcdrivers_err_t CRSFDecoder::decodePacket()
{
    return decode(_packetBuf, &_lastPacket);
}

bool CRSFDecoder::notifyCallback()
{
    if (_packetCb)
        _packetCb(_lastPacket);
    return static_cast<bool>(_packetCb);
}

const crsf_packet_t& CRSFDecoder::lastPacket() const
{
    return _lastPacket;
}

rcdrivers_err_t CRSFDecoder::onPacket(crsf_packet_cb cb)
{
    _packetCb = cb;
    return RCDRIVERS_OK;
}

rcdrivers_err_t CRSFDecoder::decode(const uint8_t buf[], crsf_packet_t *packet)
{
    return RCDRIVERS_FAIL;
}

rcdrivers_err_t CRSFDecoder::encode(uint8_t buf[], const crsf_packet_t *packet)
{
    return RCDRIVERS_FAIL;
}