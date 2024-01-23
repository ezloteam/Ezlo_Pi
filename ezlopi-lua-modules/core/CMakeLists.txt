file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "core_external_gateway_api.c" "core_others_api.c" "core_updaters_api.c" "core_removers_api.c" "core_adders_api.c" "core_getters_api.c" "corelib.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES lua ezlopi-util-trace ezlopi-core-factory-info ezlopi-core-devices ezlopi-core-gateway ezlopi-util-version helper_functions )