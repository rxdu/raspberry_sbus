#include <cstdio>
#include <ctime>
#include "SBUS.h"

SBUS sbus;

void onPacket(sbus_packet_t packet)
{
    static time_t lastPrint = time(nullptr);
    time_t now = time(nullptr);

    if (now > lastPrint)
    {
        lastPrint = now;
        printf("ch1: %u\tch2: %u\tch3: %u\tch4: %u\t"
               "ch5: %u\tch6: %u\tch7: %u\tch8: %u\t"
               "ch9: %u\tch10: %u\tch11: %u\tch12: %u\t"
               "ch13: %u\tch14: %u\tch15: %u\tch16: %u\tch17: %u\tch18: %u%s%s\n",
               packet.channels[0], packet.channels[1], packet.channels[2], packet.channels[3],
               packet.channels[4], packet.channels[5], packet.channels[6], packet.channels[7],
               packet.channels[8], packet.channels[9], packet.channels[10], packet.channels[11],
               packet.channels[12], packet.channels[13], packet.channels[14], packet.channels[15],
               packet.ch17, packet.ch18,
               packet.frameLost ? "\tFrame lost" : "",
               packet.failsafe ? "\tFailsafe active" : "");
    }
}

int main()
{
    printf("SBUS example\n");

    sbus.onPacket(onPacket);

    sbus_err_t ret = sbus.install("/dev/ttyAMA0");
    if (ret != SBUS_OK)
    {
        fprintf(stderr, "SBUS install error: %d\n", ret);
        return ret;
    }

    while (sbus.read() == SBUS_OK)
    {
        static time_t lastWrite = time(nullptr);
        time_t now = time(nullptr);

        if (now > lastWrite)
        {
            lastWrite = now;

            uint16_t channels[16];
            for (int i = 0; i < 16; i++)
            {
                channels[i] = i;
            }

            sbus_packet_t packet = {
                channels,
                true, false,
                true, false
            };

            sbus.write(packet);
        }
    }

    return 0;
}