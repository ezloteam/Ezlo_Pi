#include <string.h>

#include "modes.h"
#include "trace.h"
#include "frozen.h"
#include "cJSON.h"
#include "ezlopi_cloud_constants.h"

const char *modes_1_start = "{\"method\":\"hub.modes.get\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"current\":\"%s\",\"switchTo\":\"\",\"timeIsLeftToSwitch\":0,\"switchToDelay\":0,\"alarmDelay\":0,\"modes\":[";
const char *modes_1_modes_start = "{\"_id\":\"%s\",\"name\":\"%s\",\"description\":\"%s\"";
const char *modes_1_notifications = "\"notifications\":[\"%s\"";
const char *modes_1_disarmedDefault = "\"disarmedDefault\":[\"%s\"";
const char *modes_1_disarmedDevices = "\"disarmedDevices\":[\"%s\"";
const char *modes_1_alarmsOffDevices = "\"alarmsOffDevices\":[\"%s\"";
const char *modes_1_camerasOffDevices = "\"camerasOffDevices\":[\"%s\"";
const char *modes_1_protect = "\"protect\":[\"%s\"";
const char *modes_1_item_end = "]";
const char *modes_1_end = "]},\"sender\":%.*s}";

void modes_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
    if (cj_result)
    {
        cJSON_AddStringToObject(cj_result, "current", "");
        cJSON_AddStringToObject(cj_result, "switchTo", "");
        cJSON_AddStringToObject(cj_result, "timeIsLeftToSwitch", "");
        cJSON_AddStringToObject(cj_result, "switchToDelay", "");
        cJSON_AddStringToObject(cj_result, "alarmDelay", "");
        cJSON_AddArrayToObject(cj_result, "modes");
    }
}
