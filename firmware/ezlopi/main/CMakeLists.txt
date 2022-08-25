# set(srcs    "main.cpp"
# "example.cpp"
# )

file(GLOB_RECURSE SOURCES "./*.c*")

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES nvs_flash frozen wifi_interface web_provisioning websocket_client
    http factory_info certificates qt_serial interface_common debug
    switch_service MPU6050_service bt json aes_lib esp_rom wss esp_system)

# file( GLOB_RECURSE SOURCES "./*.cpp" )

# idf_component_register( SRCS ${SOURCES}
# INCLUDE_DIRS "")
# REQUIRES common app_update soc
# wifi_service2
# time_service)
