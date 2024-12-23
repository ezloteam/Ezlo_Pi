file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_COMPONENTS
        ezlopi-util-trace
)
set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-actions
        ezlopi-core-devices
        # ezlopi-core-timer
        ezlopi-core-valueformatter 
        ezlopi-core-errors
)
set(EZLOPI_CLOUD_COMPONENTS
    ezlopi-cloud-constants
)
set(EZLOPI_HAL_COMPONENTS
    ezlopi-hal-uart
)
set(EZLOPI_SERVICE_COMPONENTS
    ezlopi-service-gpioisr
)

idf_component_register(SRCS "pms5003.c" "sensor_0037_pms5003_sensor.c" "pms_dev_items_prepare.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS}
                ${EZLOPI_HAL_COMPONENTS}
                ${EZLOPI_SERVICE_COMPONENTS}
    )