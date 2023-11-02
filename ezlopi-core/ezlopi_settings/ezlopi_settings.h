#ifndef EZLOPI_SETTINGS_H
#define EZLOPI_SETTINGS_H

#include <stdbool.h>

#define EZLOPI_SETTINGS_MAGIC_NUMBER (uint32_t)0x457A5069

#define EZLOPI_SETTINGS_MAX_ENUM_VALUES 10

typedef enum
{
    EZLOPI_SETTINGS_ACTION_GET_SETTING,
    EZLOPI_SETTINGS_ACTION_SET_SETTING,
    EZLOPI_SETTINGS_ACTION_RESET_SETTING,
    EZLOPI_SETTINGS_ACTION_MAX
} e_ezlopi_settings_action_t;

typedef enum
{
    EZLOPI_SETTINGS_TYPE_TOKEN = 0,
    EZLOPI_SETTINGS_TYPE_BOOL,
    EZLOPI_SETTINGS_TYPE_INT,
    EZLOPI_SETTINGS_TYPE_MAX
} e_ezlopi_settings_value_type_t;

typedef struct ezlopi_settings_device_settings_type_action_value
{
    char *text;
    char *lang_tag;
} s_ezlopi_settings_device_settings_type_action_value_t;
typedef struct ezlopi_settings_device_settings_type_rgb_value
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} s_ezlopi_settings_device_settings_type_rgb_value_t;
typedef struct ezlopi_settings_device_settings_type_scalable_value
{
    float value;
    char *scale;
} s_ezlopi_settings_device_settings_type_scalable_value_t;

typedef uint8_t (*f_settings_call_t)(e_ezlopi_settings_action_t action, void *properties, void *arg, void *user_arg);

typedef struct s_ezlopi_device_settings_properties
{
    uint32_t id;
    uint32_t device_id;
    char *label;
    char *description;
    char *status;
    char *value_type;
    char *nvs_alias;
    bool value_nonvolatile;
    void *user_arg;

    union
    {
        char *string_value;
        char *token_value;
        int int_value;
        bool bool_value;
        s_ezlopi_settings_device_settings_type_action_value_t *action_value;
        s_ezlopi_settings_device_settings_type_rgb_value_t *rgb_value;
        s_ezlopi_settings_device_settings_type_scalable_value_t *scalable_value;
    } value;

    union
    {
        char *string_value;
        char *token_value;
        int int_value;
        bool bool_value;
        s_ezlopi_settings_device_settings_type_action_value_t *action_value;
        s_ezlopi_settings_device_settings_type_rgb_value_t *rgb_value;
        s_ezlopi_settings_device_settings_type_scalable_value_t *scalable_value;
    } value_defaut;

    f_settings_call_t __settings_call;

} s_ezlopi_device_settings_properties_t;

typedef struct l_ezlopi_device_settings
{
    void *user_arg;
    s_ezlopi_device_settings_properties_t *properties;
    struct l_ezlopi_device_settings *next;

} l_ezlopi_device_settings_t;

int ezlopi_device_setting_add(s_ezlopi_device_settings_properties_t *properties, void *user_arg);
void ezlopi_device_settings_print_settings(l_ezlopi_device_settings_t *head);
void ezlopi_initialize_settings(void);
void _ezlopi_device_settings_value_set(uint32_t id, void *args);
void _ezlopi_device_settings_reset_settings_id(uint32_t id);
void _ezlopi_device_settings_reset_device_id(uint32_t id);

l_ezlopi_device_settings_t *ezlopi_devices_settings_get_list(void);

#endif // EZLOPI_SETTINGS_H
