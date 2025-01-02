

idf_component_register(SRCS "ezlopi_cloud_coordinates.c"
                        INCLUDE_DIRS .
                        REQUIRES    ezlopi-util-trace
                                    ezlopi-core-nvs
                                    ezlopi-core-ezlopi
                                    ezlopi-cloud-constants
                        )

