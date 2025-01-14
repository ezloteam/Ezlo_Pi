# # Include all source files
file(GLOB_RECURSE srcs "*.c")

# Define core components
set(EZLOPI_CORE_COMPONENTS
        # ezlopi-core-cloud
        # ezlopi-core-actions
        # ezlopi-core-devices
        ezlopi-core-processes
        ezlopi-core-http
        ezlopi-core-nvs
        ezlopi-core-modes
        ezlopi-core-errors
        ezlopi-core-sntp
)

# # Define HAL components
# set(EZLOPI_HAL_COMPONENTS
#         ezlopi-hal-gpio
#         ezlopi-hal-adc
#         ezlopi-hal-pwm
#         ezlopi-hal-onewire
#         ezlopi-hal-uart
#         ezlopi-hal-i2c
#         ezlopi-hal-spi
# )

# Define HAL components
set(EZLOPI_LUA_COMPONENTS
    ezlopi-lua-modules
)

# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS
        # ezlopi-cloud-settings
        ezlopi-cloud-constants
)


idf_component_register(SRCS "ezlopi_core_scenes_then_methods.c" "ezlopi_core_scenes_then_methods_helper_func.c" "ezlopi_core_scenes_when_methods_helper_functions.c" "ezlopi_core_scenes_operators.c" "ezlopi_core_scenes_edit.c" "ezlopi_core_scenes_value.c" "ezlopi_core_scenes_expressions.c" "ezlopi_core_scenes_v2.c" "ezlopi_core_scenes_methods.c" "ezlopi_core_scenes_notifications.c" "ezlopi_core_scenes_cjson.c" "ezlopi_core_scenes_when_methods.c" "ezlopi_core_scenes_scripts.c" "ezlopi_core_scenes_populate.c" "ezlopi_core_scenes_delete.c" "${srcs}"
    INCLUDE_DIRS "."
    REQUIRES
                core
                ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS}
)
}
)
)
D_COMPONENTS}
)
}
)
)
)
