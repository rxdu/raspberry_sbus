#ifndef RPISBUS_SBUS_ERROR_H
#define RPISBUS_SBUS_ERROR_H

typedef enum
{
    SBUS_OK = 0,
    SBUS_ERR_OPEN = -1,
    SBUS_ERR_TCGETS2 = -2,
    SBUS_ERR_TCSETS2 = -3,
    SBUS_FAIL = -4,
    SBUS_ERR_INVALID_ARG = -5,
    SBUS_ERR_DESYNC = -6,
} sbus_err_t;

#endif
