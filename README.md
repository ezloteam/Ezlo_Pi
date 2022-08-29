
[![N|Ezlo-Pi](https://www.ezlopi.com/wp-content/uploads/2022/07/Logo.svg)](https://www.ezlopi.com/)

[![GitHub issues](https://img.shields.io/github/issues/ezloteam/Ezlo_Pi)](https://github.com/ezloteam/Ezlo_Pi/issues) [![GitHub forks](https://img.shields.io/github/forks/ezloteam/Ezlo_Pi)](https://github.com/ezloteam/Ezlo_Pi/network) [![GitHub stars](https://img.shields.io/github/stars/ezloteam/Ezlo_Pi)](https://github.com/ezloteam/Ezlo_Pi/stargazers) [![GitHub license](https://img.shields.io/github/license/ezloteam/Ezlo_Pi)](https://github.com/ezloteam/Ezlo_Pi)


## _Steps to setup working environment for Ezlo-Pi_

[Ezlo-Pi: Master](https://github.com/ezloteam/Ezlo_Pi/tree/master)

  

# 1. Setting esp-idf in Linux

#### 1. Clone esp-idf reposository

```bash

git clone -b v4.4.1 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
```

#### 2. Install esp-idf

```bash
./install.sh
```

#### 3. export esp-idf

```bash
. ./export.sh
```

#### OR

- Follow the instructions: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-setup.html

# 2. Setting esp-idf in Windows

**Follow this instructions: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/windows-setup.html**

**OR**

#### 1. Download esp-idf installer (version: v4.4.1)
[https://dl.espressif.com/dl/idf-installer/esp-idf-tools-setup-offline-4.4.1.exe?](https://dl.espressif.com/dl/idf-installer/esp-idf-tools-setup-offline-4.4.1.exe?  "https://dl.espressif.com/dl/idf-installer/esp-idf-tools-setup-offline-4.4.1.exe?")

#### 2. Install _"esp-idf-tools-setup-offline-4.4.1.exe"_

# 3. Settings up Ezlo-Pi source code

#### 1. Getting Ezlo-Pi repository from github

Open terminal or bash-shell and run the below commands

```bash

git clone --recursive git@github.com:ezloteam/Ezlo_Pi.git ezlopi

cd ezlopi/firmware/ezlopi

```

#### 2. Build

using ```idf.py```

```bash

idf.py build

```

Using custom command

###### a. To generate test release firmware:

Generates .bin files in firmware/[version] folder. The below command also increases the build number.

```bash

./build.sh test

```

###### b. To only build:

Does not generate any .bin files in firmware/[version] folder. This command will only increase the build number in version.

```bash

./build.sh build

```

###### c. Generate release firmware:

Generates .bin files in firmware/[version] folder. The below command also creates the .bin files in firmware/[version] folder

```bash

./build.sh release

```

#### 3. flash the firmware

using usb port and baudrate

```bash

idf.py flash

```

using specific port and baudrate

Linux:

```bash

idf.py -p /dev/ttyUSB0 -b 921600 flash

```

Windows:

```bash

idf.py -p COM9 -b 921600 flash

```

#### 4. Monitor on console

using usb port and baudrate

```bash

idf.py monitor

```

using specific port:

Linux:

```bash

idf.py -p /dev/ttyUSB0 monitor

```

Windows:

```bash

idf.py -p COM9 monitor

```

## 4: Flashing firmware using QT-App

## 5: Working with examples

[Fllow from the examples](https://www.ezlopi.com/examples/)
1. [EzloPi AC Lamp setup with Relay interfacing​](https://www.ezlopi.com/examples/relay-circuitry-and-lamp-circuit-setup/)
2. [EzloPi AC lamp setup with relay and a momentary switch](https://www.ezlopi.com/examples/ezlopi-ac-lamp-setup-with-relay-and-a-momentary-switch/)
3. [EzloPi I2C MPU6050 Example](https://www.ezlopi.com/examples/ezlopi-i2c-mpu6050-example/)