# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

set(ESP_IDF_COMPONENTS 
        json
)


idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES ${ESP_IDF_COMPONENTS})
