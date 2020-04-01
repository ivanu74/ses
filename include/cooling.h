#ifndef COOLING_H
#define COOLING_H

struct cooling_fan {
  unsigned char common_status;          /* for status only */
  unsigned char common_control;         /* for control only */
  int rpm;
  char *name;
  int page_two_offset;  /* for control */
};

extern void print_cooling_fan(struct cooling_fan *fan);

extern int extract_cooling_fan_info(
  unsigned char *cooling_element,
  unsigned char *fan_description,
  struct cooling_fan *fan,
  int page_two_offset);

#endif
