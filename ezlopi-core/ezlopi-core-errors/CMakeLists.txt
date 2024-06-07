# Include all source files
file(GLOB_RECURSE config_src "*.c")


# Register the components
idf_component_register(SRCS "${config_src}"
                        INCLUDE_DIRS "."
)
