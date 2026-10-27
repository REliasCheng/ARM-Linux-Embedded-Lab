#ifndef LINUX_DEVICE_PORT_H
#define LINUX_DEVICE_PORT_H

#include <stddef.h>
#include <stdint.h>

int linux_port_open(const char *path);
int linux_port_write_all(int fd, const uint8_t *data, size_t length, int timeout_ms);
int64_t linux_port_now_ms(void);

#endif
