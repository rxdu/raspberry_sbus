#include <iostream>
#include <chrono>
#include "rcdrivers/CRSF.h"

using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::string;
using std::chrono::steady_clock;
using std::chrono::milliseconds;

static CRSF crsf;

static void onPacket(const crsf_packet_t &packet)
{
    static auto lastPrint = steady_clock::now();
    auto now = steady_clock::now();

    if ((packet.frametype == CRSF_FRAMETYPE_RC_CHANNELS_PACKED) && (now - lastPrint > milliseconds(500)))
    {
        for (int i = 0; i < CRSF_NUM_RC_CHANNELS; ++i)
            cout << "ch" << i + 1 << ": " << packet.payload.rc_channels_packed.channels[i] << "\t";

        cout << endl;

        lastPrint = now;
    }
}

int main(int argc, char **argv)
{
    cout << "CRSF blocking receiver example" << endl;

    string ttyPath;

    if (argc > 1)
        ttyPath = argv[1];
    else
    {
        cout << "Enter tty path: ";
        cin >> ttyPath;
    }

    crsf.onPacket(onPacket);

    rcdrivers_err_t err = crsf.install(ttyPath.c_str(), true);  // true for blocking mode
    if (err != RCDRIVERS_OK)
    {
        cerr << "CRSF install error: " << err << endl;
        return err;
    }

    cout << "CRSF installed" << endl;

    // blocks until data is available
    while ((err = crsf.read()) != RCDRIVERS_FAIL)
    {
        // desync means a packet was misaligned and not received properly
        if (err == RCDRIVERS_ERR_DESYNC)
        {
            cerr << "CRSF desync" << endl;
        }
    }

    cerr << "CRSF error: " << err << endl;

    return err;
}
