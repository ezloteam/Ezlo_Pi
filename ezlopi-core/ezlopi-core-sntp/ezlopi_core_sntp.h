#ifndef _EZLOPI_CORE_SNTP_H_
#define _EZLOPI_CORE_SNTP_H_

#include <time.h>

void EZPI_CORE_sntp_init(void);
int EZPI_CORE_sntp_set_location(const char* location);
char* EZPI_CORE_sntp_get_location(void);
void EZPI_CORE_sntp_get_up_time(char* up_time_buf, uint32_t buf_len);
void EZPI_CORE_sntp_get_local_time(char* local_time_buf, uint32_t buf_len);
void EZPI_CORE_sntp_epoch_to_iso8601(char* time_buf, uint32_t buf_len, time_t t);
uint64_t EZPI_CORE_sntp_get_current_time_ms(void);
uint64_t EZPI_CORE_sntp_get_current_time_sec(void);

#endif // _EZLOPI_CORE_SNTP_H_
