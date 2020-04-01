#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "sg_lib.h"
#include "sg_cmds.h"
#include "ses.h"
#include "enclosure_info.h"
#include "scsi_buffer.h"
#include "jbod_interface.h"
#include "jbod.h"


int get_profile(const char* name, char* out)
{
#ifdef DEBUG
  perr("Start get_profile: %s\n", name);
#endif
  
  struct jbod_interface *jbod = NULL;
  size_t n = 1024;
  size_t total = 0;
  jbod = detect_dev(name);
  if (jbod) {
    jbod->print_profile(name, out + total, &n);
    total += n;
    return total;
  }
  return 0;
}

int get_serial(const char* name, char* out)
{
#ifdef DEBUG
  perr("Start get_serial: %s\n", name);
#endif
  
  struct jbod_interface *jbod = NULL;
  size_t n = 0;
  size_t total = 0;
  jbod = detect_dev(name);
  if (jbod) {
    jbod->print_serial(name, out + total, &n);
    total += n;
    return total;
  }
  return 0;
}

int get_sas_info(const char* name, char* out)
{
#ifdef DEBUG
  perr("Start get_sas_info: %s\n", name);
#endif
  struct jbod_interface *jbod;
  int sg_fd;
  size_t n = 0;
  size_t total = 0;
  jbod = detect_dev(name);
  if (jbod) {
    sg_fd = sg_cmds_open_device(name, 0 /* rw */, 0 /* not verbose */);
    if (sg_fd > 0) {
      jbod->print_sasaddress_info(sg_fd, out + total, &n);
      sg_cmds_close_device(sg_fd);
      total += n;
#ifdef DEBUG
    perr("total bytes: %ld\n", total);
#endif
      return total;
    }
  }
#ifdef DEBUG
    perr("%s is not a jbod device\n", name);
#endif
  return 0;
}

int set_hdd_led(const char* name, int id, int op)
{
#ifdef DEBUG
  perr("Start set_hdd_led: %s %s\n", name, id);
#endif
  struct jbod_interface *jbod;
  int sg_fd;
  int ret;
  jbod = detect_dev(name);
  if (jbod) {
    sg_fd = sg_cmds_open_device(name, 0 /* rw */, 0 /* not verbose */);
    if (sg_fd > 0) {
      ret = jbod->hdd_led_control(sg_fd, id, op);
      sg_cmds_close_device(sg_fd);
      if (ret == 0) {
        return id;
      }
#ifdef DEBUG
      perr("operation failed with return code = %d\n", ret);
#endif
      return -1;
    }
#ifdef DEBUG
    perr("can't open device: %s\n", name);
#endif
    return -1;    
  }
#ifdef DEBUG
  perr("%s is not a jbod device\n", name);
#endif
  return -1;
}