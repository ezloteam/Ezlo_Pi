#ifndef _EZLOPI_CORE_SNTP_H_
#define _EZLOPI_CORE_SNTP_H_

void EZPI_CORE_sntp_init(void);
int EZPI_CORE_sntp_set_location(const char *location);
char *EZPI_CORE_sntp_get_location(void);
char *EZPI_CORE_sntp_get_up_time(void);
char *EZPI_CORE_sntp_get_local_time(void);
char *EZPI_CORE_sntp_epoch_to_iso8601(time_t t);

#endif // _EZLOPI_CORE_SNTP_H_
