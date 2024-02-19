

idf_component_register(SRCS "ezlopi_cloud_coordinates.c"
                        INCLUDE_DIRS .
                        REQUIRES    json
                                    ezlopi-util-trace
                                    ezlopi-core-nvs
                                    ezlopi-cloud-constants
                        )

