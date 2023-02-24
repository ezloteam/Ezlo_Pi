file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace library ezlopi_actions ezlopi_timer ezlopi_devices IR_Blaster_encoder_decoder
            ezlopi_cloud_constants ir_blaster_tools IR_Blaster_data_operation)
