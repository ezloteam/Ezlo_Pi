file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0031_other_JSNSR04T.c"
                        INCLUDE_DIRS "."
                       PRIV_REQUIRES "driver"
                       REQUIRES trace ezlopi_actions  ezlopi_timer ezlopi_devices ezlopi_cloud_constants)