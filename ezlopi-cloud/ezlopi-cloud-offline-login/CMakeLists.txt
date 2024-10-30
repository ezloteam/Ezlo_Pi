file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_cloud_offline_login.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-cloud-data
             ezlopi-util-trace
             cjext
             ezlopi-core-offline-login )
