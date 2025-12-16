/*
 * ------------ Batmon ------------
 * Copyright (c) 2025, Ian Moffett.
 * Provided under the BSD-3 clause.
 * --------------------------------
 */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "batstat.h"

static int
battery_get_node(const char *node_path)
{
    int fd;
    char buf[256];

    snprintf(
        buf,
        sizeof(buf),
        "%s/%s",
        BATTERY_PATH,
        node_path
    );

    fd = open(buf, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    return fd;
}

ssize_t
battery_capacity(void)
{
    char buf[32];
    int fd;

    if ((fd = battery_get_node(BATTERY_CHARGE_FULL)) < 0) {
        return -1;
    }

    if (read(fd, buf, sizeof(buf)) < 0) {
        perror("read");
        close(fd);
        return -1;
    }

    close(fd);
    return atoi(buf);
}

ssize_t
battery_current(void)
{
    char buf[32];
    int fd;

    if ((fd = battery_get_node(BATTERY_CHARGE_NOW)) < 0) {
        return -1;
    }

    if (read(fd, buf, sizeof(buf)) < 0) {
        perror("read");
        close(fd);
        return -1;
    }

    close(fd);
    return atoi(buf);
}

bool
battery_charging(void)
{
    size_t max_len;
    char buf[32];
    int fd;

    if ((fd = battery_get_node(BATTERY_STATUS)) < 0) {
        return false;
    }

    if (read(fd, buf, sizeof(buf)) < 0) {
        perror("read");
        close(fd);
        return false;
    }

    close(fd);
    max_len = sizeof(BATTERY_CHARGING) - 1;
    if (memcmp(buf, BATTERY_CHARGING, max_len) == 0) {
        return true;
    }

    return false;
}
