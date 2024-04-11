file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "bme680_bsec.c" "sensor_0010_I2C_BME680.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace  
                Bosch_BSEC2
                BME68x_Sensor_library
                 
                ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions
                ezlopi-core-valueformatter

                ezlopi-hal-i2c
                
                ezlopi-cloud-items 
                ezlopi-cloud-constants    
)
