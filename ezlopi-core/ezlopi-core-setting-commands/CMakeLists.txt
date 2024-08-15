

idf_component_register(SRCS "ezlopi_core_setting_commands.c"
                        INCLUDE_DIRS .
                        REQUIRES    ezlopi-util-trace 
                                    cjext
                                    ezlopi-core-nvs
                                    ezlopi-core-log

                                    ezlopi-cloud-constants
                                    ezlopi-core-ezlopi
)



