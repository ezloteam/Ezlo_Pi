# set(srcs    "main.cpp"
# "example.cpp"
# )

file(GLOB_RECURSE SOURCES "./*.c*")

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES sensor_service ezlopi_timer )

# file( GLOB_RECURSE SOURCES "./*.cpp" )

# idf_component_register( SRCS ${SOURCES}
# INCLUDE_DIRS "")
# REQUIRES common app_update soc
# wifi_service2
# time_service)
