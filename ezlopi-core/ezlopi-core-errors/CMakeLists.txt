# Include all source files
file(GLOB_RECURSE config_src "*.c")


# Register the components
idf_component_register(SRCS "ezlopi_core_errors.c" "${config_src}"
                        INCLUDE_DIRS "."
)
