# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "./*.c")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES spi_flash esp_common frozen debug mbedtls aes_lib)