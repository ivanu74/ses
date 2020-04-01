#ifndef DRIVE_CONTROL_H
#define DRIVE_CONTROL_H

#include "common.h"

/* remove HDD from OS by echo into /sys/block/sdXXX/device/delete */
int remove_hdd(const char *devname, const char *sas_addr_str);

#endif
