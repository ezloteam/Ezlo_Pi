
    idf_component_register(SRCS "bsec2.c" 
                        INCLUDE_DIRS "bsec_interface" "."
                        REQUIRES BME68x_Sensor_library)

    target_link_libraries(${COMPONENT_TARGET} "-L${CMAKE_CURRENT_LIST_DIR}/precompiled_binaries/esp32s3")
    target_link_libraries(${COMPONENT_TARGET} algobsec)
