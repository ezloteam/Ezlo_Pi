

idf_component_register(SRCS "ezlopi_cloud_hub_status_get.c"
                        INCLUDE_DIRS .
                        REQUIRES    json 
                                    ezlopi-core-processes
                                    ezlopi-cloud-constants
                                    ezlopi-util-trace)

