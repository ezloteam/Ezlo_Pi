# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace ezlopi_devices ezlopi_factory_info ezlopi_nvs ezlopi_wifi ezlopi_event_queue mac_uuid ezlopi_ping ezlopi_event_group)
