
[![N|Ezlo-Pi](https://www.ezlopi.com/wp-content/uploads/2022/07/Logo.svg)](https://www.ezlopi.com/)

[![GitHub issues](https://img.shields.io/github/issues/ezloteam/Ezlo_Pi)](https://github.com/ezloteam/Ezlo_Pi/issues) [![GitHub forks](https://img.shields.io/github/forks/ezloteam/Ezlo_Pi)](https://github.com/ezloteam/Ezlo_Pi/network) [![GitHub stars](https://img.shields.io/github/stars/ezloteam/Ezlo_Pi)](https://github.com/ezloteam/Ezlo_Pi/stargazers) [![License](https://img.shields.io/github/license/ezloteam/Ezlo_Pi/)](https://github.com/ezloteam/Ezlo_Pi/blob/master/LICENCE.txt)

# ezlopi
EzloPi is an open-source project contributed by Ezlo Innovation to extend the capabilities of ESP32 chipset-based devices and platforms. It provides unparalleled capabilities to configure and control your ESP-based devices and bring any of your automation ideas to life.

Checkout our [website](https://www.ezlopi.com/) for further guide and examples

## supported features
* __GPIO__
* __Analog Input__
* __Digital Input__
* __I2C__
* __One Wire__

## _Steps to setup working environment for Ezlo-Pi_

[Ezlo-Pi: Master](https://github.com/ezloteam/Ezlo_Pi/)


# 1. Getting started
EzloPi has a windows based configurator application to configure the ESP32 for any of the feature mentioned above at supported features. The desktop application can be used to configure the ESP32 device for interfacing Relay, and analog input, digital input, an I2C based sensor etc in no time. After the device has been configured using the desktop app, anyone can use our cloud and mobile app serviece to get control of device from anywhere in real-time.

![EzloPi Desktop UI](https://github.com/ezloteam/Ezlo_Pi/blob/master/ezlopi_screenshot.png)

# 2. Customizing and building EzloPi firmware in your local setup
It is required to have [ESP-IDF](https://www.espressif.com/en/products/sdks/esp-idf) installed in your machine.
Install ESP-IDF with taking help from the docs [here](https://docs.espressif.com/projects/esp-idf/en/v4.4.2/esp32/get-started/index.html). IDF version in this project : 4.4.1.

#### 1. Clone Ezlo-Pi repository from github
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
###### c. Build for release firmware:
Generates .bin files in firmware/[version] folder. The below command also creates the .bin files in firmware/[version] folder
```bash
./build.sh release
```
#### 3. flash the firmware
using usb port and baudrate
```bash
idf.py -p <COM Port Name> -b <baud> flash
```
Monitor
```bash
idf.py -p <COM Port Name> monitor
```

# 3: Customizing Desktop app:
The desktop application has been build under QT V 5.15. QT creator with necessary compiler should be set in your local machine. 
The installer script is under _deploy_ directory.

# 4: Working with examples

[Fllow from the examples](https://www.ezlopi.com/examples/)
1. [EzloPi AC Lamp setup with Relay interfacing​](https://www.ezlopi.com/examples/relay-circuitry-and-lamp-circuit-setup/)
2. [EzloPi AC lamp setup with relay and a momentary switch](https://www.ezlopi.com/examples/ezlopi-ac-lamp-setup-with-relay-and-a-momentary-switch/)
3. [EzloPi I2C MPU6050 Example](https://www.ezlopi.com/examples/ezlopi-i2c-mpu6050-example/)
