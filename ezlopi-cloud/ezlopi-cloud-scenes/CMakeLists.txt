file(GLOB_RECURSE src_files "*.c*")

# Define esp-components
set(ESP_COMPONENTS
       json
       heap
)

# Define ezlopi-components
set(EZLOPI_COMPONENTS
        ezlopi-util-trace
)
# Define core components
set(EZLOPI_CORE_COMPONENTS 
            ezlopi-core-factory-info 
            ezlopi-core-scenes
)
# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS 
            ezlopi-cloud-constants 
)


idf_component_register(SRCS "ezlopi_cloud_scenes_scripts.c" "ezlopi_cloud_scenes.c" "ezlopi_cloud_scenes_block_list.c" "ezlopi_cloud_scenes_block_data_list.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES 
            ${ESP_COMPONENTS}       
            ${EZLOPI_COMPONENTS}
            ${EZLOPI_CORE_COMPONENTS}
            ${EZLOPI_CLOUD_COMPONENTS}
            )

