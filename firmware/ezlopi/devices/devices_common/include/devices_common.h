#ifndef __DEVICES_COMMON_H__
#define __DEVICES_COMMON_H__

#include <string>
#include <iostream>

#define MAX_DEV 10
#define DEVICE_TYPE_ID "ezlopi" // manufacturer deviceTypeId

typedef enum e_device_type
{
    LED = 0,
    SWITCH,
    PLUG,
    TAMPER,
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

class devices_common
{
private:
    void example_init(void);

public:
    void init_devices(void);
    s_device_properties_t *device_list(void);
    static devices_common *get_instance(void);
    int get_device_by_item_id(const char *item_id);
    const char *get_device_type_id(void);

    devices_common(devices_common &other) = delete;
    void operator=(const devices_common &) = delete;

protected:
    devices_common() {}
    static devices_common *devices_common_;
};

#endif // __DEVICES_COMMON_H__