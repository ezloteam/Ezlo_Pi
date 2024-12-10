file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "dht22.c" "sensor_0016_oneWire_DHT22.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ezlopi-util-trace
                                        # ezlopi-core-timer
                                        ezlopi-core-cloud
                                        ezlopi-core-actions
                                        ezlopi-core-devices
                                        ezlopi-core-valueformatter
                                        ezlopi-core-setting-commands
                                        ezlopi-core-errors

                                        ezlopi-cloud-items
                                        ezlopi-cloud-constants

                                        ezlopi-service-gpioisr
                                        )
