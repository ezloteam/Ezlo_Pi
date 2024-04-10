#ifndef EZLOPI_CORE_INFO_H
#define EZLOPI_CORE_INFO_H

#include "driver/uart.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_idf_version.h"
#include "ezlopi_util_version.h"
#include "sdkconfig.h"

uart_parity_t EZPI_CORE_info_set_parity(const char* parity);
char EZPI_CORE_info_get_parity(const uart_parity_t parity_val);
void EZPI_CORE_info_get_flow_ctrl_to_str(uart_hw_flowcontrol_t flw_ctrl, char* flw_ctrl_str);
uart_hw_flowcontrol_t EZPI_CORE_info_get_flow_ctrl_frm_str(char* flw_ctrl_str);
uart_word_length_t EZPI_CORE_info_get_frame_size(const uint32_t frame_size);
char* EZPI_CORE_info_esp_reset_reason_str(esp_reset_reason_t reason);
char* EZPI_CORE_info_chip_type_str(int chip_type);
char* EZPI_CORE_info_get_wifi_mode(const wifi_mode_t mode_val);
void EZPI_CORE_info_tick_to_time(char* time_buff, uint32_t buff_len, uint32_t ms);

#endif // EZLOPI_CORE_INFO_H
