#ifndef __EZLOPI_SCENES_EDIT_H__
#define __EZLOPI_SCENES_EDIT_H__

#include "string.h"
#include "stdint.h"
#include "cJSON.h"

int ezlopi_scene_edit_store_updated_to_nvs(cJSON *cj_updated_scene);
int ezlopi_scenes_edit_update_id(uint32_t scene_id, cJSON *cj_updated_scene);

#endif // __EZLOPI_SCENES_EDIT_H__
