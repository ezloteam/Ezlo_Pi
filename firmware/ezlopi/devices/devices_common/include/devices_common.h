#ifndef __DEVICES_COMMON_H__
#define __DEVICES_COMMON_H__

#include <string.h>
#include "inttypes.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_DEV 10
#define DEVICE_TYPE_ID "ezlopi" // manufacturer deviceTypeId

    typedef enum e_device_type
    {
        LED = 0,
        SWITCH,
        PLUG,
        TAMPER,
        MPU6050 = 5,
    } e_device_type_t;

    typedef struct s_device_properties
    {
        char device_id[8];
        uint8_t dev_type;
        char name[16]; // device Name
        char roomId[8];
        char roomName[20];
        char item_id[8]; // item id
        char item_name[20];
        bool input_vol;
        bool out_vol;
        uint8_t input_gpio;
        uint8_t out_gpio;
        bool is_input;
        bool input_inv;
        bool out_inv;
        bool is_meter;
        char category[32];
        char subcategory[32];
        char devicType[32];
        char value_type[16];
        bool has_getter;
        bool has_setter;
    } s_device_properties_t;

    void devices_common_init_devices(void);
    s_device_properties_t *devices_common_device_list(void);
    int devices_common_get_device_by_item_id(const char *item_id);
    const char *devices_common_get_device_type_id(void);

#ifdef __cplusplus
}
#endif

#endif // __DEVICES_COMMON_H__