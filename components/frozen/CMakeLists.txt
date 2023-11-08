set(frozen_src "frozen.c")

# file( GLOB_RECURSE frozen_src "*.c*" )

idf_component_register(SRCS "${frozen_src}"
                       INCLUDE_DIRS ".")