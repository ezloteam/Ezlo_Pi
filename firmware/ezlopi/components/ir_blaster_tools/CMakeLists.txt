set(component_srcs "ir_init.c")

idf_component_register(SRCS "ir_init.c" "${component_srcs}"
                       INCLUDE_DIRS "."
                       REQUIRES trace)
