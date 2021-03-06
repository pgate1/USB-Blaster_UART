# USB-Blaster UART with VirtualJTAG

Let's communicate between PC user software and FPGA via Altera USB-Blaster.
The target FPGA boards are DE0, DE1, DE2, DE0-CV, etc. made by Terasic, in which the USB-Blaster circuit is installed.  
https://pgate1.at-ninja.jp/NES_on_FPGA/dev_other.htm#usbjtag

Get FTDI D2XX driver.  
https://www.ftdichip.com/Drivers/D2XX.htm

![UART via USB-Blaster](https://pgate1.at-ninja.jp/NES_on_FPGA/dev_other_usbjtag_e.png)

The result of running the sample.  
```
size 32768 byte

send sum 0x26
send 78ms 410.3kB/s

recv sum 0x26
recv 1061ms 30.2kB/s
```
