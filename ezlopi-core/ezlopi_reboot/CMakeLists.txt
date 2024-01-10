file(GLOB_RECURSE config_src "./*.c*")

idf_component_register(SRCS "ezlopi_reboot.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES  trace web_provisioning)
