file(GLOB_RECURSE config_src "*.c*")

idf_component_register(SRCS "ezlopi_core_wsc.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES mbedtls ezlopi-util-trace ezlopi-core-factory-info cjext)