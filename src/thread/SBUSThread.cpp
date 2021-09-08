#include "sbus/SBUSThread.h"

SBUSThread::SBUSThread()
{
    _sbus.onPacket([this](const sbus_packet_t &p){
        onPacket(p);
    });
}

void SBUSThread::onPacket(const sbus_packet_t &packet)
{
    std::lock_guard<std::mutex> lock(_lastPacketMtx);
    _lastPacket = packet;
    _gotPacket.notify_all();
}

sbus_err_t SBUSThread::start(const char *tty)
{
    sbus_err_t err = _sbus.install(tty, true, 5);
    if (err != SBUS_OK)
        return err;

    _run.test_and_set();

    _thread = std::thread([this]{
        while(_run.test_and_set()) _sbus.read();
    });

    return SBUS_OK;
}

sbus_err_t SBUSThread::stop()
{
    if (_thread.joinable())
    {
        _run.clear();
        _thread.join();
    }
    _gotPacket.notify_all();
    return _sbus.uninstall();
}

sbus_packet_t SBUSThread::getLastPacket()
{
    std::unique_lock<std::mutex> lock(_lastPacketMtx);
    sbus_packet_t packet = _lastPacket;
    lock.unlock();
    return packet;
}

sbus_packet_t SBUSThread::getNextPacket()
{
    std::unique_lock<std::mutex> lck(_gotPacketMtx);
    _gotPacket.wait(lck);
    return getLastPacket();
}
