#ifndef EXPANDER_H
#define EXPANDER_H

#include "common.h"
#include "array_device_slot.h"

struct sas_expander {
  SAS_ADDR(sas_addr);
  SAS_ADDR_STR(sas_addr_str);
  char *name;
};

extern void print_expander_info(struct sas_expander *expander);

extern int extract_expander_info(
  unsigned char *expander_element,
  unsigned char *additional_expander_element,
  unsigned char *expander_description,
  struct sas_expander *expander,
  struct array_device_slot *slots);

#endif
