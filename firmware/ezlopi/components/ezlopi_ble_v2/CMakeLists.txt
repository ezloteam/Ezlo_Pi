# file( GLOB_RECURSE src_files "*.c*" )

# idf_component_register(SRCS "ezlopi_ble_gap.c" "ezlopi_ble_v2.c" "ezlopi_ble_gatt.c" "${src_files}"
#                        INCLUDE_DIRS "."
#                        REQUIRES bt trace json ezlopi_nvs ezlopi_wifi)