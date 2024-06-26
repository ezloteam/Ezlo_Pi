# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)

# message(STATUS "current dir: ${CMAKE_CURRENT_SOURCE_DIR}")
execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/update_device_list.sh"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/xyz.txt"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

# ADD_CUSTOM_COMMAND( OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/output_ff.txt"
# COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/update_device_list.sh
# DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/update_device_list.sh)

# Scans directories in 'curdir' and returns the list in 'result'
MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children ${curdir}/*) # puts the list of all the components(files and/or folders) of 'curdir' in 'children'
    SET(dirlist "") # clear the list 'dirlist'

    FOREACH(child ${children}) # runs for loop for each components in 'children' and puts in 'child' one by one
        IF(IS_DIRECTORY ${child}) # check if the 'child' is directory or file
            LIST(APPEND dirlist ${child}) # if the 'child' is directory, append it to 'dirlist'
            message(STATUS "linking '${child}'.")
        ENDIF()
    ENDFOREACH()

    SET(${result} ${dirlist}) # copy the components of 'dirlist' into 'result'(return variable)
ENDMACRO()

# list of external components
set(external_components_list 
                                ezlopi-components 
                                ezlopi-sensors-devices 
                                ezlopi-lua-modules
                                ezlopi-services 
                                ezlopi-cloud 
                                ezlopi-core 
                                ezlopi-hal
                                ezlopi-util
                                )  

                                # 
# set_target_properties(your_subdir2 PROPERTIES EXCLUDE_FROM_ALL TRUE)

set(EXTRA_COMPONENT_DIRS "") # clearing variable 'EXTRA_COMPONENT_DIRS'
set(ALL_COMPONENTS "") # clearing 'ALL_COMPONENTS' variable

foreach(component ${external_components_list})
    message(STATUS "  \r\n")
    message(STATUS "linking '${component}:: ")
    set(SUBDIRS, "") # clearing 'SUBDIRS'
    SUBDIRLIST(SUBDIRS ${component}) # scans and gets the list of directories from 'services' in 'SUBDIRS'
    LIST(APPEND ALL_COMPONENTS ${SUBDIRS})
endforeach()

LIST(APPEND ALL_COMPONENTS ezlopi-user-config ezlopi-main)

set(EXTRA_COMPONENT_DIRS ${ALL_COMPONENTS} ) # add the list of directories 'SUBDIRS' to 'EXTRA_COMPONENT_DIRS'(ESP32 VARIABLES), links the external components
message(STATUS "EXTRA_COMPONENTS_DIRS: ${EXTRA_COMPONENT_DIRS}\r\n")

# Add AddressSanitizer flags
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

# # Set linker flags for AddressSanitizer
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

# Add components that needs to be excluded
set(EXCLUDE_COMPONENTS freemodbus)

## ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ##
project(Ezlo_Pi_v4x)
