#ifndef OPTIONS_H
#define OPTIONS_H

enum fb_jbod_cmd {INFO, LIST, SENSOR, HDD, LED, FAN, POWER_CYCLE,
                  GPIO, ASSET_TAG, EVENT, CONFIG, IDENTIFY, VERSION, PHYERR,
                  PWM, CFM};

struct cmd_options {
  enum fb_jbod_cmd cmd;
  char *key_word;
  int (*execute) (int argc, char *argv[]);
  int (*execute_flash) (int argc, char *argv[]);
  char *help_msg;
};

/* all supported commands */
extern struct cmd_options all_cmds[];

extern void usage(int argc, char *argv[]);
extern int parse_cmd(int argc, char *argv[]);

#endif
