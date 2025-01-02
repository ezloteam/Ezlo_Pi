#ifndef __EZLOPI_CLOUD_CONSTANTS_H__
#define __EZLOPI_CLOUD_CONSTANTS_H__

//------------------- MACRO FOR COMPARING EQUAL STR ------------------------------------------------------------
#define BIGGER_LEN(str1_len, str2_len) ((str1_len > str2_len) ? str1_len : str2_len)
#define COMPARE_IF_EQUAL(STR1, STR2 , STR1_LEN , STR2_LEN) (strncmp(STR1, STR2, BIGGER_LEN(STR1_LEN, STR2_LEN)) == 0) 
#define EZPI_STRNCMP_IF_EQUAL(STR1, STR2 , STR1_LEN , STR2_LEN) \
    ((NULL == STR1) ? false       \
     : (NULL == STR2) ? false       \
     : ((0 == STR1_LEN) && (0 == STR2_LEN)) ? false       \
                      : COMPARE_IF_EQUAL(STR1, STR2 , STR1_LEN , STR2_LEN))
//-------------------------------------------------------------------------------------------------------------

#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_scales_str.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_devices_str.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_item_name_str.h"
#include "ezlopi_cloud_room_types_str.h"
#include "ezlopi_cloud_thermostat_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"

#endif // __EZLOPI_CLOUD_CONSTANTS_H__
