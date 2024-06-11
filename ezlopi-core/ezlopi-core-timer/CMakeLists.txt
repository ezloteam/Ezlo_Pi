


file(GLOB_RECURSE files_src "./*.c*")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-event-queue
        ezlopi-core-errors
)

idf_component_register(SRCS "ezlopi_core_timer.c" "${files_src}"
                        INCLUDE_DIRS "."
                        REQUIRES ${EZLOPI_CORE_COMPONENTS})
