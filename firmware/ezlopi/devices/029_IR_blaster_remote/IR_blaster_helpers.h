

#ifndef __IR_BLASTER_HELPERS_H__
#define __IR_BLASTER_HELPERS_H__

#include "029_IR_blaster_remote.h"
#include "esp_err.h"
#include "cJSON.h"


esp_err_t ir_remote_blaster_learned_code(char *Hex_string_data);
esp_err_t ir_remote_learner();
esp_err_t build(void *result, const ir_builder_config_t *config, uint32_t *buffer1, uint32_t timing_array_len);
int capture(); // 
void store(rmt_item32_t *items, uint32_t len);
esp_err_t blaster_mode_get_value_cjson(cJSON *params);
esp_err_t learner_mode_get_value_cjson(cJSON *params);

#endif // __IR_BLASTER_HELPERS_H__

