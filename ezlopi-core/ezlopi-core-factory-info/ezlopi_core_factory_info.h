/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_factory_info.h
 * @brief   perform some function on factory-data
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
*/
#ifndef _EZLOPI_CORE_FACTORY_INFO_H_
#define _EZLOPI_CORE_FACTORY_INFO_H_

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include <esp_partition.h>

#include "cjext.h"
#include "EZLOPI_USER_CONFIG.h"

#include "ezlopi_core_errors.h"

/*******************************************************************************
*                          C++ Declaration Wrapper
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
    *                          Type & Macro Declarations
    *******************************************************************************/
#define EZLOPI_DEVICE_TYPE_TEST_DEVICE -1
#define EZLOPI_DEVICE_TYPE_GENERIC 0

#if CONFIG_EZPI_DEV_TYPE_TEST == 1
#define EZLOPI_DEVICE_TYPE EZLOPI_DEVICE_TYPE_TEST_DEVICE
#else
#define EZLOPI_DEVICE_TYPE EZLOPI_DEVICE_TYPE_GENERIC
#endif

#define EZLOPI_FACTORY_INFO_V3_PARTITION_NAME "prov_data"
#define EZLOPI_FACTORY_INFO_V3_PARTITION_SIZE 0x8000 // 32KB
#define EZLOPI_FACTORY_INFO_V3_PARTITION_TYPE 0x40
#define EZLOPI_FACTORY_INFO_V3_SUBTYPE ESP_PARTITION_SUBTYPE_APP_FACTORY // ESP_PARTITION_SUBTYPE_ANY

#define EZLOPI_FINFO_OFFSET_CONN_DATA 0x0000
#define EZLOPI_FINFO_OFFSET_HUB_DATA 0x6000

#define EZLOPI_FINFO_READ_LEN_4KB 4096

    typedef enum e_factory_info_v3_partition_type
    {
        E_EZLOPI_FACTORY_INFO_CONN_DATA,
        E_EZLOPI_FACTORY_INFO_HUB_DATA,
        E_EZLOPI_FACTORY_INFO_MAX
    } e_factory_info_v3_partition_type_t;

    typedef enum e_EZPI_core_factory_info_v3_offset
    {
        // Offset Connection Data
        EZLOPI_FINFO_REL_OFFSET_CONFIG_VERSION = 0x02,
        EZLOPI_FINFO_REL_OFFSET_PROVISIONING_UUID = 0x0004,
        EZLOPI_FINFO_REL_OFFSET_DEVICE_UUID = 0x0082,
        EZLOPI_FINFO_REL_OFFSET_PROVISIONING_SERVER_URL = 0X100,
        EZLOPI_FINFO_REL_OFFSET_PROVISIONING_TOKEN = 0X200,
        EZLOPI_FINFO_REL_OFFSET_CLOUD_SERVER_URL = 0x0400,
        EZLOPI_FINFO_REL_OFFSET_LOCAL_KEY = 0x0500,
        EZLOPI_FINFO_REL_OFFSET_EZLOPI_CONFIG_JSON = 0x1000,
        EZLOPI_FINFO_REL_OFFSET_CA_CERTIFICATE = 0x3000,
        EZLOPI_FINFO_REL_OFFSET_SSL_PRIVATE_KEY = 0x4000,
        EZLOPI_FINFO_REL_OFFSET_SSL_SHARED_KEY = 0x5000,

        // Offset Hub Data
        EZLOPI_FINFO_REL_OFFSET_SERIAL_NUMBER = 0,
        EZLOPI_FINFO_REL_OFFSET_FMW_VERSION = 0x0002,
        EZLOPI_FINFO_REL_OFFSET_FMW_BUILD = 0x0006,
        EZLOPI_FINFO_REL_OFFSET_DEVICE_ID = 0x0008,
        EZLOPI_FINFO_REL_OFFSET_WIFI_SSID = 0x0020,
        EZLOPI_FINFO_REL_OFFSET_WIFI_PASS = 0x0040,
        EZLOPI_FINFO_REL_OFFSET_DEVICE_MAC = 0x0080,
        EZLOPI_FINFO_REL_OFFSET_DEVICE_NAME = 0x00C0,
        EZLOPI_FINFO_REL_OFFSET_MANUF_NAME = 0x0100,
        EZLOPI_FINFO_REL_OFFSET_BRAND_NAME = 0x0140,
        EZLOPI_FINFO_REL_OFFSET_MODEL_NAME = 0x0180,
        EZLOPI_FINFO_REL_OFFSET_EZLOPI_DEVICE_TYPE = 0x01C0

    } e_EZPI_core_factory_info_v3_offset_t;

    typedef enum e_EZPI_core_factory_info_v3_length
    {
        EZLOPI_FINFO_LEN_VERSION = 0x0004,
        EZLOPI_FINFO_LEN_CONFIG_VERSION = 0x0002,
        EZLOPI_FINFO_LEN_DEVICE_NAME = 0x0040,
        EZLOPI_FINFO_LEN_MANUF_NAME = 0x0040,
        EZLOPI_FINFO_LEN_BRAND_NAME = 0x0040,
        EZLOPI_FINFO_LEN_MODEL_NAME = 0x0040,
        EZLOPI_FINFO_LEN_DEVICE_ID = 0x0008,
        EZLOPI_FINFO_LEN_DEVICE_UUID = 0x007E,
        EZLOPI_FINFO_LEN_PROV_UUID = 0x007E,
        EZLOPI_FINFO_LEN_WIFI_SSID = 0x0020,
        EZLOPI_FINFO_LEN_WIFI_PASS = 0x0040,
        EZLOPI_FINFO_LEN_DEVICE_MAC = 0x0040,
        EZLOPI_FINFO_LEN_CLOUD_SERVER_URL = 0x00100,
        EZLOPI_FINFO_LEN_LOCAL_KEY = 0X100,
        EZLOPI_FINFO_LEN_EZLOPI_DEVICE_TYPE = 0x0040,
        EZLOPI_FINFO_LEN_CA_CERTIFICATE = 0x1000,
        EZLOPI_FINFO_LEN_SSL_PRIVATE_KEY = 0x1000,
        EZLOPI_FINFO_LEN_SSL_SHARED_KEY = 0x1000,
        EZLOPI_FINFO_LEN_EZLOPI_CONFIG_JSON = 0x1000,
        EZLOPI_FINFO_LEN_PROVISIONING_TOKEN = 0x200,
        EZLOPI_FINFO_LEN_PROVISIONING_SERVER_URL = 0x100,
    } e_EZPI_core_factory_info_v3_length_t;

    typedef struct s_basic_factory_info
    {
        int16_t version;
        char *device_name;
        char *manufacturer;
        char *brand;
        char *model_number;
        unsigned long long id;
        char *device_uuid;
        char *prov_uuid;
        // char *wifi_ssid;
        // char *wifi_password;
        char *device_mac;
        char *cloud_server;
        char *device_type;
        char *provision_server;
        char *provision_token;
        char *user_id;
        char *local_key;
        uint16_t config_version;
    } s_basic_factory_info_t;

    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/


#if (EZLOPI_DEVICE_TYPE_GENERIC == EZLOPI_DEVICE_TYPE)

#elif (EZLOPI_DEVICE_TYPE_TEST_DEVICE == EZLOPI_DEVICE_TYPE)
#if defined(CONFIG_IDF_TARGET_ESP32)

    static const char *ezlopi_config_test =
        "{\
    \"cmd\":3,\
    \"dev_detail\":[\
        {\
        \"dev_type\":5,\
        \"dev_name\":\"Dimmer_riken\",\
        \"id_room\":\"\",\
        \"id_item\":22,\
        \"gpio\":18,\
        \"pwm_resln\":3,\
        \"freq_hz\":50,\
        \"duty_cycle\":0\
        },\
        {\
            \"dev_type\":1,\
            \"dev_name\":\"switch_temp_riken\",\
            \"id_room\":\"\",\
            \"id_item\":2,\
            \"gpio_in\":0,\
            \"gpio_out\":25,\
            \"pullup_ip\":false,\
            \"pullup_op\":false,\
            \"is_ip\":false,\
            \"ip_inv\":false,\
            \"op_inv\":false,\
            \"val_ip\":false,\
            \"val_op\":false\
        },\
        {\
            \"dev_type\":1,\
            \"dev_name\":\"Bulb_riken\",\
            \"id_room\":\"\",\
            \"id_item\":1,\
            \"gpio_in\":\"\",\
            \"gpio_out\":22,\
            \"pullup_ip\":false,\
            \"pullup_op\":false,\
            \"is_ip\":false,\
            \"ip_inv\":false,\
            \"op_inv\":false,\
            \"val_ip\":false,\
            \"val_op\":false\
        },\
        {\
            \"dev_type\":7,\
            \"dev_name\":\"DHT22_temp_humi_riken\",\
            \"id_room\":\"\",\
            \"id_item\":16,\
            \"gpio\":21\
        }\
    ],\
    \"config_id\":\"1234567\",\
    \"config_time\":1696508363,\
    \"config_name\":\"My moisture sensor\",\
    \"chipset\":\"ESP32\"\
}";
    // "{\"cmd\":3,\"dev_detail\":[{\"dev_type\":8,\"dev_name\":\"BME 280\",\"id_room\":\"\",\"id_item\":12,\"gpio_sda\":21,\"gpio_scl\":22,\"pullup_scl\":true,\"pullup_sda\":true,\"slave_addr\":118},{\"dev_type\":8,\"dev_name\":\"TSL2561_luminosity\",\"id_room\":\"\",\"id_item\":44,\"gpio_sda\":21,\"gpio_scl\":22,\"slave_addr\":57}],\"config_id\":\"1234567\",\"config_time\":1696508363,\"config_name\":\"My moisture sensor\",\"chipset\":\"ESP32\"}";
    // static const char * ezlopi_config_test = "{\"config_id\":1260701997,\"config_time\":1722516114,\"config_name\":\"Washing Machine Leak \",\"cmd\":4,\"dev_total\":2,\"dev_detail\":[{\"dev_type\":3,\"dev_name\":\"Washing Machine Leak Detector\",\"id_item\":27,\"id_room\":\"\",\"resln_bit\":10,\"gpio\":32,\"id\":\"83cd1aae-0a76-bf7e-1475-863ea1464607\",\"devType\":\"Analog Input\",\"device_id\":\"10e52000\"},{\"dev_type\":1,\"id_item\":2,\"dev_name\":\"Washing Machine Leak Buzzer\",\"gpio_in\":\"\",\"gpio_out\":2,\"pullup_ip\":false,\"pullup_op\":true,\"is_ip\":false,\"ip_inv\":false,\"op_inv\":false,\"val_ip\":false,\"val_op\":false,\"id\":\"bc44fa36-f5a5-1f80-54aa-f564210faf55\",\"devType\":\"Digital Output\",\"device_id\":\"10e52001\"}],\"chipset\":\"ESP32\"}";
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    static const char *ezlopi_config_test =
        "{\
    \"cmd\":3,\
    \"dev_detail\":[\
        {\
        \"dev_type\":5,\
        \"dev_name\":\"Dimmer\",\
        \"id_room\":\"\",\
        \"id_item\":22,\
        \"gpio\":18,\
        \"pwm_resln\":3,\
        \"freq_hz\":50,\
        \"duty_cycle\":0\
        },\
        {\
            \"dev_type\":1,\
            \"dev_name\":\"switch_temp\",\
            \"id_room\":\"\",\
            \"id_item\":2,\
            \"gpio_in\":0,\
            \"gpio_out\":25,\
            \"pullup_ip\":false,\
            \"pullup_op\":false,\
            \"is_ip\":false,\
            \"ip_inv\":false,\
            \"op_inv\":false,\
            \"val_ip\":false,\
            \"val_op\":false\
        },\
        {\
            \"dev_type\":1,\
            \"dev_name\":\"Bulb\",\
            \"id_room\":\"\",\
            \"id_item\":1,\
            \"gpio_in\":\"\",\
            \"gpio_out\":22,\
            \"pullup_ip\":false,\
            \"pullup_op\":false,\
            \"is_ip\":false,\
            \"ip_inv\":false,\
            \"op_inv\":false,\
            \"val_ip\":false,\
            \"val_op\":false\
        },\
        {\
            \"dev_type\":7,\
            \"dev_name\":\"DHT22_temp_humi\",\
            \"id_room\":\"\",\
            \"id_item\":16,\
            \"gpio\":21\
        }\
    ],\
    \"config_id\":\"1234567\",\
    \"config_time\":1696508363,\
    \"config_name\":\"My moisture sensor\",\
    \"chipset\":\"ESP32S3\"\
}";
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    static const char *ezlopi_config_test = "{\"cmd\":3,\"dev_detail\":[{\"dev_type\":1,\"dev_name\":\"switch_temp\",\"id_room\":\"\",\"id_item\":2,\"gpio_in\":0,\"gpio_out\":2,\"pullup_ip\":false,\"pullup_op\":false,\"is_ip\":false,\"ip_inv\":false,\"op_inv\":false,\"val_ip\":false,\"val_op\":false},{\"dev_type\":1,\"dev_name\":\"Bulb\",\"id_room\":\"\",\"id_item\":1,\"gpio_in\":0,\"gpio_out\":3,\"pullup_ip\":false,\"pullup_op\":false,\"is_ip\":false,\"ip_inv\":false,\"op_inv\":false,\"val_ip\":false,\"val_op\":false},{\"dev_type\":7,\"dev_name\":\"DHT22_temp_humi\",\"id_room\":\"\",\"id_item\":16,\"gpio\":1}],\"config_id\":\"1234567\",\"config_time\":1696508363,\"config_name\":\"My moisture sensor\",\"chipset\":\"ESP32S2\"}";
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    static const char *ezlopi_config_test = "{\"cmd\":3,\"dev_detail\":[],\"config_id\":\"1234567\",\"config_time\":1696508363,\"config_name\":\"My moisture sensor\",\"chipset\":\"ESP32C3\"}";
#endif
#endif

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/
    /**
     * @brief Function to get absolute address of target partition.
     *
     * @param relative_offset Offset of the partition
     * @param partition_type Partition-Type of target
     * @return uint32_t
     */
    uint32_t EZPI_core_factory_info_v3_get_abs_address(uint32_t relative_offset, e_factory_info_v3_partition_type_t partition_type);
    /**
     * @brief Function to print factory info
     *
     */
    void EZPI_print_factory_info_v3(void);
    /**
     * @brief Funntion to initialize factory-info
     *
     * @return const esp_partition_t*
     */
    const esp_partition_t *EZPI_factory_info_v3_init(void);
    /**
     * @brief Function to get provisioning status from factory info
     *
     * @return uint32_t
     */
    uint32_t EZPI_core_factory_info_v3_get_provisioning_status(void);
    /**
     * @brief Function to get version from factory info
     *
     * @return uint16_t
     */
    uint16_t EZPI_core_factory_info_v3_get_version(void);
    /**
     * @brief Function to get name from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_name(void);
    /**
     * @brief Function to get manufacturer_name from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_manufacturer(void);
    /**
     * @brief Function to get brand_name from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_brand(void);
    /**
     * @brief Function to get model_num from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_model(void);
    /**
     * @brief Function to get dev_ID from factory info
     *
     * @return unsigned long long
     */
    unsigned long long EZPI_core_factory_info_v3_get_id(void);
    /**
     * @brief Function to get dev_uuid from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_device_uuid(void);
    /**
     * @brief Function to get prov_uuid from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_provisioning_uuid(void);
    /**
     * @brief Function to get ssid from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_ssid(void);
    /**
     * @brief Function to get password from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_password(void);
    /**
     * @brief Function to get device-MAC from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_ezlopi_mac(void);
    /**
     * @brief Function to get cloud-server from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_cloud_server(void);
    /**
     * @brief Function to get device-type from factory info
     *
     * @return const char*
     */
    const char *EZPI_core_factory_info_v3_get_device_type(void);
    /**
     * @brief Function to get local_key from factory info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_local_key(void);
    /**
     * @brief Function to free ca certificate
     *
     */
    void EZPI_core_factory_info_v3_free_ca_certificate(void);
    /**
     * @brief Function to free ssl private_key
     *
     */
    void EZPI_core_factory_info_v3_free_ssl_private_key(void);
    /**
     * @brief Function to free ssl shared_key
     *
     */
    void EZPI_core_factory_info_v3_free_ssl_shared_key(void);
    /**
     * @brief Function to get ca_certificate from factory-info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_ca_certificate(void);
    /**
     * @brief Function to get ssl_private_key from factory-info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_ssl_private_key(void);
    /**
     * @brief Function to get ssl_shared_key from factory-info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_ssl_shared_key(void);
    /**
     * @brief Function to get ezlopi_config from factory-info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_ezlopi_config(void);
    /**
     * @brief Function to get provision_token from factory-info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_provision_token(void);
    /**
     * @brief Function to get get_provisioning_server from factory-info
     *
     * @return char*
     */
    char *EZPI_core_factory_info_v3_get_provisioning_server(void);
    /**
     * @brief Funtion to get config_version from factory-info
     *
     * @return uint16_t
     */
    uint16_t EZPI_core_factory_info_v3_get_config_version(void);
    /**
     * @brief Funtion to set wifi credentials
     *
     * @param ssid ssid to set
     * @param password pass to set
     * @return int
     */
    int EZPI_core_factory_info_v3_set_wifi(char *ssid, char *password);
    /**
     * @brief Function to set basic factory-info
     *
     * @param ezlopi_config_basic Structure containing basic info
     * @return int
     */
    int EZPI_core_factory_info_v3_set_basic(s_basic_factory_info_t *ezlopi_config_basic);
    /**
     * @brief Function to set ezlopi-config in factory info
     *
     * @param cj_data Target cjson data
     * @return int
     */
    int EZPI_core_factory_info_v3_set_ezlopi_config(cJSON *cj_data);
    /**
     * @brief  Function to set 'ca_cert' info factory info
     *
     * @param cj_data
     * @return int
     */
    int EZPI_core_factory_info_v3_set_ca_cert(cJSON *cj_data);
    /**
     * @brief  Function to set 'shared_key' info factory info
     *
     * @param cj_data
     * @return int
     */
    int EZPI_core_factory_info_v3_set_ssl_shared_key(cJSON *cj_data);
    /**
     * @brief  Function to set 'public_key' info factory info
     *
     * @param cj_data
     * @return int
     */
    int EZPI_core_factory_info_v3_set_ssl_public_key(cJSON *cj_data);
    /**
     * @brief Function to set 'private_key' info factory info
     *
     * @param cj_data
     * @return int
     */
    int EZPI_core_factory_info_v3_set_ssl_private_key(cJSON *cj_data);
    /**
     * @brief Function to free factory info
     *
     * @param arg
     */
    void EZPI_core_factory_info_v3_free(void *arg);
    /**
     * @brief Function to reset of factory info
     *
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_factory_info_v3_factory_reset(void);
    /**
     * @brief Function to softreset the sysytem
     *
     * @return int
     */
    int EZPI_core_factory_info_v3_scenes_factory_soft_reset(void);

    // int EZPI_core_factory_info_prov_init(void);

#ifdef __cplusplus
}
#endif


#endif // _EZLOPI_CORE_FACTORY_INFO_H_


/*******************************************************************************
*                          End of File
*******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif




#ifdef __cplusplus
}
#endif

