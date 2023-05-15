# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c*")

idf_component_register(SRCS "ezlopi_ble_service_provisioning.c" "ezlopi_ble_service_wifi.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES driver freertos trace ezlopi_devices ezlopi_ble ezlopi_factory_info ezlopi_nvs ezlopi_ping) 