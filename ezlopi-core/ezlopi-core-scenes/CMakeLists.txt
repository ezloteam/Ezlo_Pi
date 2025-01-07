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


idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS "."
    REQUIRES
                core
                ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS}
)
