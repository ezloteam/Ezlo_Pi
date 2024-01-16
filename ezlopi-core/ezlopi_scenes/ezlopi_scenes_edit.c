#include "ezlopi_scenes_v2.h"
#include "ezlopi_cjson_macros.h"
#include "ezlopi_cloud_constants.h"

// static __edit_

int ezlopi_scenes_edit_by_id(uint32_t scene_id, cJSON *cj_updated_scene)
{
    CJSON_TRACE("cj_updated_scene", cj_updated_scene);

    int ret = 0;
    if (scene_id && cj_updated_scene)
    {
        l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_head_v2();
        while (scene_node)
        {
            if (scene_id == scene_node->_id)
            {
                
                break;
            }
            scene_node = scene_node->next;
        }
    }

    return ret;
}