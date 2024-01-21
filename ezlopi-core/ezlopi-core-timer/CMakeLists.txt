


file(GLOB_RECURSE files_src "./*.c*")

idf_component_register(SRCS "${files_src}"
                        INCLUDE_DIRS "."
                        REQUIRES trace ezlopi-core-event-queue)
