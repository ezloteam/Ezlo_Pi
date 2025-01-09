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

# Header File Template Documentation

## Overview
This header file template is designed to provide a consistent structure for header files in a C or C++ project. It includes sections for include files, C++ declaration wrappers, type and macro declarations, extern data declarations, and extern function prototypes.

## File Structure

### Include Files
This section is for including necessary header files. It is denoted by the comment `/* Include Files */`.

### C++ Declaration Wrapper
If the project is intended to be compatible with C++, this section allows for C++ name mangling to be handled correctly. It is denoted by the comment `/* C++ Declaration Wrapper */`.

### Type & Macro Declarations
This section is for declaring any types or macros that will be used in the file. It is denoted by the comment `/* Type & Macro Declarations */`.

### Extern Data Declarations
This section is for declaring any external data that will be used in the file. It is denoted by the comment `/* Extern Data Declarations */`.

### Extern Function Prototypes
This section is for declaring the prototypes of any external functions that will be used in the file. It is denoted by the comment `/* Extern Function Prototypes */`. The function prototype should follow the naming convention `EZPI_maincomponent_subcomponent_functiontitle( type_t arg )`, where `maincomponent` is the main component such as hal, core, service etc., `subcomponent` is the sub component such as i2c from hal, ble from service etc, and `functiontitle` is the title of the function.

# Source File Template Documentation

## Overview
This source file template is designed to provide a consistent structure for source files in a C or C++ project. It includes sections for include files, extern data declarations, extern function declarations, type and macro definitions, static function prototypes, static data definitions, extern data definitions, extern function definitions, and static function definitions.

## File Structure

### Include Files
This section is for including necessary header files. It is denoted by the comment `/* Include Files */`. The include sequence is as follows:
1. C standard libraries (e.g., stdint, string, etc.)
2. Third-party components (e.g., cJSON, freeRTOS, etc.)
3. ESP-IDF specific (e.g., driver/gpio, esp_log, etc.)
4. EzloPi HAL specific (e.g., ezlopi_hal_i2c)
5. EzloPi core specific (e.g., ezlopi_core_ble)
6. EzloPi cloud specific (e.g., ezlopi_cloud_items)
7. EzloPi Service specific (e.g., ezlopi_service_ble)
8. EzloPi Sensors and Device Specific (e.g., ezlopi_device_0001_digitalout_generic)

### Extern Data Declarations
This section is for declaring any external data that will be used in the file. It is denoted by the comment `/* Extern Data Declarations */`.

### Extern Function Declarations
This section is for declaring the prototypes of any external functions that will be used in the file. It is denoted by the comment `/* Extern Function Declarations */`.

### Type & Macro Definitions
This section is for defining any types or macros that will be used in the file. It is denoted by the comment `/* Type & Macro Definitions */`.

### Static Function Prototypes
This section is for declaring the prototypes of any static functions that will be used in the file. It is denoted by the comment `/* Static Function Prototypes */`.

### Static Data Definitions
This section is for defining any static data that will be used in the file. It is denoted by the comment `/* Static Data Definitions */`.

### Extern Data Definitions
This section is for defining any external data that will be used in the file. It is denoted by the comment `/* Extern Data Definitions */`.

### Extern Function Definitions
This section is for defining any external functions that will be used in the file. It is denoted by the comment `/* Extern Function Definitions */`.

### Static Function Definitions
This section is for defining any static functions that will be used in the file. It is denoted by the comment `/* Static Function Definitions */`.

## End of File
The end of the file is denoted by the comment `/* End of File */`.
