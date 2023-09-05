#include "trace.h"
#include "ezlopi_scenes_methods.h"
#include "ezlopi_scenes.h"

e_scene_method_type_t ezlopi_scenes_method_get_type_enum(char *method_name)
{
    e_scene_method_type_t methode_type = EZLOPI_SCENE_METHOD_TYPE_NONE;
    if (method_name)
    {
        /* When block */
        if (0 == strncmp(method_name, "isItemState", 12))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE;
        }
        else if (0 == strncmp(method_name, "isItemStateChanged", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE_CHANGED;
        }
        else if (0 == strncmp(method_name, "isButtonState", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_BUTTON_STATE;
        }
        else if (0 == strncmp(method_name, "isSunState", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_SUN_STATE;
        }
        else if (0 == strncmp(method_name, "isDate", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_DATE;
        }
        else if (0 == strncmp(method_name, "isOnce", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_ONCE;
        }
        else if (0 == strncmp(method_name, "isInterval", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_INTERVAL;
        }
        else if (0 == strncmp(method_name, "isDateRange", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_DATE_RANGE;
        }
        else if (0 == strncmp(method_name, "isUserLockOperation", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_USER_LOCK_OPERATION;
        }
        else if (0 == strncmp(method_name, "isHouseModeChangedTo", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_HOUSE_MODE_CHANGED_TO;
        }
        else if (0 == strncmp(method_name, "isHouseModeChangedFrom", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_HOUSE_MODE_CHANGED_FROM;
        }
        else if (0 == strncmp(method_name, "isDeviceState", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_DEVICE_STATE;
        }
        else if (0 == strncmp(method_name, "isNetworkState", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_NETWORK_STATE;
        }
        else if (0 == strncmp(method_name, "isSceneState", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_SCENE_STATE;
        }
        else if (0 == strncmp(method_name, "isGroupState", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_GROUP_STATE;
        }
        else if (0 == strncmp(method_name, "isCloudState", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_CLOUD_STATE;
        }
        else if (0 == strncmp(method_name, "isBatteryState", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_BATTERY_STATE;
        }
        else if (0 == strncmp(method_name, "isBatteryLevel", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_BATTERY_LEVEL;
        }
        else if (0 == strncmp(method_name, "compareNumbers", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_COMPARE_NUMBERS;
        }
        else if (0 == strncmp(method_name, "compareNumberRange", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_COMPARE_NUMBER_RANGE;
        }
        else if (0 == strncmp(method_name, "compareStrings", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_COMPARE_STRINGS;
        }
        else if (0 == strncmp(method_name, "stringOperation", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_STRING_OPERATION;
        }
        else if (0 == strncmp(method_name, "inArray", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IN_ARRAY;
        }
        else if (0 == strncmp(method_name, "compareValues", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_COMPARE_VALUES;
        }
        else if (0 == strncmp(method_name, "hasAtLeastOneDictionaryValue", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_HAS_ATLEAST_ONE_DICTIONARY_VALUE;
        }
        else if (0 == strncmp(method_name, "isFirmwareUpdateState", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_FIRMWARE_UPDATE_STATE;
        }
        else if (0 == strncmp(method_name, "isDictionaryChanged", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_DICTIONARY_CHANGED;
        }
        else if (0 == strncmp(method_name, "isDetectedInHotzone", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_IS_DETECTED_IN_HOTZONE;
        }
        else if (0 == strncmp(method_name, "and", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_AND;
        }
        else if (0 == strncmp(method_name, "not", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_NOT;
        }
        else if (0 == strncmp(method_name, "or", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_OR;
        }
        else if (0 == strncmp(method_name, "xor", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_XOR;
        }
        else if (0 == strncmp(method_name, "function", 15))
        {
            methode_type = EZLOPI_SCENE_WHEN_METHOD_FUNCTION;
        }

        /* Then block methods */
        else if (0 == strncmp(method_name, "setItemValue", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_METHOD_SET_ITEM_VALUE;
        }
        else if (0 == strncmp(method_name, "setDeviceArmed", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_METHOD_SET_DEVICE_ARMED;
        }
        else if (0 == strncmp(method_name, "sendCloudAbstractCommand", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_METHOD_SEND_CLOUD_ABSTRACT_COMMAND;
        }
        else if (0 == strncmp(method_name, "switchHouseMode", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_METHOD_SWITCH_HOUSE_MODE;
        }
        else if (0 == strncmp(method_name, "sendHttpRequest", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_METHOD_SEND_HTTP_REQUEST;
        }
        else if (0 == strncmp(method_name, "runCustomScript", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_METHOD_RUN_CUSTOM_SCRIPT;
        }
        else if (0 == strncmp(method_name, "runPluginScript", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_METHOD_RUN_PLUGIN_SCRIPT;
        }
        else if (0 == strncmp(method_name, "runScene", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_METHOD_RUN_SCENE;
        }
        else if (0 == strncmp(method_name, "setSceneState", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_METHOD_SET_SCENE_STATE;
        }
        else if (0 == strncmp(method_name, "resetLatch", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_RESET_LATCH;
        }
        else if (0 == strncmp(method_name, "resetSceneLatches", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_RESET_SCENE_LATCHES;
        }
        else if (0 == strncmp(method_name, "rebootHub", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_REBOOT_HUB;
        }
        else if (0 == strncmp(method_name, "resetHub", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_RESET_HUB;
        }
        else if (0 == strncmp(method_name, "cloudAPI", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_CLOUD_API;
        }
        else if (0 == strncmp(method_name, "setExpression", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_SET_EXPRESSION;
        }
        else if (0 == strncmp(method_name, "setVariable", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_SET_VARIABLE;
        }
        else if (0 == strncmp(method_name, "toggleValue", 15))
        {
            methode_type = EZLOPI_SCENE_THEN_TOGGLE_VALUE;
        }
        else
        {
            TRACE_E("Not Imeplemented!, methode_type: %d", methode_type);
        }
    }

    return methode_type;
}