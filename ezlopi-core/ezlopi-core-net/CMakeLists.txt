# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace ezlopi-core-boards ezlopi-core-ethernet ezlopi-core-wifi ezlopi-core-ping ezlopi-core-websocket_client)