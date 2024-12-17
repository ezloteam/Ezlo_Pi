file(GLOB_RECURSE src_files "*.c*")

idf_component_register(SRCS "${src_files}"
                        INCLUDE_DIRS . 
                        REQUIRES    mdns 
                                    ezlopi-core-factory-info
                                    ezlopi-core-wifi
                                    ezlopi-util-trace
                                    ezlopi-core-processes
                                    ezlopi-core-errors
                                    )


