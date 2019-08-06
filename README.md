# ESP32-CAM-Print-Monitor
Camera print monitor for Marlin-based 3D Printers

## Pictures
[![picture alt](https://i.ibb.co/0BXkSBM/IMG-20190727-165736.jpg "Mounted on Ender 3")](https://ibb.co/0BXkSBM)
[![picture alt](https://i.ibb.co/4ZX3y97/IMG-20190727-165721.jpg "Power and serial wires to module")](https://ibb.co/4ZX3y97)
[![picture alt](https://i.ibb.co/71xF6GT/IMG-20190728-092931.jpg "Module with voltage limiting diodes visible")](https://ibb.co/71xF6GT)
[![picture alt](https://i.ibb.co/943KXT8/IMG-20190728-092938.jpg "Module with voltage limiting diodes visible")](https://ibb.co/943KXT8)
[![picture alt](https://i.ibb.co/WxJRKwY/IMG-20190728-092943.jpg "Module with voltage limiting diodes visible")](https://ibb.co/WxJRKwY)

## Video
[![VIDEO](https://img.youtube.com/vi/KHugenK15AQ/0.jpg)](https://www.youtube.com/watch?v=KHugenK15AQ)

## Getting Started

You will need:
1.  ESP32-CAM device
2.  USB to Serial bridge adapter
3.  2 x 3.3V Zener Diodes 1N4728
4.  Suitable housing for camera
5.  Marlin-based 3D Printer
5.  Serial connection from 3D Printer
6.  5V Power source or 5V connection from Printer

## Connection diagram

Example without mains relay function:

[![picture alt](https://i.ibb.co/stBdbZn/Ender3-pptx-Microsoft-Power-Point-000292.jpg "Wiring diagram without mains relay")](https://ibb.co/stBdbZn)



Example including mains relay function:

[![picture alt](https://i.ibb.co/N6F7hCz/Ender3-relay-pptx-Microsoft-Power-Point-000291.jpg "Wiring diagram with mains relay")](https://ibb.co/N6F7hCz)

### TODO

*  Implement Authentication
*  ~~Implement expandable menu to:~~
   *  ~~Configurable data refresh rate~~
   *  ~~Ability to reboot device~~
*  ~~Test Mains Relay code~~
*  GCode helper
*  Dream goal..... copy files to SD-Card over WiFi... FAST.....

```
In the code I'm using the following:
Serial connection to printer GPIO 14 & 15 - diodes between both these and GND to level shift to 3.3V - observe polarity
Pay attention to set the serial connection speed to the appropriate one for your printer.
GPIO 13 for relay that switches mains on/off  - not implemented yet.
```


## Acknowledgments

* @luc-github
* @Electronlibre2012

