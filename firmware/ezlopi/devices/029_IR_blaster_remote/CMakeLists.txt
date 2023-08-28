# file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "029_IR_blaster_remote.c" "IR_blaster_helpers.c"
    INCLUDE_DIRS .
    REQUIRES trace library ezlopi_actions  ezlopi_timer ezlopi_devices ezlopi_cloud_constants ir_blaster_tools 
    IR_Blaster_encoder_decoder IR_Blaster_data_operation)
