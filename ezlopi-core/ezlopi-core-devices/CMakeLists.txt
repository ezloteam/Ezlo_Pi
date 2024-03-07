file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-cloud 
        ezlopi-core-actions
        ezlopi-core-devices 
)
set(EZLOPI_HAL_COMPONENTS 
        ezlopi-hal-gpio
        ezlopi-hal-adc
        ezlopi-hal-pwm
        ezlopi-hal-onewire     
        ezlopi-hal-uart                           
        ezlopi-hal-i2c
        ezlopi-hal-spi
)
set(EZLOPI_CLOUD_COMPONENTS 
        ezlopi-cloud-settings
)
set(EZLOPI_SENSOR_DEVICE_COMPONENTS 
        device-0001-digitalOut-generic
        device-0002-digitalOut-relay
        device-0003-digitalOut-plug
        device-0004-digitalIn-generic

        sensor-0005-I2C-MPU6050
        sensor-0007-I2C-GY271
        sensor-0006-I2C-ADXL345
        sensor-0008-I2C-LTR303ALS
        device-0009-other-RMT-SK6812
        sensor-0010-I2C-BME680 
        sensor-0012-I2C-BME280
        sensor-0015-oneWire-DHT11
        sensor-0016-oneWire-DHT22

        sensor-0017-ADC-potentiometer
        sensor-0018-other-internal-hall-effect
        # sensor-0019-digitalIn-PIR
        # sensor-0020-other-2axis-joystick
        # sensor-0021-UART-MB1013
        # device-0022-PWM-dimmable-lamp
        # sensor-0023-digitalIn-touch-switch-TTP223B
        # sensor-0024-other-HCSR04
        # sensor-0025-digitalIn-LDR
        # sensor-0026-ADC-LDR
        sensor-0027-ADC-waterLeak                                

        # sensor-0028-other-GY61
        # sensor-0029-I2C-GXHTC3
        # sensor-0030-oneWire-DS18B20
        # sensor-0031-other-JSNSR04T
        sensor-0032-ADC-soilMoisture   
        # sensor-0033-ADC-turbidity   
        # sensor-0034-digitalIn-proximity 
        # sensor-0035-digitalIn-touch-sensor-TPP223B                                                       
        # device-0036-PWM-servo-MG996R
        # sensor-0037-pms5003-sensor
        # device-0038-other-RGB
                
        # sensor-0040-other-TCS230          
        # sensor-0041-ADC-FC28-soilMoisture  
        # sensor-0042-ADC-shunt-voltmeter                                
        # sensor-0043-ADC-GYML8511-UV-intensity
        # sensor-0044-I2C-TSL256-luminosity
        # sensor-0046-ADC-ACS712-05B-currentmeter

        # sensor-0047-other-HX711-loadcell
        # sensor-0048-other-MQ4-CH4-detector
        # sensor-0049-other-MQ2-LPG-detector
        # sensor-0050-other-MQ3-alcohol-detector
        # sensor-0051-other-MQ8-H2-detector
        # sensor-0052-other-MQ135-NH3-detector
        # sensor-0053-UART-GYGPS6MV2
        # sensor-0054-PWM-YFS201-flowmeter
        sensor-0055-ADC-FlexResistor
        # sensor-0056-ADC-Force-Sensitive-Resistor                             
        # sensor-0057-other-KY026-FlameDetector
        # sensor-0059-other-MQ6-LPG-detector
        # sensor-0060-digitalIn-vibration-detector
        # sensor-0061-digitalIn-reed-switch
        # sensor-0062-other-MQ7-CO-detector
        # sensor-0063-other-MQ9-LPG-flameable-detector
        # sensor-0065-digitalIn-float-switch
        # sensor-0066-other-R307-FingerPrint
        # sensor-0067-UART-hilink-presence-sensor
        # sensor-0068-I2C-ENS160-gas-sensor
        # sensor-0069-UART-ze08-ch02-gas-sensor
)


idf_component_register(SRCS "ezlopi_core_device_value_updated.c" "ezlopi_core_devices.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}                
                        )


