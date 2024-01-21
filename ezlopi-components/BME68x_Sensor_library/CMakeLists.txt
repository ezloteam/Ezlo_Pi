

    file(GLOB_RECURSE config_src "*.c")
    idf_component_register(SRCS "bme68x/bme68x.c" "bme68xLibrary.c" 
                                        INCLUDE_DIRS "bme68x" "."
                                        REQUIRES )
