file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "bme680_bsec.c" "sensor_0010_I2C_BME680.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    trace  
                Bosch_BSEC2
                BME68x_Sensor_library
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_actions 
                ezlopi_devices 
                ezlopi_cloud_constants 
                ezlopi_valueformatter)
