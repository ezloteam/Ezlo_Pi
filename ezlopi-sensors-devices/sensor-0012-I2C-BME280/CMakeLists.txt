# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")
# set(config_src "sensor_bme280.c")

idf_component_register(SRCS "sensor_0012_I2C_BME280.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace
                # ezlopi-core-timer
                ezlopi-core-cloud
                ezlopi-core-actions
                ezlopi-core-devices
                ezlopi-core-valueformatter
                ezlopi-core-setting-commands
                ezlopi-core-errors

                ezlopi-hal-i2c

                ezlopi-cloud-items
                ezlopi-cloud-constants
                )
