

idf_component_register(SRCS "ezlopi_cloud_setting_commands.c"
                        INCLUDE_DIRS .
                        REQUIRES    ezlopi-util-trace
                                    cjext

                                    ezlopi-cloud-constants

                                    ezlopi-core-setting-commands
)


