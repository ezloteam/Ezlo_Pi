# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c")

idf_component_register(SRCS "timer_service.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES driver freertos trace web_provisioning items 
    ezlopi_actions ezlopi_event_queue ezlopi_devices)