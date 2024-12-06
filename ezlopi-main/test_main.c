#include <stdio.h>
#include <stdlib.h>
#include "cjext.h"

// Helper function to add a string key-value attribute
cJSON *addStringAttribute(const char *key, const char *value)
{
    cJSON *attribute = cJSON_CreateObject(__FUNCTION__);
    printf("attribute: %p\r\n", attribute);

    if (attribute)
    {
        cJSON_AddStringToObject(__FUNCTION__, attribute, "key", key);
        cJSON *valueObj = cJSON_AddObjectToObject(__FUNCTION__, attribute, "value");
        printf("valueObj: %p\r\n", valueObj);

        if (valueObj)
        {
            cJSON_AddStringToObject(__FUNCTION__, valueObj, "stringValue", value);
        }
    }
    return attribute;
}

// Helper function to add a boolean, int, or double attribute
cJSON *addPrimitiveAttribute(const char *key, const char *type, double value)
{
    cJSON *attribute = cJSON_CreateObject(__FUNCTION__);
    printf("attribute: %p\r\n", attribute);

    if (attribute)
    {
        cJSON_AddStringToObject(__FUNCTION__, attribute, "key", key);
        cJSON *valueObj = cJSON_AddObjectToObject(__FUNCTION__, attribute, "value");
        printf("valueObj: %p\r\n", valueObj);
        if (valueObj)
        {
            if (strcmp(type, "boolValue") == 0)
            {
                cJSON_AddBoolToObject(__FUNCTION__, valueObj, type, value != 0);
            }
            else if (strcmp(type, "intValue") == 0)
            {
                cJSON_AddNumberToObject(__FUNCTION__, valueObj, type, (int)value);
            }
            else if (strcmp(type, "doubleValue") == 0)
            {
                cJSON_AddNumberToObject(__FUNCTION__, valueObj, type, value);
            }
        }
    }
    return attribute;
}

// Helper function to add an array attribute
cJSON *addArrayAttribute(const char *key, const char **values, size_t count)
{
    cJSON *attribute = cJSON_CreateObject(__FUNCTION__);
    printf("attribute: %p\r\n", attribute);
    printf("here 1\r\n");

#if 0
    if (attribute)
    {
        printf("key: %s\r\n", key ? key : "null");
        cJSON_AddStringToObject(__FUNCTION__, attribute, "key", "key");

        printf("here 3\r\n");

        cJSON *valueObj = cJSON_AddObjectToObject(__FUNCTION__, attribute, "value");
        printf("here 4\r\n");
        printf("valueObj: %p\r\n", valueObj);
        if (valueObj)
        {
            cJSON *arrayValue = cJSON_AddArrayToObject(__FUNCTION__, valueObj, "arrayValue");
            printf("arrayValue: %p\r\n", arrayValue);

            for (size_t i = 0; i < count; i++)
            {
                cJSON *item = cJSON_CreateObject(__FUNCTION__);
                printf("item[%u]: %p\r\n", i, item);
                cJSON_AddStringToObject(__FUNCTION__, item, "stringValue", values[i]);
                cJSON_AddItemToArray(arrayValue, item);
            }
        }
    }
#endif
    return attribute;
}

// Helper function to add a map attribute
cJSON *addMapAttribute(const char *key, const char *mapKey, const char *mapValue)
{
    cJSON *attribute = cJSON_CreateObject(__FUNCTION__);
    printf("attribute: %p\r\n", attribute);
    if (attribute)
    {
        cJSON_AddStringToObject(__FUNCTION__, attribute, "key", key);
        cJSON *valueObj = cJSON_AddObjectToObject(__FUNCTION__, attribute, "value");
        printf("valueObj: %p\r\n", valueObj);
        if (valueObj)
        {
            cJSON *kvlistValue = cJSON_AddObjectToObject(__FUNCTION__, valueObj, "kvlistValue");
            printf("kvlistValue: %p\r\n", kvlistValue);

            cJSON *values = cJSON_AddArrayToObject(__FUNCTION__, kvlistValue, "values");
            printf("values: %p\r\n", values);

            cJSON *mapItem = cJSON_CreateObject(__FUNCTION__);
            printf("mapItem: %p\r\n", mapItem);

            cJSON_AddStringToObject(__FUNCTION__, mapItem, "key", mapKey);
            cJSON *mapValueObj = cJSON_AddObjectToObject(__FUNCTION__, mapItem, "value");
            printf("mapValueObj: %p\r\n", mapValueObj);

            cJSON_AddStringToObject(__FUNCTION__, mapValueObj, "stringValue", mapValue);
            cJSON_AddItemToArray(values, mapItem);
        }
    }
    return attribute;
}

// Main function to create the JSON
cJSON *createResourceLogs()
{
    cJSON *logs = cJSON_CreateObject(__FUNCTION__);
    cJSON *resourceLogs = cJSON_AddArrayToObject(__FUNCTION__, logs, "resourceLogs");
    printf("resourceLogs: %p\r\n", resourceLogs);

    cJSON *resourceLog = cJSON_CreateObject(__FUNCTION__);
    printf("resourceLog: %p\r\n", resourceLog);

    cJSON_AddItemToArray(resourceLogs, resourceLog);

    // Add resource
    cJSON *resource = cJSON_AddObjectToObject(__FUNCTION__, resourceLog, "resource");
    printf("resource: %p\r\n", resource);

    cJSON *attributes = cJSON_AddArrayToObject(__FUNCTION__, resource, "attributes");
    printf("attributes: %p\r\n", attributes);

    cJSON_AddItemToArray(attributes, addStringAttribute("service.name", "my.service"));

    // Add scopeLogs
    cJSON *scopeLogs = cJSON_AddArrayToObject(__FUNCTION__, resourceLog, "scopeLogs");
    printf("scopeLogs: %p\r\n", scopeLogs);

    cJSON *scopeLog = cJSON_CreateObject(__FUNCTION__);
    printf("scopeLog: %p\r\n", scopeLog);

    cJSON_AddItemToArray(scopeLogs, scopeLog);

    // Add scope
    cJSON *scope = cJSON_AddObjectToObject(__FUNCTION__, scopeLog, "scope");
    printf("scope: %p\r\n", scope);

    cJSON_AddStringToObject(__FUNCTION__, scope, "name", "my.library");
    cJSON_AddStringToObject(__FUNCTION__, scope, "version", "1.0.0");
    cJSON *scopeAttributes = cJSON_AddArrayToObject(__FUNCTION__, scope, "attributes");
    printf("scopeAttributes: %p\r\n", scopeAttributes);

    cJSON_AddItemToArray(scopeAttributes, addStringAttribute("my.scope.attribute", "some scope attribute"));

    // Add logRecords
    cJSON *logRecords = cJSON_AddArrayToObject(__FUNCTION__, scopeLog, "logRecords");
    printf("logRecords: %p\r\n", logRecords);

    cJSON *logRecord = cJSON_CreateObject(__FUNCTION__);
    printf("logRecord: %p\r\n", logRecord);

    cJSON_AddItemToArray(logRecords, logRecord);

    // Add logRecord fields
    cJSON_AddStringToObject(__FUNCTION__, logRecord, "timeUnixNano", "1544712660300000000");
    cJSON_AddStringToObject(__FUNCTION__, logRecord, "observedTimeUnixNano", "1544712660300000000");
    cJSON_AddNumberToObject(__FUNCTION__, logRecord, "severityNumber", 10);
    cJSON_AddStringToObject(__FUNCTION__, logRecord, "severityText", "Information");
    cJSON_AddStringToObject(__FUNCTION__, logRecord, "traceId", "5B8EFFF798038103D269B633813FC60C");
    cJSON_AddStringToObject(__FUNCTION__, logRecord, "spanId", "EEE19B7EC3C1B174");

    // Add log body
    cJSON *body = cJSON_AddObjectToObject(__FUNCTION__, logRecord, "body");
    printf("body: %p\r\n", body);

    cJSON_AddStringToObject(__FUNCTION__, body, "stringValue", "Example log record");

    // Add log attributes
    cJSON *logAttributes = cJSON_AddArrayToObject(__FUNCTION__, logRecord, "attributes");
    printf("logAttributes: %p\r\n", logAttributes);

    cJSON_AddItemToArray(logAttributes, addStringAttribute("string.attribute", "some string"));
    cJSON_AddItemToArray(logAttributes, addPrimitiveAttribute("boolean.attribute", "boolValue", 1));
    cJSON_AddItemToArray(logAttributes, addPrimitiveAttribute("int.attribute", "intValue", 10));
    cJSON_AddItemToArray(logAttributes, addPrimitiveAttribute("double.attribute", "doubleValue", 637.704));

    const char *arrayValues[] = {"many", "values"};
    cJSON_AddItemToArray(logAttributes, addArrayAttribute("array.attribute", arrayValues, 2));
    cJSON_AddItemToArray(logAttributes, addMapAttribute("map.attribute", "some.map.key", "some value"));

    return logs;
}

// Example usage
void app_main(void)
{
    uint32_t free_heap = esp_get_free_heap_size();
    uint32_t watermark_heap = esp_get_minimum_free_heap_size();
    uint32_t free_heap_internal = esp_get_free_internal_heap_size();
    uint32_t total_heap_size = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);

    printf("----------------------------------------------\r\n");
    printf("Total heap:                %d B   %.4f KB\r\n", total_heap_size, total_heap_size / 1024.0);
    printf("Free Heap Size:            %d B    %.4f KB\r\n", free_heap, free_heap / 1024.0);
    printf("Heap Watermark:            %d B    %.4f KB\r\n", watermark_heap, watermark_heap / 1024.0);
    printf("----------------------------------------------\r\n");

    cJSON *json = createResourceLogs();
    char *jsonString = cJSON_Print(__FUNCTION__, json);
    printf("%s\n", jsonString ? jsonString : "null");

    // Clean up
    free(jsonString);
    cJSON_Delete(__FUNCTION__, json);
    return 0;
}