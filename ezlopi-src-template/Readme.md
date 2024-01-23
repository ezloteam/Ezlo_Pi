# CMakeLists.txt Documentation

## Overview
The CMakeLists.txt file is a script that is processed by CMake to generate build files for a project. In the context of an ESP-IDF project, it is used to specify the source files and dependencies for the project.

## File Structure

### Source Files
The source files for the project are specified using the `file(GLOB_RECURSE config_src "*.c")` command. This command searches for all .c files in the current directory and its subdirectories and stores the file paths in the `config_src` variable.

### Core Components
The core components of the project are specified in the `EZLOPI_CORE_COMPONENTS` variable. These components include `ezlopi-core-cloud`, `ezlopi-core-actions`, and `ezlopi-core-devices`.

### HAL Components
The Hardware Abstraction Layer (HAL) components are specified in the `EZLOPI_HAL_COMPONENTS` variable. These components include various hardware interfaces such as `ezlopi-hal-gpio`, `ezlopi-hal-adc`, `ezlopi-hal-pwm`, etc.

### Cloud Components
The cloud components of the project are specified in the `EZLOPI_CLOUD_COMPONENTS` variable. These components include `ezlopi-cloud-settings`.

### Sensor Device Components
The sensor device components of the project are specified in the `EZLOPI_SENSOR_DEVICE_COMPONENTS` variable. These components include various sensor and device drivers.

### Component Registration
The `idf_component_register` function is used to register the source files and dependencies for the project. The `SRCS` argument specifies the source files, the `INCLUDE_DIRS` argument specifies the include directories, and the `REQUIRES` argument specifies the dependencies for the project.
