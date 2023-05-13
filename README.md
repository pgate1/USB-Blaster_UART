# USB-Blaster UART with VirtualJTAG

Let's communicate between PC user software and FPGA via Altera USB-Blaster.
The target FPGA boards are DE0, DE1, DE2-115, DE0-CV, etc. made by Terasic, in which the USB-Blaster circuit is installed. And, Terasic USB Blaster Download Cable.  
https://pgate1.at-ninja.jp/memo/VirtualJTAG_USB-Blaster/

Get FTDI D2XX driver.  
https://ftdichip.com/drivers/d2xx-drivers/
<!-- https://www.ftdichip.com/Drivers/D2XX.htm -->

![UART via USB-Blaster](https://pgate1.at-ninja.jp/memo/VirtualJTAG_USB-Blaster/usbjtag_e.png)

The result of running the sample.  
```
DE0 result
size 32768 byte

send sum 0x4B
send 51ms 627.5 kB/s

recv sum 0x4B
recv 83ms 385.5 kB/s


DE0-CV result
size 1048576 byte

send sum 0xE0
send 1679ms 609.9 kB/s

recv sum 0xE0
recv 2636ms 388.5 kB/s
```
