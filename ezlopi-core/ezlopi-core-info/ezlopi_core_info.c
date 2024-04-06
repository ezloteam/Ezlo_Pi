#include <string.h>

#include "freertos/FreeRTOSConfig.h"
#include "esp_netif_ip_addr.h"
#include "ezlopi_core_info.h"

static const uint8_t EZPI_UART_SERV_FLW_CTRL_STR_SIZE = 10;

uart_parity_t EZPI_CORE_info_set_parity(const char* parity)
{
    uart_parity_t parity_val = UART_PARITY_DISABLE;

    if (strncmp("N", parity, 1) == 0)
    {
        parity_val = UART_PARITY_DISABLE;
    }
    else if (strncmp("P", parity, 1) == 0)
    {
        parity_val = UART_PARITY_ODD;
    }
    else if (strncmp("E", parity, 1) == 0)
    {
        parity_val = UART_PARITY_EVEN;
    }
    return parity_val;
}

char EZPI_CORE_info_get_parity(const uart_parity_t parity_val)
{
    char parity = 'N';
    switch (parity_val)
    {
    case UART_PARITY_ODD:
    {
        parity = 'P';
        break;
    }
    case UART_PARITY_EVEN:
    {
        parity = 'E';
        break;
    }
    case UART_PARITY_DISABLE:
    default:
    {
        parity = 'N';
        break;
    }
    }
    return parity;
}

void EZPI_CORE_info_get_flow_ctrl_to_str(uart_hw_flowcontrol_t flw_ctrl, char* flw_ctrl_str)
{
    switch (flw_ctrl)
    {
    case UART_HW_FLOWCTRL_DISABLE:
        strncpy(flw_ctrl_str, "DISABLED", EZPI_UART_SERV_FLW_CTRL_STR_SIZE);
        break;
    case UART_HW_FLOWCTRL_RTS:
        strncpy(flw_ctrl_str, "RTS", EZPI_UART_SERV_FLW_CTRL_STR_SIZE);
        break;
    case UART_HW_FLOWCTRL_CTS:
        strncpy(flw_ctrl_str, "CTS", EZPI_UART_SERV_FLW_CTRL_STR_SIZE);
        break;
    case UART_HW_FLOWCTRL_CTS_RTS:
        strncpy(flw_ctrl_str, "CTS_RTS", EZPI_UART_SERV_FLW_CTRL_STR_SIZE);
        break;
    default:
        strncpy(flw_ctrl_str, "DISABLED", EZPI_UART_SERV_FLW_CTRL_STR_SIZE);
        break;
    }
}

uart_hw_flowcontrol_t EZPI_CORE_info_get_flow_ctrl_frm_str(char* flw_ctrl_str)
{
    uart_hw_flowcontrol_t flw_ctrl = UART_HW_FLOWCTRL_DISABLE;
    if (strncmp(flw_ctrl_str, "DISABLED", EZPI_UART_SERV_FLW_CTRL_STR_SIZE) == 0)
    {
        flw_ctrl = UART_HW_FLOWCTRL_DISABLE;
    }
    else if (strncmp(flw_ctrl_str, "RTS", EZPI_UART_SERV_FLW_CTRL_STR_SIZE) == 0)
    {
        flw_ctrl = UART_HW_FLOWCTRL_RTS;
    }
    else if (strncmp(flw_ctrl_str, "CTS", EZPI_UART_SERV_FLW_CTRL_STR_SIZE) == 0)
    {
        flw_ctrl = UART_HW_FLOWCTRL_CTS;
    }
    else if (strncmp(flw_ctrl_str, "CTS_RTS", EZPI_UART_SERV_FLW_CTRL_STR_SIZE) == 0)
    {
        flw_ctrl = UART_HW_FLOWCTRL_CTS_RTS;
    }
    else
    {
        flw_ctrl = UART_HW_FLOWCTRL_DISABLE;
    }
    return flw_ctrl;
}

uart_word_length_t EZPI_CORE_info_get_frame_size(const uint32_t frame_size)
{
    uart_word_length_t frame_size_val = UART_DATA_8_BITS;
    if (5 == frame_size)
    {
        frame_size_val = UART_DATA_5_BITS;
    }
    else if (6 == frame_size)
    {
        frame_size_val = UART_DATA_6_BITS;
    }
    else if (7 == frame_size)
    {
        frame_size_val = UART_DATA_7_BITS;
    }
    return frame_size_val;
}

char* EZPI_CORE_info_esp_reset_reason_str(esp_reset_reason_t reason)
{
    switch (reason)
    {
    case ESP_RST_UNKNOWN:
        return "unknown";
        break;
    case ESP_RST_POWERON:
        return "Power ON";
        break;
    case ESP_RST_EXT:
        return "External pin";
        break;
    case ESP_RST_SW:
        return "Software Reset via esp_restart";
        break;
    case ESP_RST_PANIC:
        return "Software reset due to exception/panic";
        break;
    case ESP_RST_INT_WDT:
        return "Interrupt watchdog";
        break;
    case ESP_RST_TASK_WDT:
        return "Task watchdog";
        break;
    case ESP_RST_WDT:
        return "Other watchdogs";
        break;
    case ESP_RST_DEEPSLEEP:
        return "Reset after exiting deep sleep mode";
        break;
    case ESP_RST_BROWNOUT:
        return "Brownout reset";
        break;
    case ESP_RST_SDIO:
        return "Reset over SDIO";
        break;
    default:
        return "unknown";
        break;
    }
}

char* EZPI_CORE_info_chip_type_str(int chip_type)
{
    switch (chip_type)
    {
    case CHIP_ESP32:
        return "ESP32";
        break;
    case CHIP_ESP32S2:
        return "ESP32-S2";
        break;
    case CHIP_ESP32S3:
        return "ESP32-S3";
        break;
    case CHIP_ESP32C3:
        return "ESP32-C3";
        break;
    case CHIP_ESP32H2:
        return "ESP32-H2";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}

char* EZPI_CORE_info_get_wifi_mode(const wifi_mode_t mode_val)
{
    char* mode = NULL;
    switch (mode_val)
    {
    case WIFI_MODE_STA:
    {
        mode = "STA";
        break;
    }
    case WIFI_MODE_AP:
    {
        mode = "AP";
        break;
    }
    case WIFI_MODE_APSTA:
    {
        mode = "APSTA";
        break;
    }
    default:
    {
        break;
    }
    }
    return mode;
}

void EZPI_CORE_info_tick_to_time(char* time_buff, uint32_t buff_len, uint32_t ms)
{
    uint32_t seconds = ms / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    uint32_t days = hours / 24;

    seconds %= 60;
    minutes %= 60;
    hours %= 24;

    if (time_buff && buff_len)
    {
        memset(time_buff, 0, buff_len);
        snprintf(time_buff, buff_len, "%dd %dh %dm %ds", days, hours, minutes, seconds);
    }
}