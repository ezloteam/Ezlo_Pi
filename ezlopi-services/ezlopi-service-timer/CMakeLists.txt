# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c")

idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES driver freertos trace ezlopi-service-webprov ezlopi-cloud-items 
    ezlopi-core-actions ezlopi-core-event-queue ezlopi-core-devices)