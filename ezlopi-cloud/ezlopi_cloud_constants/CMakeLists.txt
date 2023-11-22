file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_item_name_str.c" "ezlopi_cloud_value_type_str.c" "ezlopi_cloud_thermostat_str.c" "ezlopi_cloud_subcategory_str.c" "ezlopi_cloud_status_str.c" "ezlopi_cloud_security_str.c" "ezlopi_cloud_scales_str.c" "ezlopi_cloud_room_types_str.c" "ezlopi_cloud_methods_str.c" "ezlopi_cloud_keywords.c" "ezlopi_cloud_item_name_str.c" "ezlopi_cloud_devices_str.c" "ezlopi_cloud_device_types_str.c" "ezlopi_cloud_constants.c" "ezlopi_cloud_category_str.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES )