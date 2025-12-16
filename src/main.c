/*
 * ------------ Batmon ------------
 * Copyright (c) 2025, Ian Moffett.
 * Provided under the BSD-3 clause.
 * --------------------------------
 */

#include <sys/reboot.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "batstat.h"

#define CPU_PATH "/sys/devices/system/cpu"
#define NOTIFY_PATH "/usr/bin/notify-send"

static uint8_t thres_mask = 0;

static void
send_notify(const char *msg)
{
    pid_t child;

    child = fork();
    if (child == 0) {
        execl(NOTIFY_PATH, NOTIFY_PATH, msg, NULL);
    }
}

static void
cpu_set_state(bool throttle)
{
    char pathbuf[128];
    int fd;

    for (int i = 1; i < 255; ++i) {
        snprintf(pathbuf, sizeof(pathbuf), "%s/cpu%d/online", CPU_PATH, i);
        if ((fd = open(pathbuf, O_WRONLY)) < 0) {
            break;
        }

        if (throttle) {
            write(fd, "0", 1);
        } else {
            write(fd, "1", 1);
        }

        close(fd);
    }
}

static void
battery_emergency(void)
{
    sync();
    reboot(RB_POWER_OFF);
}

static void
battery_monitor(size_t max_cap)
{
    ssize_t charge_now;
    size_t percent;
    bool is_charging = true, tmp;

    for (;;) {
        sleep(1);
        if ((charge_now = battery_current()) < 0) {
            printf("[!] ignoring erroneous data\n");
            continue;
        }

        /*
         * If the battery just started charging, clear the threshold
         * mask and notify the user.
         */
        tmp = battery_charging();
        if (!tmp && is_charging) {
            is_charging = false;
        } else if (tmp && !is_charging) {
            send_notify("Battery is charging\n");
            is_charging = true;
            cpu_set_state(false);
            thres_mask = 0;
        }

        percent = ((double)charge_now / (double)max_cap) * 100;
        if (percent <= BATTERY_THRES_THROT) {
            if ((thres_mask & (1 << BATTERY_THROTTLE)) == 0) {
                thres_mask |= (1 << BATTERY_THROTTLE);
                send_notify("CRITICAL - LOW BATTERY\n");
                cpu_set_state(true);
            }
        }

        if (percent <= BATTERY_THRES_CRITCAL) {
            if ((thres_mask & (1 << BATTERY_CRITCAL)) == 0) {
                thres_mask |= (1 << BATTERY_CRITCAL);
                battery_emergency();
            }
        }
    }
}

int
main(void)
{
    ssize_t charge_full;
    pid_t pid;

    if (geteuid() != 0) {
        printf("[!] please run as root!\n");
        return -1;
    }

    if (access(NOTIFY_PATH, F_OK) != 0) {
        printf("[!] missing 'notify-send' binary\n");
        return -1;
    }

    if ((charge_full = battery_capacity()) < 0) {
        return -1;
    }

    pid = fork();
    if (pid == 0) {
        battery_monitor(charge_full);
    }

    return 0;
}
