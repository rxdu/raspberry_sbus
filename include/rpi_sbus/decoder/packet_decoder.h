#ifndef RPISBUS_PACKET_DECODER_H
#define RPISBUS_PACKET_DECODER_H

#include "rpi_sbus/sbus_error.h"
#include "rpi_sbus/sbus_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

enum sbus_err_t sbus_decode(const uint8_t buf[], struct sbus_packet_t *packet);

enum sbus_err_t sbus_encode(uint8_t buf[], const struct sbus_packet_t *packet);

#ifdef __cplusplus
}
#endif

#endif
