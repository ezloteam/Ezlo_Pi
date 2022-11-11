# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "./*.c*")

idf_component_register(SRCS "gpio_isr_service.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES driver freertos trace ezlopi_devices)