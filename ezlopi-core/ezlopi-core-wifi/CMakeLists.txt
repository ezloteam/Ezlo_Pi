# set(config_src "config.cpp")

file(GLOB_RECURSE wifi_src "./*.c*")

set(EZLOPI_COMPONENTS 
        dns_hijacking
)
set(EZLOPI_SERVICE_COMPONENTS 
        ezlopi-service-uart
)

set (EZLOPI_CORE_COMPONENTS
        ezlopi-core-processes
        ezlopi-core-errors
        ezlopi-core-ping
)


idf_component_register(SRCS "ezlopi_core_wifi_err_reason.c" "ezlopi_core_wifi.c" "ezlopi_core_wifi_ap.c" "${wifi_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_SERVICE_COMPONENTS}
        )
