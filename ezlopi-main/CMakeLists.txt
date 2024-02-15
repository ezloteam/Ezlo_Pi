# Include all source files
file(GLOB_RECURSE SOURCES "*.c")

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES 
            ezlopi-user-config
            ezlopi-util-trace
            ezlopi-core-ezlopi
    )


