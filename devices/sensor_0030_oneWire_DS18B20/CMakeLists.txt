file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "ds18b20_onewire.c" "sensor_0030_oneWire_DS18B20.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace ezlopi_actions  ezlopi_timer ezlopi_devices ezlopi_cloud_constants)