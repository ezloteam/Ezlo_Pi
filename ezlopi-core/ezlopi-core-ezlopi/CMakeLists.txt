file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_CORE_COMPONENTS
        ezlopi-util-trace
        ezlopi-core-ble
        ezlopi-core-wifi
        ezlopi-core-sntp
        ezlopi-core-mdns
        ezlopi-core-modes
        ezlopi-core-scenes
        ezlopi-core-buffer
        ezlopi-core-log
        ezlopi-core-errors
)

set(EZLOPI_CLOUD_COMPONENTS
        ezlopi-cloud-data
        ezlopi-cloud-devices
        ezlopi-cloud-scenes
        ezlopi-cloud-registration
        ezlopi-cloud-favorite
        ezlopi-cloud-gateways
        ezlopi-cloud-info
        ezlopi-cloud-modes
        ezlopi-cloud-items
        ezlopi-cloud-room
        ezlopi-cloud-settings
        ezlopi-cloud-network
        ezlopi-cloud-ota
        ezlopi-cloud-location
        ezlopi-cloud-coordinates
        ezlopi-cloud-offline-login
        ezlopi-cloud-status
        ezlopi-core-setting-commands
)

idf_component_register(SRCS  "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    freertos
                ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS}
                ezlopi-service-system-temperature-sensor
)

