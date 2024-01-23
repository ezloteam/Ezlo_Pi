


file(GLOB_RECURSE files_src "./*.c*")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-event-queue
)

idf_component_register(SRCS "${files_src}"
                        INCLUDE_DIRS "."
                        REQUIRES ${EZLOPI_CORE_COMPONENTS})
