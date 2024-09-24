#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_util_trace.h"

#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_methods.h"

static const char *ezlopi_scenes_methods_name[] = {
#define EZLOPI_SCENE(method_type, name, func, category) name,
#include "ezlopi_core_scenes_method_types.h"
#undef EZLOPI_SCENE
};

static const char *ezlopi_scenes_method_category_name[] = {
#define EZLOPI_SCENE(method_type, name, func, category) category,
#include "ezlopi_core_scenes_method_types.h"
#undef EZLOPI_SCENE
};

const char *ezlopi_scene_get_scene_method_name(e_scene_method_type_t method_type)
{
    const char *ret = NULL;
    if ((method_type > EZLOPI_SCENE_METHOD_TYPE_NONE) && (method_type < EZLOPI_SCENE_METHOD_TYPE_MAX))
    {
        ret = ezlopi_scenes_methods_name[method_type];
    }
    return ret;
}

e_scene_method_type_t ezlopi_scenes_method_get_type_enum(char *method_name)
{
    e_scene_method_type_t method_type = EZLOPI_SCENE_METHOD_TYPE_NONE;
    if (method_name)
    {
        for (e_scene_method_type_t i = EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE; i < EZLOPI_SCENE_METHOD_TYPE_MAX; i++)
        {
            size_t max_len = (strlen(method_name) > strlen(ezlopi_scenes_methods_name[i])) ? strlen(method_name) : strlen(ezlopi_scenes_methods_name[i]);
            if (0 == strncmp(method_name, ezlopi_scenes_methods_name[i], max_len))
            {
                method_type = i;
                break;
            }
        }

#if 0
        /* When block */
        if (0 == strncmp(method_name, "isItemState", 12))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE;
        }
        else if (0 == strncmp(method_name, "isItemStateChanged", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE_CHANGED;
        }
        else if (0 == strncmp(method_name, "isButtonState", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_BUTTON_STATE;
        }
        else if (0 == strncmp(method_name, "isSunState", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_SUN_STATE;
        }
        else if (0 == strncmp(method_name, "isDate", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_DATE;
        }
        else if (0 == strncmp(method_name, "isOnce", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_ONCE;
        }
        else if (0 == strncmp(method_name, "isInterval", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_INTERVAL;
        }
        else if (0 == strncmp(method_name, "isDateRange", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_DATE_RANGE;
        }
        else if (0 == strncmp(method_name, "isUserLockOperation", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_USER_LOCK_OPERATION;
        }
        else if (0 == strncmp(method_name, "isHouseModeChangedTo", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_HOUSE_MODE_CHANGED_TO;
        }
        else if (0 == strncmp(method_name, "isHouseModeChangedFrom", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_HOUSE_MODE_CHANGED_FROM;
        }
        else if (0 == strncmp(method_name, "isDeviceState", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_DEVICE_STATE;
        }
        else if (0 == strncmp(method_name, "isNetworkState", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_NETWORK_STATE;
        }
        else if (0 == strncmp(method_name, "isSceneState", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_SCENE_STATE;
        }
        else if (0 == strncmp(method_name, "isGroupState", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_GROUP_STATE;
        }
        else if (0 == strncmp(method_name, "isCloudState", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_CLOUD_STATE;
        }
        else if (0 == strncmp(method_name, "isBatteryState", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_BATTERY_STATE;
        }
        else if (0 == strncmp(method_name, "isBatteryLevel", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_BATTERY_LEVEL;
        }
        else if (0 == strncmp(method_name, "compareNumbers", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_COMPARE_NUMBERS;
        }
        else if (0 == strncmp(method_name, "compareNumberRange", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_COMPARE_NUMBER_RANGE;
        }
        else if (0 == strncmp(method_name, "compareStrings", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_COMPARE_STRINGS;
        }
        else if (0 == strncmp(method_name, "stringOperation", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_STRING_OPERATION;
        }
        else if (0 == strncmp(method_name, "inArray", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IN_ARRAY;
        }
        else if (0 == strncmp(method_name, "compareValues", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_COMPARE_VALUES;
        }
        else if (0 == strncmp(method_name, "hasAtLeastOneDictionaryValue", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_HAS_ATLEAST_ONE_DICTIONARY_VALUE;
        }
        else if (0 == strncmp(method_name, "isFirmwareUpdateState", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_FIRMWARE_UPDATE_STATE;
        }
        else if (0 == strncmp(method_name, "isDictionaryChanged", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_DICTIONARY_CHANGED;
        }
        else if (0 == strncmp(method_name, "isDetectedInHotzone", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_IS_DETECTED_IN_HOTZONE;
        }
        else if (0 == strncmp(method_name, "and", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_AND;
        }
        else if (0 == strncmp(method_name, "not", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_NOT;
        }
        else if (0 == strncmp(method_name, "or", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_OR;
        }
        else if (0 == strncmp(method_name, "xor", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_XOR;
        }
        else if (0 == strncmp(method_name, "function", 15))
        {
            method_type = EZLOPI_SCENE_WHEN_METHOD_FUNCTION;
        }

        /* Then block methods */
        else if (0 == strncmp(method_name, "setItemValue", 15))
        {
            method_type = EZLOPI_SCENE_THEN_METHOD_SET_ITEM_VALUE;
        }
        else if (0 == strncmp(method_name, "setDeviceArmed", 15))
        {
            method_type = EZLOPI_SCENE_THEN_METHOD_SET_DEVICE_ARMED;
        }
        else if (0 == strncmp(method_name, "sendCloudAbstractCommand", 15))
        {
            method_type = EZLOPI_SCENE_THEN_METHOD_SEND_CLOUD_ABSTRACT_COMMAND;
        }
        else if (0 == strncmp(method_name, "switchHouseMode", 15))
        {
            method_type = EZLOPI_SCENE_THEN_METHOD_SWITCH_HOUSE_MODE;
        }
        else if (0 == strncmp(method_name, "sendHttpRequest", 15))
        {
            method_type = EZLOPI_SCENE_THEN_METHOD_SEND_HTTP_REQUEST;
        }
        else if (0 == strncmp(method_name, "runCustomScript", 15))
        {
            method_type = EZLOPI_SCENE_THEN_METHOD_RUN_CUSTOM_SCRIPT;
        }
        else if (0 == strncmp(method_name, "runPluginScript", 15))
        {
            method_type = EZLOPI_SCENE_THEN_METHOD_RUN_PLUGIN_SCRIPT;
        }
        else if (0 == strncmp(method_name, "runScene", 15))
        {
            method_type = EZLOPI_SCENE_THEN_METHOD_RUN_SCENE;
        }
        else if (0 == strncmp(method_name, "setSceneState", 15))
        {
            method_type = EZLOPI_SCENE_THEN_METHOD_SET_SCENE_STATE;
        }
        else if (0 == strncmp(method_name, "resetLatch", 15))
        {
            method_type = EZLOPI_SCENE_THEN_RESET_LATCH;
        }
        else if (0 == strncmp(method_name, "resetSceneLatches", 15))
        {
            method_type = EZLOPI_SCENE_THEN_RESET_SCENE_LATCHES;
        }
        else if (0 == strncmp(method_name, "rebootHub", 15))
        {
            method_type = EZLOPI_SCENE_THEN_REBOOT_HUB;
        }
        else if (0 == strncmp(method_name, "resetHub", 15))
        {
            method_type = EZLOPI_SCENE_THEN_RESET_HUB;
        }
        else if (0 == strncmp(method_name, "cloudAPI", 15))
        {
            method_type = EZLOPI_SCENE_THEN_CLOUD_API;
        }
        else if (0 == strncmp(method_name, "setExpression", 15))
        {
            method_type = EZLOPI_SCENE_THEN_SET_EXPRESSION;
        }
        else if (0 == strncmp(method_name, "setVariable", 15))
        {
            method_type = EZLOPI_SCENE_THEN_SET_VARIABLE;
        }
        else if (0 == strncmp(method_name, "toggleValue", 15))
        {
            method_type = EZLOPI_SCENE_THEN_TOGGLE_VALUE;
        }
        else
        {
            TRACE_E("Not Imeplemented!, method_type: %d", method_type);
        }
#endif
    }

    return method_type;
}

const char *ezlopi_scene_get_scene_method_category_name(char *method_name)
{
    const char *ret = NULL;
    for (e_scene_method_type_t i = EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE; i < EZLOPI_SCENE_METHOD_TYPE_MAX; i++)
    {
        size_t max_len = (strlen(method_name) > strlen(ezlopi_scenes_method_category_name[i])) ? strlen(method_name) : strlen(ezlopi_scenes_method_category_name[i]);
        if (0 == strncmp(method_name, ezlopi_scenes_method_category_name[i], max_len))
        {
            ret = ezlopi_scenes_method_category_name[i];
            break;
        }
    }
    return ret;
}

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS