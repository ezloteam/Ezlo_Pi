file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0031_other_JSNSR04T.c" "jsn_sr04t.c"
                        INCLUDE_DIRS "."
                        PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver"
                       REQUIRES trace  ezlopi-core-actions  ezlopi-core-timer ezlopi-core-devices ezlopi-cloud-constants)