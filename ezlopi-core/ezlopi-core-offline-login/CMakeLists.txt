
file(GLOB_RECURSE config_src "*.c")


idf_component_register(SRCS  "${config_src}"
    INCLUDE_DIRS .
    REQUIRES    cjext 
                ezlopi-core-nvs 
                ezlopi-core-factory-info
)

