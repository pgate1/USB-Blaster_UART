# USB-Blaster UART with VirtualJTAG

Let's communicate between PC user software and FPGA via Altera USB-Blaster.
The target FPGA boards are DE0, DE1, DE2-115, DE0-CV, etc. made by Terasic, in which the USB-Blaster circuit is installed. And, Terasic USB Blaster Download Cable.  
https://pgate1.at-ninja.jp/memo/VirtualJTAG_USB-Blaster/

Get FTDI D2XX driver.  
https://www.ftdichip.com/Drivers/D2XX.htm

![UART via USB-Blaster](https://pgate1.at-ninja.jp/memo/VirtualJTAG_USB-Blaster/usbjtag_e.png)

The result of running the sample.  
```
size 32768 byte

send sum 0x26
send 78ms 410.3 kB/s

recv sum 0x26
recv 1061ms 30.2 kB/s
```
