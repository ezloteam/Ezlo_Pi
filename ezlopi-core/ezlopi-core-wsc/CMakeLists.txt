file(GLOB_RECURSE config_src "*.c*")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES mbedtls ezlopi-util-trace ezlopi-core-factory-info cjext)