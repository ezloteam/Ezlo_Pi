
    file(GLOB_RECURSE config_src "*.c")
    # set(config_src)

    idf_component_register(SRCS "bme680_bsec.c" "0010_I2C_BME680.c" "${config_src}"
        INCLUDE_DIRS "."
        REQUIRES trace ezlopi_actions ezlopi_hal ezlopi_timer ezlopi_devices ezlopi_cloud_constants gpio_isr_service ezlopi_cloud Bosch_BSEC2)
