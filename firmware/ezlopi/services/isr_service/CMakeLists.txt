


file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "isr_service.c" "${src_files}"
                        INCLUDE_DIRS "."
                        REQUIRES trace ezlopi_devices)



