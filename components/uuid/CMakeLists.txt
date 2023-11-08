file(GLOB_RECURSE config_src "src/*.c*")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "include")