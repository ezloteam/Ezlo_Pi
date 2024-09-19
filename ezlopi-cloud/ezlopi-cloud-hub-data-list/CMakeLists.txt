
idf_component_register(SRCS "ezlopi_cloud_hub_data_list.c"
                        INCLUDE_DIRS .
                        REQUIRES    ezlopi-util-trace 
                                    cjext
                                    ezlopi-cloud-constants
                                    ezlopi-core-setting-commands
)



