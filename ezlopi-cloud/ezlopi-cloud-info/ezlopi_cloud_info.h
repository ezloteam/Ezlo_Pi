#ifndef __HUB_INFO_H__
#define __HUB_INFO_H__
#include <string.h>

#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void info_get(cJSON* cj_request, cJSON* cj_response);

    /**
     *@brief converts ms into time (i.e. hr:min:sec)
     *
     * @param time_buff estimated size should be 50 bytes
     * @param buff_len size of time_buff
     * @param ms time in mili-seconds
     */
    void ezlopi_tick_to_time(char* time_buff, uint32_t buff_len, uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // __HUB_INFO_H__