#ifndef ENCLOSURE_INFO_H
#define ENCLOSURE_INFO_H

#define VENDOR_ID_LENGTH   8
#define PRODUCT_ID_LENGTH  16
#define REVISION_LENGTH    4

#include "common.h"

struct enclosure_descriptor {
  SAS_ADDR(sas_addr);
  SAS_ADDR_STR(sas_addr_str);
  char vendor[VENDOR_ID_LENGTH];
  char product[PRODUCT_ID_LENGTH];
  char revision[REVISION_LENGTH];
};

struct enclosure_control {
  int page_two_offset;
};

extern int extract_enclosure_control(
  unsigned char *enclosure_element,
  int page_two_offset,
  struct enclosure_control *enclosurep);

extern int power_cycle_enclosure(
  unsigned char *page_two,
  struct enclosure_control *enclosurep);

#endif
