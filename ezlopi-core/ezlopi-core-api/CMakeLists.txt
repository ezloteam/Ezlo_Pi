file( GLOB_RECURSE src_files "*.c*" )

set(ESP_IDF_COMPONENTS
)
set(EZLOPI_COMPONENTS
        ezlopi-util-trace
)

set(EZLOPI_CLOUD_COMPONENTS
        ezlopi-cloud-ota
        ezlopi-cloud-data
        ezlopi-cloud-info
        ezlopi-cloud-room
        ezlopi-cloud-items
        ezlopi-cloud-modes
        ezlopi-cloud-scenes
        ezlopi-cloud-devices
        ezlopi-cloud-network
        ezlopi-cloud-location
        ezlopi-cloud-favorite
        ezlopi-cloud-gateways
        ezlopi-cloud-settings
        ezlopi-cloud-constants
        ezlopi-cloud-coordinates
        ezlopi-cloud-offline-login
        ezlopi-cloud-registration
        ezlopi-cloud-log
        ezlopi-cloud-hub-data-list
        ezlopi-cloud-setting-commands
)

set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-reset
        ezlopi-core-ezlopi
)


idf_component_register(SRCS  "${src_files}"
                       INCLUDE_DIRS "."
                       REQUIRES         ${EZLOPI_COMPONENTS}
                                        ${EZLOPI_COMPONENTS}
                                        ${ESP_IDF_COMPONENTS}
                                        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                        )

