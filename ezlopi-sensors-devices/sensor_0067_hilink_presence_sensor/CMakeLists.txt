

idf_component_register(SRCS "sensor_0067_hilink_presence_sensor.c" "hilink_presence_sensor_setting"
                        INCLUDE_DIRS .
                        REQUIRES trace ezlopi-core-actions  ezlopi-core-timer ezlopi-core-devices ezlopi-cloud-constants ezlopi-service-gpioisr ezlopi-core-cloud ld2410)

