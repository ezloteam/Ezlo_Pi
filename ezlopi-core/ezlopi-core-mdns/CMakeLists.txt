

idf_component_register(SRCS "ezlopi_core_mdns.c"
                        INCLUDE_DIRS . 
                        REQUIRES    mdns 
                                    ezlopi-core-factory-info
                                    ezlopi-core-wifi
                                    ezlopi-util-trace
                                    ezlopi-core-processes
                                    ezlopi-core-errors
                                    )


