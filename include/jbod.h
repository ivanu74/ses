#ifndef _MY_OCPJBOD_H_
#define _MY_OCPJBOD_H_

#ifdef __cplusplus
extern "C" {
#endif

	int get_profile(const char* name, char* out);
	int get_serial(const char* name, char* out);
	int get_sas_info(const char* name, char* out);
	int set_hdd_led(const char* name, int id, int op);

#ifdef __cplusplus
}
#endif

#endif // _MY_OCPJBOD_H_