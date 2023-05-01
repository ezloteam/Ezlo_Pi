file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "proximity.c"
                        INCLUDE_DIRS "."
                        REQUIRES trace ezlopi_actions ezlopi_timer items gpio_isr_service)