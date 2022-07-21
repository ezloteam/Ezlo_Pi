file(GLOB_RECURSE src_files "src/*.cpp")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "include"
    REQUIRES frozen
    factory_info
    debug
    devices_common)