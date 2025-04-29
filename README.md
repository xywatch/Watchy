# Unofficially Maintained Watchy  

Since the official [Watchy](https://github.com/sqfmi/Watchy) repository has not been maintained for a long time and there are no plans to continue maintenance, this repository is forked to provide ongoing support.  

Please use **PlatformIO** for development.

### Differences Between V2 and V3:  

|      | v2      | v3      |
|------------|------------|------------|
| SoC     | ESP32-PICO-D4     | ESP32-S3FN8      |
| RTC      | PCF8536      | EXT 32KHz Crystal      |
| USB-Serial      | CP2102N/CH340x      | Built-in CDC/JTAG      |
| PCB | [xywatch/watchy-hardware-v2](https://github.com/xywatch/watchy-hardware-v2) | [xywatch/watchy-hardware-v3](https://github.com/xywatch/watchy-hardware-v3)|

Current issues with v3: Using EXT 32KHz Crystal clock results in 1-2 minutes of inaccuracy after several days and high power consumption.

### Key Notes:  
- **PlatformIO** is the recommended development environment.  
- Ensure the correct `.ini` file is used for your Watchy version (V2/V3).  

### v3: Enter Bootloader Mode (esp32-s3)
To upload new firmware/watchfaces to Watchy, you will need to enter bootloader mode
1. Plug in the USB on Watchy
1. Press and hold the top 2 buttons (Back & Up) for more than 4 seconds, 
1. then release the Back button first, 
1. before releasing the Up button

You should now see an ESP32S3 device enumerate a serial port i.e. COM, cu.*


# 非官方维护 Watchy

因为官方 [Watchy](https://github.com/sqfmi/Watchy) 已很久没有维护且也没有打算继续维护的计划, 所以fork该仓库来维护.

请使用 paltformio 来开发

v2与v3的差异:

|      | v2      | v3      |
|------------|------------|------------|
| SoC     | ESP32-PICO-D4     | ESP32-S3FN8      |
| RTC      | PCF8536      | EXT 32KHz Crystal      |
| USB-Serial      | CP2102N/CH340x      | Built-in CDC/JTAG      |
| PCB | [xywatch/watchy-hardware-v2](https://github.com/xywatch/watchy-hardware-v2) | [xywatch/watchy-hardware-v3](https://github.com/xywatch/watchy-hardware-v3)|

v3当前的问题: 使用EXT 32KHz Crystal时钟几天后就会不准1-2分钟且功耗大, 可能原因: 依赖外部晶振，若晶振功耗高或匹配电路不佳，可能导致额外耗电

如何解决:
* 检查 32.768kHz 晶振 的负载电容（通常 12.5pF）是否匹配
* 换用 高精度晶振（如 Seiko Epson MC-306）

v3进入下载模式:
1. 同时按 BACK 和 UP 键大约4s
2. 松开 BACK
3. 松开 UP

[参考](https://blog.csdn.net/qq_62078117/article/details/135510767)

# Watchy - Fully Open Source E-Paper Watch

![Watchy](https://watchy.sqfmi.com/img/watchy_render.png)

**Buy Watchy from [Mouser](https://www.mouser.com/ProductDetail/SQFMI/SQFMI-WATCHY-10?qs=DRkmTr78QARN9VSJRzqRxw%3D%3D), [The Pi Hut](https://thepihut.com/collections/sqfmi), and [Crowd Supply](https://www.crowdsupply.com/sqfmi/watchy)**

[**Watchy Case & Accessories**](https://shop.sqfmi.com)

## Getting Started Guide
Follow the instructions here https://watchy.sqfmi.com/docs/getting-started

### Have Fun! :)

### Got Questions?

Join our [Discord](https://discord.gg/ZXDegGV8E7)


