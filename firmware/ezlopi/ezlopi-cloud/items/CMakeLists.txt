file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "items.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES frozen ezlopi_factory_info trace items json ezlopi_cloud_constants ezlopi_devices web_provisioning)