#include "array_device_slot.h"
#include "common.h"
#include "json.h"
#include "ses.h"
#include "sg_lib.h"
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

const char* status_str(struct array_device_slot slot) {
  if ((slot.common_status & 0xf) == 0x5) {
    return "Not Installed";
  } else if (slot.device_off) {
    return "Power Off";
  } else {
    return "Power On";
  }
}

const char *fault_led_status_str(int fault_code) {
  const char *fault_led_status[] = {"None", "Requested", "Sensed", "Sensed"};
  return fault_led_status[fault_code];
}

void print_array_device_slot(struct array_device_slot *slot)
{
  const char *device_status_str = status_str(*slot);

  IF_PRINT_NONE_JSON {
    printf("%s Slot:\t%d\tPhy: %d\t%s\tSAS Addr: 0x%s\t"
           "Attached SAS Addr: 0x%s\tFault: %s",
           slot->name, slot->slot, slot->phy, device_status_str,
           slot->sas_addr_str, slot->attached_sas_addr_str,
           fault_led_status_str(slot->fault));
    if (slot->dev_name) {
      printf("\t%s", slot->dev_name);
      printf("\t%s", slot->by_slot_name);
    }
    printf("\n");
  }

  PRINT_JSON_GROUP_HEADER(slot->name);
  PRINT_JSON_ITEM("name", "%s", slot->name);
  PRINT_JSON_ITEM("slot", "%d", slot->slot);
  PRINT_JSON_ITEM("phy", "%d", slot->phy);
  PRINT_JSON_ITEM("status", "%s", device_status_str);
  PRINT_JSON_ITEM("sas_addr", "0x%s", slot->sas_addr_str);
  if (slot->dev_name) {
    PRINT_JSON_ITEM("fault", "%s", fault_led_status_str(slot->fault));
    PRINT_JSON_ITEM("devname", "%s", slot->dev_name);
    PRINT_JSON_LAST_ITEM("by_slot_name", "%s", slot->by_slot_name);
  } else
    PRINT_JSON_LAST_ITEM("fault", "%s", fault_led_status_str(slot->fault));

  PRINT_JSON_GROUP_ENDING;
}

int extract_array_device_slot_info(
  unsigned char *array_device_slot_element,
  unsigned char *additional_slot_element,
  unsigned char *slot_description,
  int page_two_offset,
  struct array_device_slot *slot)
{
  switch (0xf & additional_slot_element[0]) {     /* switch on protocol identifier */
    case 0x6:    //TPROTO_SAS
      verify_additional_element_eip_sas(additional_slot_element);

      slot->common_status = array_device_slot_element[0];
      slot->slot = (int) additional_slot_element[3];
      slot->phy = -1;  /* update phy later in the expander info */
      slot->fault = (array_device_slot_element[3] & 0x60) >> 5;
      slot->ident = array_device_slot_element[2] & 0x02 ? 1 : 0;
      slot->active = 0;
      slot->device_off = array_device_slot_element[3] & 0x10 ? 1 : 0;

      memcpy(slot->attached_sas_addr, additional_slot_element + 12, 8);
      print_sas_addr_a(slot->attached_sas_addr, slot->attached_sas_addr_str);

      memcpy(slot->sas_addr, additional_slot_element + 20, 8);
      print_sas_addr_a(slot->sas_addr, slot->sas_addr_str);

      slot->name = copy_description(slot_description);
    
      slot->page_two_offset = page_two_offset;
      slot->dev_name = NULL;
      slot->by_slot_name = NULL;
      break;
    case 0xb:    //TPROTO_PCIE added in ses3r08; contains little endian fields
      verify_additional_element_eip_pcie(additional_slot_element);

      slot->common_status = array_device_slot_element[0];
      slot->slot = (int) additional_slot_element[3];
      slot->phy = -1;  /* update phy later in the expander info */
      slot->fault = (array_device_slot_element[3] & 0x60) >> 5;
      slot->ident = array_device_slot_element[2] & 0x02 ? 1 : 0;
      slot->active = 0;
      slot->device_off = array_device_slot_element[3] & 0x10 ? 1 : 0;

      memcpy(slot->attached_sas_addr, additional_slot_element + 12, 8);
      print_sas_addr_a(slot->attached_sas_addr, slot->attached_sas_addr_str);

      memcpy(slot->sas_addr, additional_slot_element + 20, 8);
      print_sas_addr_a(slot->sas_addr, slot->sas_addr_str);

      slot->name = copy_description(slot_description);
    
      slot->page_two_offset = page_two_offset;
      slot->dev_name = NULL;
      slot->by_slot_name = NULL;
      break;
//      printf("%sTransport protocol: PCIe\n", pad);
//      if (0 == eip_offset) {
//        printf("%sfor this protocol EIP must be set (it isn't)\n", pad);
//        break;
//      }
//      if (len < 6)
//        break;
//      pcie_pt = (ae_bp[5] >> 5) & 0x7;
//      if (TPROTO_PCIE_PS_NVME == pcie_pt)
//        printf("%sPCIe protocol type: NVMe\n", pad);
//      else {  /* no others currently defined */
//        printf("%sTransport protocol: PCIe subprotocol=0x%x not "
//               "decoded\n", pad, pcie_pt);
//        if (op->verbose)
//            hex2stdout(ae_bp, len, 0);
//        break;
//      }
//      phys = ae_bp[4];
//      printf("%snumber of ports: %d, not all ports: %d", pad, phys,
//             ae_bp[5] & 1);
//      printf(", device slot number: %d\n", ae_bp[7]);
//
//      pcie_vid = sg_get_unaligned_le16(ae_bp + 10);   /* N.B. LE */
//      printf("%sPCIe vendor id: 0x%" PRIx16 "%s\n", pad, pcie_vid,
//             (0xffff == pcie_vid) ? " (not reported)" : "");
//      printf("%sserial number: %.20s\n", pad, ae_bp + 12);
//      printf("%smodel number: %.40s\n", pad, ae_bp + 32);
//      aep = ae_bp + 72;
//      for (j = 0; j < phys; ++j, aep += 8) {
//        printf("%sport index: %d\n", pad, j);
//        psn_valid = !!(0x4 & aep[0]);
//        bdf_valid = !!(0x2 & aep[0]);
//        cid_valid = !!(0x1 & aep[0]);
//        printf("%s  PSN_VALID=%d, BDF_VALID=%d, CID_VALID=%d\n", pad,
//               (int)psn_valid, (int)bdf_valid, (int)cid_valid);
//        if (cid_valid)      /* N.B. little endian */
//          printf("%s  controller id: 0x%" PRIx16 "\n", pad,
//                 sg_get_unaligned_le16(aep + 1)); /* N.B. LEndian */
//        if (bdf_valid)
//          printf("%s  bus number: 0x%x, device number: 0x%x, "
//                 "function number: 0x%x\n", pad, aep[4],
//                 (aep[5] >> 3) & 0x1f, 0x7 & aep[5]);
//        if (psn_valid)      /* little endian, top 3 bits assumed zero */
//          printf("%s  physical slot number: 0x%" PRIx16 "\n", pad,
//                 0x1fff & sg_get_unaligned_le16(aep + 6)); /* N.B. LE */
//      }
//      break;
    default:
      perr("Transport protocol: %d not decoded\n", 0xf & additional_slot_element[0]);
#ifdef DEBUG
      char bb[1024];
      dStrHexStr(additional_slot_element, 255, " ", 0, sizeof(bb), bb);
      perr("additional elements: %s\n", bb);
#endif
      break;
    }
  return 0;
}

int control_hdd_power(
  unsigned char *page_two,
  struct array_device_slot *slot,
  int op /* 0 for power off; 1 for power on */)
{
  slot->common_control = slot->common_status & 0xf0;
  slot->common_control |= 0x80;

  page_two[slot->page_two_offset] = slot->common_control;
  if (op) {
    page_two[slot->page_two_offset + 3] &= 0xef;
  } else {
    page_two[slot->page_two_offset + 3] |= 0x10;
  }
  return 0;
}

/* return whether the HDD is powered on */
int check_hdd_power(
  unsigned char *page_two,
  struct array_device_slot *slot)
{
  /* this bit is DEVICE_OFF, so we need to reverse it */
  return !(page_two[slot->page_two_offset + 3] & 0x10);
}

int control_hdd_led_fault(
  unsigned char *page_two,
  struct array_device_slot *slot,
  int op  /* 0 for clear fault request; 1 for request fault */)
{
#ifdef DEBUG
      char bb[16];
      dStrHexStr(&page_two[slot->page_two_offset], 4, " ", 0, sizeof(bb), bb);
      perr("control_hdd_led_fault: %s\n", bb);
#endif
  slot->common_control = slot->common_status & 0xf0;
  slot->common_control |= 0x80;

  page_two[slot->page_two_offset] = slot->common_control;
  if (op) {
    page_two[slot->page_two_offset + 2] |= 0x02;
  } else {
    page_two[slot->page_two_offset + 2] &= 0xfd;
  }
#ifdef DEBUG
      char bb1[16];
      dStrHexStr(&page_two[slot->page_two_offset], 4, " ", 0, sizeof(bb1), bb1);
      perr("control_hdd_led_fault: %s\n", bb1);
#endif
  return 0;
}

int find_dev_name(
  struct array_device_slot *slot,
  char *expander_addr)
{
  char link_name[PATH_MAX];
  char real_name[PATH_MAX];
  DIR *dir;
  struct dirent *ent;
  char *sys_block = "/sys/block";
  char sas_address_filename[PATH_MAX];
  int sas_address_fd;
  char sas_address_file_content[4096];
  struct stat st;
  int read_len;

  snprintf(link_name, PATH_MAX, "%s/enclosure-0x%s-slot%d",
           DEV_DISK_BY_SLOT, expander_addr, slot->slot);

  unlink(link_name);

  if (sas_addr_invalid(slot->sas_addr))
    return 0;

  dir = opendir(sys_block);
  if (dir == NULL)
    return 0;
  while ((ent = readdir(dir)) != NULL) {
    if (strncmp(ent->d_name, "sd", 2))  /* only check sd devices */
      continue;

    snprintf(sas_address_filename, PATH_MAX, "%s/%s/device/sas_address",
             sys_block, ent->d_name);
    if (access(sas_address_filename, R_OK) != 0)
      continue;
    if ((sas_address_fd = open(sas_address_filename, O_RDONLY)) < 0)
      continue;

    read_len = read(sas_address_fd, sas_address_file_content, 4096);
    close(sas_address_fd);

    if (read_len < SAS_ADDR_LENGTH * 2 + 2)
      continue;
    if (strncmp(sas_address_file_content + 2,
                slot->sas_addr_str, SAS_ADDR_LENGTH * 2) == 0) {
      snprintf(real_name, PATH_MAX, "/dev/%s", ent->d_name);
      if ((stat(real_name, &st) == 0) &&
          ((st.st_mode & S_IFMT) == S_IFBLK)) {
        slot->dev_name = strndup(real_name, PATH_MAX);
        if (symlink(real_name, link_name) == 0)
          slot->by_slot_name = strndup(link_name, PATH_MAX);
      }
      break;
    }
  }
  closedir(dir);

  return 0;
}
