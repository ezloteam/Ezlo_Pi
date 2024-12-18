file(GLOB_RECURSE config_src "*.c")


idf_component_register(SRCS "${config_src}"
                        INCLUDE_DIRS .
                        REQUIRES    ezlopi-util-trace 
                                    cjext
                                    ezlopi-core-nvs
                                    ezlopi-core-log

                                    ezlopi-cloud-constants
                                    ezlopi-core-ezlopi
)



