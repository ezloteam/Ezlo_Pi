

idf_component_register(SRCS "ezlopi_cloud_status.c"
                        INCLUDE_DIRS .
                        REQUIRES    cjext 
                                    ezlopi-core-processes
                                    ezlopi-cloud-constants
                                    ezlopi-util-trace)

