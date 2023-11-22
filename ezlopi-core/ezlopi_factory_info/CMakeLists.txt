# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "./*.c")

idf_component_register(SRCS "ezlopi_device_config.c" "ezlopi_factory_info.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES spi_flash esp_common trace mbedtls ezlopi_nvs)
