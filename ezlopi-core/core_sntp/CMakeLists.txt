file(GLOB_RECURSE config_src "./*.c*")

idf_component_register(SRCS "core_sntp.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash trace)
