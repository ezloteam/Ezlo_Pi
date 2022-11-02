# set(srcs    "main.cpp"
# "example.cpp"
# )

# file(GLOB_RECURSE SOURCES "./*.c*")
file(GLOB_RECURSE SOURCES "")

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES sensor_bme280 digital_io)

# file( GLOB_RECURSE SOURCES "./*.cpp" )

# idf_component_register( SRCS ${SOURCES}
# INCLUDE_DIRS "")
# REQUIRES common app_update soc
# wifi_service2
# time_service)
