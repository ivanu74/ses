// +build ignore

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <signal.h>

#include "sg_lib.h"
#include "sg_cmds.h"
#include "ses.h"
#include "enclosure_info.h"
#include "scsi_buffer.h"
#include "jbod_interface.h"
#include "options.h"
//#include "common.h"

int main(int argc, char *argv[])
{
  (void) prctl(PR_SET_PDEATHSIG, SIGINT, 0, 0, 0);
#ifdef DEBUG
  perr("Start debug program: %s\n", argv[0]);
#endif
  return parse_cmd(argc, argv);
}
