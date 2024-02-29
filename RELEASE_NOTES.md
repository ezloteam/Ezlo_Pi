
<picture>
  <img src="ezlopi-doc/ezlopi_logo.png" >
</picture>

# RELEASE NOTES

## Supported Ezlo Cloud APIs

1. [Items](https://api.ezlo.com/hub/items_api/index.html)
   - [hub.items.list](https://api.ezlo.com/hub/items_api/index.html#hubitemslist)
   - [hub.item.value.set (single item)](https://api.ezlo.com/hub/items_api/index.html#hubitemvalueset-single-item)

2. [Devices](https://api.ezlo.com/hub/devices_api/index.html)
   - [hub.devices.list](https://api.ezlo.com/hub/devices_api/index.html#hubdeviceslist)
   - [hub.device.name.set](https://api.ezlo.com/hub/devices_api/index.html#hubdevicenameset)
   - [hub.device.room.set](https://api.ezlo.com/hub/devices_api/index.html#hubdeviceroomset)

3. [Device Settings](https://api.ezlo.com/hub/settings_api/index.html)
   - [hub.device.settings.list](https://api.ezlo.com/hub/settings_api/index.html#hubdevicesettingslist)
   - [hub.device.setting.value.set](https://api.ezlo.com/hub/settings_api/index.html#hubdevicesettingvalueset)
   - [hub.device.setting.reset](https://api.ezlo.com/hub/settings_api/index.html#hubdevicesettingreset)

4. [Room](https://api.ezlo.com/hub/room/index.html)
    - [hub.room.list](https://api.ezlo.com/hub/room/index.html#hubroomlist)
    - [hub.room.create](https://api.ezlo.com/hub/room/index.html#hubroomcreate)
    - [hub.room.name.set](https://api.ezlo.com/hub/room/index.html#hubroomnameset)
    - [hub.room.get](https://api.ezlo.com/hub/room/index.html#hubroomget)
    - [hub.room.delete](https://api.ezlo.com/hub/room/index.html#hubroomdelete)

5. [Common commands](https://api.ezlo.com/hub/common/index.html)
    - [hub.info.get](https://api.ezlo.com/hub/common/index.html#hubinfoget)
    - [hub.reboot](https://api.ezlo.com/hub/common/index.html#hubreboot)

6. [Network](https://log.ezlo.com/new/hub/network/)
    - [hub.network.get](https://log.ezlo.com/new/hub/network/#hubnetworkget)
    - [hub.network.wifi.scan.start](https://log.ezlo.com/new/hub/network/#hubnetworkwifiscanstart)
    - [hub.network.wifi.scan.stop](https://log.ezlo.com/new/hub/network/#hubnetworkwifiscanstop)

7. [Time configuration](https://api.ezlo.com/hub/time/index.html)
    - [hub.time.location.set](https://api.ezlo.com/hub/time/index.html#hubtimelocationset)
    - [hub.time.location.get](https://api.ezlo.com/hub/time/index.html#hubtimelocationget)
    - [hub.time.location.list](https://api.ezlo.com/hub/time/index.html#hubtimelocationlist)

7. [House Modes commands](https://api.ezlo.com/hub/house_modes_manager/index.html#house-modes-commands)
    - [hub.modes.get](https://api.ezlo.com/hub/house_modes_manager/index.html#hubmodesget)
    - [hub.modes.current.get](https://api.ezlo.com/hub/house_modes_manager/index.html#hubmodescurrentget)
    - [hub.modes.switch](https://api.ezlo.com/hub/house_modes_manager/index.html#hubmodesswitch)


8. [Broadcasts](https://api.ezlo.com/hub/broadcasts/index.html)
    - [hub.item.updated](https://api.ezlo.com/hub/broadcasts/index.html#hubitemupdated)
    - [hub.room.created](https://api.ezlo.com/hub/broadcasts/index.html#hubroomcreated)
    - [hub.room.deleted](https://api.ezlo.com/hub/broadcasts/index.html#hubroomdeleted)
    - [hub.room.reordered](https://api.ezlo.com/hub/broadcasts/index.html#hubroomreordered)
    - [hub.room.edited](https://api.ezlo.com/hub/broadcasts/index.html#hubroomedited)
    - [hub.scene.added](https://api.ezlo.com/hub/broadcasts/index.html#hubsceneadded)
    - [hub.scene.deleted](https://api.ezlo.com/hub/broadcasts/index.html#hubscenedeleted)
    - [hub.scene.changed](https://api.ezlo.com/hub/broadcasts/index.html#hubscenechanged)
    - [hub.scene.run.progress](https://api.ezlo.com/hub/broadcasts/index.html#hubscenerunprogress)
    - [hub.network.wifi.scan.progress](https://api.ezlo.com/hub/broadcasts/index.html#hubnetworkwifiscanprogress)
    - [hub.device.setting.updated](https://api.ezlo.com/hub/broadcasts/index.html#hubdevicesettingupdated)    

9. [Scenes](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html)
    - [hub.scenes.create](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenescreate)
    - [hub.scenes.get](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesget)
    - [hub.scenes.list](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubsceneslist)
    - [hub.scenes.edit](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesedit)
    - [hub.scenes.delete](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesdelete)
    - [hub.scenes.block.data.list](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesblockdatalist)
    - [hub.scenes.enabled.set](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesenabledset)
    - [hub.scenes.notification.add](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesnotificationadd)
    - [hub.scenes.notification.remove](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesnotificationremove)
    - [hub.scenes.run](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesrun)
    - [hub.scenes.name.set](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesnameset)
    - [hub.scenes.expressions.set](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesexpressionsset)
    - [hub.scenes.expressions.delete](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesexpressionsdelete)
    - [hub.scenes.expressions.list](https://api.ezlo.com/hub/scenes/local_scenes_api/index.html#hubscenesexpressionslist)


10. [Scenes : When blocks](https://api.ezlo.com/hub/scenes/when_blocks/index.html)
    - [isItemState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isitemstate)
    - [isItemStateChanged](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isitemstatechanged)
    - [isInterval](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isinterval)
    - [isSunState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#issunstate)
    - [isOnce](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isonce)
    - [isDateRange](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isdaterange)
    - [isDeviceState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isdevicestate)
    - [isSceneState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isscenestate)
    - [isCloudState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#iscloudstate)
    - [compareNumbers](https://api.ezlo.com/hub/scenes/when_blocks/index.html#comparenumbers)
    - [compareNumberRange](https://api.ezlo.com/hub/scenes/when_blocks/index.html#comparenumberrange)
    - [compareStrings](https://api.ezlo.com/hub/scenes/when_blocks/index.html#comparestrings)
    - [stringOperation](https://api.ezlo.com/hub/scenes/when_blocks/index.html#stringoperation)
    - [inArray](https://api.ezlo.com/hub/scenes/when_blocks/index.html#inarray)
    - [compareValues](https://api.ezlo.com/hub/scenes/when_blocks/index.html#comparevalues)
    - [hasAtLeastOneDictionaryValue](https://api.ezlo.com/hub/scenes/when_blocks/index.html#hasatleastonedictionaryvalue)
    - [isFirmwareUpdateState](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isfirmwareupdatestate)
    - [isDictionaryChanged](https://api.ezlo.com/hub/scenes/when_blocks/index.html#isdictionarychanged)
    - [and](https://api.ezlo.com/hub/scenes/when_blocks/index.html#and)
    - [not](https://api.ezlo.com/hub/scenes/when_blocks/index.html#not)
    - [or](https://api.ezlo.com/hub/scenes/when_blocks/index.html#or)
    - [xor](https://api.ezlo.com/hub/scenes/when_blocks/index.html#xor)
  
11. [Scenes : Action blocks](https://api.ezlo.com/hub/scenes/action_blocks/index.html#action-blocks)
    - [setItemValue](https://api.ezlo.com/hub/scenes/action_blocks/index.html#setitemvalue)
    - [sendHttpRequest](https://api.ezlo.com/hub/scenes/action_blocks/index.html#sendhttprequest)
    - [runCustomScript](https://api.ezlo.com/hub/scenes/action_blocks/index.html#runcustomscript)
    - [rebootHub](https://api.ezlo.com/hub/scenes/action_blocks/index.html#reboothub)
    - [resetHub](https://api.ezlo.com/hub/scenes/action_blocks/index.html#resethub)


## Supported Sensors


1. MPU6050 Accelerometer and Gyroscope
2. ADXL345 3-Axis Accelerometer
3. GY-271 Magnetometer
4. LTR303ALS Luminosity sensor
5. RMT SK6812
6. BMP680 I2C Sensor
7. BMP280 I2C Sensor
8. DHT11 Temperature Humidity Sensor
9. DHT22 Temperature Humidity Sensor
10. Potentiometer
11. Internal Hall Effect Sensor
12. PIR Sensor
13. Parallax 2-Axis Joystick
14. MB1013 Sonar
15. PWM Generic Load
16. Touch Switch TTP223B (Toggle)
17. Ultrasonic Sensor HC SR04
18. LDR Digital and analog Sensor
19. Water Leak Sensor
20. GY61 3-Axis Accelerometer
21. GXHTC3 - RH and T Sensor
22. DS18B20 Temperature Sensor
23. JSN SR04T Water Level Sensor
24. Soil Moisture Sensor
25. Turbidity Sensor
26. Proximity Sensor
27. Touch switch TPP 223B (Momentary)
28. Servo Motor MG996R
29. SK6812 RGB LEDs
30. TCS230 Sensor
31. FC28 Soil Moisture Sensor
32. Shunt voltmeter
33. UV Intensity Sensor
34. TSL2561 Luminosity Sensor
35. CS712TELC 05B Current Sensor
36. HX711 Load Cell
37. MQ4 Methane Gas Sensor
38. MQ2 LPG Gas Sensor
39. MQ3 Alcohol Sensor
40. MQ8 H2 Gas Sensor
41. MQ135 NH3 Gas Sensor
42. GPS GYGPS6MV2
43. YFS201 flowmeter
44. Flex Resistor
45. Force Sensitive Resistor
46. KY026 Flame Sensor
47. MQ6 LPG Detector
48. Vibration Sensor
49. Reed Switch
50. MQ7 CO Detector
51. MQ9 LPG Detector
52. Float Switch
53. R307 Fingerprint Module
54. HLK-LD2410B Presence Sensor
55. ENS160 gas sensor
56. ZE08 ch02 gas sensor

## Supported Controllable Devices 
1. PWM Dimmable 
2. RGB Dimmable 
3. Smart Plug / Relay
4. Servo Mot0r
5. Addressable LEDs

## Firmware Features
### Serial Command
EzloPi can be configured with serial interface as well. Following are common serial interface commands : 
#### Action: Device management
This command is for device administrative purpose such as device restart, device factory restart etc
```
Request 

Factory Restore
{
    "cmd": 0,
    "sub_cmd": 0 // Factory Restore only
}

Just Reboot
{
    "cmd": 0,
    "sub_cmd": 1 // Reboot only
}

Response : 
{
  "cmd": 0,
  "status": 1 // Status of the command execution, 0 = failed, 1 = success 
}
```
#### Action: Get Info
```
Request 

{
"cmd": 1
}
Just Reboot
{
    "cmd": 0,
    "sub_cmd": 1 // Reboot only
}

Response : 

{
    "cmd": 1,
    "firmware_version": "3.3.6",
    "build": 185,
    "chip_info": {
        "chip_type": "ESP32-S3",
        "chip_version": "0.01",
        "firmware_SDK_name": "ESP-IDF",
        "firmware_SDK_version": "v4.4.6-dirty"
    },
    "uptime": "0d 0h 30m 7s",
    "build_date": 1701862983,
    "boot_count": 354,
    "boot_reason": "Power ON",
    "mac": "2A:4B:6F:8E:1D:3C",
    "uuid": "fb3c84b0-534c-11ec-b2d6-8f260f5287fa",
    "uuid_prov": "065320-838d-11ee-8889-0f56091e7639",
    "serial": 102000529,
    "ssid": "ESP",
    "device_name": "__NAME",
    "ezlopi_device_type": "ezlopi_generic",
    "device_total_flash": "8MB",
    "brand": "__BRAND",
    "manufacture": "__MANU",
    "model": "__MODEL",
    "net_info": {
        "wifi_mode": "STA",
        "wifi_connection_status": true,
        "ip_sta": "192.168.100.19",
        "ip_nmask": "255.255.255.0",
        "ip_gw": "192.168.100.2",
        "internet_status": "unknown",
        "cloud_connection_status": false
    }
}
```
#### Action: Set Wi-Fi
```
Request 

{
  "cmd": 2,
  "pass": "myWiFiPass",
  "ssid": "my wifi ssid"
}

Response : 
{
  "cmd": 2,
  "status_write": 1,
  "status_connect": 1
}
```
#### Action Device Config
```
Request 

{
  "cmd": 4
}

Response : 
	
{
    "config_id": "_ID_",
    "config_time": 1696508363,
    "dev_total": 3,
    "config_name": "My moisture sensor",
    "dev_detail": [
        {
            "dev_type": 1,
            "dev_name": "Dining Room Main Lamp",
            "id_room": "",
            "id_item": 2,
            "val_ip": true,
            "val_op": false,
            "gpio_in": 21,
            "gpio_out": 13,
            "is_ip": false,
            "ip_inv": true,
            "pullup_ip": true,
            "pullup_op ": true,
            "op_inv": false
        },
        {
            "dev_type": 2,
            "dev_name": "Switch",
            "id_room": "",
            "id_item": 4,
            "gpio": 21,
            "pull_up": true,
            "logic_inv": true
        },
        {
            "dev_type": 8,
            "dev_name": "Sensor for my Dog",
            "id_room": "",
            "id_item": 5,
            "gpio_sda": 21,
            "gpio_scl": 22,
            "pullup_scl": true,
            "pullup_sda": true,
            "slave_addr": 105
        }
    ]
}
```

### BLE WiFi Onboarding 
BLE (Bluetooth Low Energy) WiFi on-boarding for EzloPi Devices simplifies the process of connecting EzloPi devices to a WiFi network. Using BLE technology, the device can initiate a secure and user-friendly on-boarding process without the need for direct user input on the device. 
### BLE Provisioning 
BLE provisioning for EzloPi devices enhances user convenience by providing a wireless and user-friendly method for initial setup, eliminating the need for complex manual configurations directly on the device.
### BLE Pin Configurations 
BLE Configuration simplifies the process of configuring the GPIOs of a hardware microcontroller employed by EzloPi, enhancing user convenience.
### OTA 
This feature allows for convenient and remote firmware upgrades, ensuring that EzloPi devices can receive the latest software improvements and enhancements without the need for manual intervention or physical access.

### mDNS Discovery
The EzloPi can be discovered locally with following services. The hostname would be ```ezlopi_<last 4 digits of Serial id>```

## Local Meshbot (Scene)
Scenes provide the possibility to make the relations between devices and make some actions with them. A scene consists of "conditions" and "actions" represented by (when blocks and action blocks respectively. All of them, except action groups, have Json Array representation.

In the perfect case when and exceptions ("conditions") block must form Abstract Syntax Tree (AST) from the Json and be executable. Currently, it's represented as a list of possible single-argument function.
then and else (action) blocks contain the list of actions. Each functional element actually is the item. As far as you can see there are read only items such as : temperature, pressure, humidity, etc. and there are read-write items : switch, dimmer, light(switch), doorlock, etc...