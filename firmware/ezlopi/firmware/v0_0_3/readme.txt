
Step - 1:
        esptool.py -p (PORT) -b 460800 --before default_reset --after hard_reset --chip esp32  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0xd000 build/ota_data_initial.bin 0x10000 build/ezpi_3.bin

        _____________________________________________
        |  Address           | Binary Files Name    |    
        _____________________________________________
        |  0x010000          | firmware.bin         |
        ---------------------------------------------    
        |  0x00d000          | ota_data_initial.bin |
        ---------------------------------------------                
        |  0x008000          | partition-table.bin  |
        ---------------------------------------------            
        |  0x001000          | bootloader.bin       |
        ---------------------------------------------        
        |  0x3b0000          | id.bin               |        
        ---------------------------------------------


Step - 2:
        esptool.py -p (PORT) -b 921600 --before default_reset --after hard_reset --chip esp32  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x3b0000 ~/Downloads/100003995.bin


Step - 3:
        Can use "nRF Connect" android app for wifi provisiong using BLE
            - Read the data from the controller
            - Write the data according to above format
            - Example: {"SSID":"my ssid", "PSD":"password"}


Step - 4:
        Used "vera mobile" app for testing
        One can also use "https://home.getvera.com/" (a web app) 