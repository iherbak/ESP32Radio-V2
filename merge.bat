python C:\Users\XXXXX\.platformio\packages\tool-esptoolpy\esptool.py --chip esp32 merge_bin -o .pio/build/esp32/merged-flash.bin --flash_mode qio --flash_size 4MB 0x1000 .pio/build/esp32/bootloader.bin 0x8000 .pio/build/esp32/partitions.bin 0x10000 .pio/build/esp32/firmware.bin 0x3D0000 .pio/build/esp32/spiffs.bin