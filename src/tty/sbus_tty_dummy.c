#include "rpi_sbus/tty/sbus_tty_impl.h"
#ifdef RPISBUS_TTY_IMPL_DUMMY

#include "rpi_sbus/tty/sbus_tty.h"
#include <stdlib.h>


int sbus_install(const char path[], bool blocking, uint8_t timeout)
{
    exit(1);
}

enum sbus_err_t sbus_uninstall(int fd)
{
    exit(1);
}

int sbus_read(int fd, uint8_t buf[], int bufSize)
{
    exit(1);
}

enum sbus_err_t sbus_write(int fd, const uint8_t buf[], int count)
{
    exit(1);
}

#endif // RPISBUS_TTY_IMPL_DUMMY
