/*
 * ------------ Batmon ------------
 * Copyright (c) 2025, Ian Moffett.
 * Provided under the BSD-3 clause.
 * --------------------------------
 */

#ifndef BATSTAT_H
#define BATSTAT_H 1

#include <sys/types.h>
#include <stdbool.h>

/* Path for battery stats */
#define BATTERY_PATH        "/sys/class/power_supply/BAT0"

/* Components within the battery path */
#define BATTERY_CHARGE_NOW      "charge_now"
#define BATTERY_CHARGE_FULL     "charge_full"
#define BATTERY_STATUS          "status"
#define BATTERY_CHARGING        "Charging"

/* Thresholds */
#define BATTERY_THRES_THROT 20      /* Throttle at this percent */
#define BATTERY_THRES_CRITCAL 8     /* Sync disks and shutdown */

typedef enum {
    BATTERY_THROTTLE,
    BATTERY_CRITCAL
} threstype_t;

/*
 * Obtain the maxiumum capacity of the battery
 *
 * A return value less than zero indicates failure
 */
ssize_t battery_capacity(void);

/*
 * Obtain the current charge of the battery
 *
 * A return value less than zero indicates failure
 */
ssize_t battery_current(void);

/*
 * Returns true if the battery is charging
 */
bool battery_charging(void);

#endif  /* !BATSTAT_H */
