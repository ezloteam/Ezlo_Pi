
    idf_component_register(SRCS "bsec2.c"
                        INCLUDE_DIRS "bsec_interface" "."
                        REQUIRES BME68x_Sensor_library)

    if(CONFIG_IDF_TARGET_ESP32S3)
        target_link_libraries(${COMPONENT_TARGET} "-L${CMAKE_CURRENT_LIST_DIR}/precompiled_binaries/esp32s3")
        target_link_libraries(${COMPONENT_TARGET} algobsec)
    endif()

    if(CONFIG_IDF_TARGET_ESP32c3)
        target_link_libraries(${COMPONENT_TARGET} "-L${CMAKE_CURRENT_LIST_DIR}/precompiled_binaries/esp32c3")
        target_link_libraries(${COMPONENT_TARGET} algobsec)
    endif()

    if(CONFIG_IDF_TARGET_ESP32)
        target_link_libraries(${COMPONENT_TARGET} "-L${CMAKE_CURRENT_LIST_DIR}/precompiled_binaries/esp32")
        target_link_libraries(${COMPONENT_TARGET} algobsec)
    endif()

    if(CONFIG_IDF_TARGET_ESP32S2)
        target_link_libraries(${COMPONENT_TARGET} "-L${CMAKE_CURRENT_LIST_DIR}/precompiled_binaries/esp32s2")
        target_link_libraries(${COMPONENT_TARGET} algobsec)
    endif()
