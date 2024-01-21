
file(GLOB_RECURSE src_files "*.c*")

idf_component_register(SRCS "${src_files}"
                        INCLUDE_DIRS .
                        REQUIRES trace ezlopi-core-actions  ezlopi-core-timer ezlopi-core-devices ezlopi-cloud-constants ezlopi-service-gpioisr ezlopi-core-cloud ld2410)

