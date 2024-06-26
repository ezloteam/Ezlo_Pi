#ifndef _EZLOPI_CORE_SCENES_CJSON_H_
#define _EZLOPI_CORE_SCENES_CJSON_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "cjext.h"

#include "ezlopi_core_scenes_v2.h"

cJSON* ezlopi_scenes_cjson_create_then_block(l_action_block_v2_t* then_block);
cJSON* ezlopi_scenes_cjson_create_when_block(l_when_block_v2_t* when_block);
void ezlopi_scenes_cjson_add_then_blocks(cJSON* root, l_action_block_v2_t* then_blocks);
void ezlopi_scenes_cjson_add_when_blocks(cJSON* root, l_when_block_v2_t* when_blocks);
cJSON* ezlopi_scenes_create_cjson_scene(l_scenes_list_v2_t* scene);
cJSON* ezlopi_scenes_create_cjson_scene_list(l_scenes_list_v2_t* scenes_list);
cJSON* ezlopi_scene_cjson_get_field(l_fields_v2_t* field_node);

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_CJSON_H_