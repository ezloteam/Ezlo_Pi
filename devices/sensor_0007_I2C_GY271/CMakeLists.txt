idf_component_register(SRCS "sensor_0007_I2C_GY271.c"
    INCLUDE_DIRS "."
    REQUIRES trace 
             items 
             freertos 
             updated        
             ezlopi_actions 
             ezlopi_timer )