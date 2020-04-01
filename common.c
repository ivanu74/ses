#include "common.h"

void print_sas_addr_a(unsigned char *sas_addr, char *sas_addr_str)
{
  int i;
  for (i = 0; i < SAS_ADDR_LENGTH; i ++) {
    snprintf(sas_addr_str + 2 * i, SAS_ADDR_STR_LENGTH + 1 - 2 * i,
             "%02x", sas_addr[i]);
  }
}

int sas_addr_invalid(unsigned char *addr)
{
  if (addr == NULL)
    return 1;

  /* if first 4 bytes are all zero, the address is invalid */
  return (addr[0] == 0 && addr[1] == 0 && addr[2] == 0 && addr[3] == 0);
}

/* replace all non-ASCII char with space */
void fix_none_ascii(char *buf, int len)
{
  int i;

  for (i = 0; i < len; ++i)
    if (buf[i] < 0x20 || buf[i] == 0x7f || buf[i] == '"' || buf[i] == '`')
      /* remove none ASCII, ", and ` (workaround HoneyBadger bug) */
      buf[i] = 0x20;
}
