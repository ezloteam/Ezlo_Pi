file(GLOB_RECURSE src_files "*.c")

idf_component_register(SRCS "ezlopi_core_sntp.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash ezlopi-util-trace)
