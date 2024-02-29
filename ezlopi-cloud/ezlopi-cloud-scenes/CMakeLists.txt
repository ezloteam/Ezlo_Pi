file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_cloud_scenes_block_list.c" "ezlopi_cloud_scenes_block_data_list.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-core-factory-info ezlopi-util-trace json ezlopi-cloud-constants ezlopi-core-scenes)

