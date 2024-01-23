file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash ezlopi-util-trace)
