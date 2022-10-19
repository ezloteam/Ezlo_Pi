# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c")

idf_component_register(SRCS "MPU6050_service.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES driver freertos devices_common interface_common trace mpu6050 web_provisioning items i2c_master_interface)