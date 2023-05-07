@echo off

set PATH=%QUARTUS_ROOTDIR%\bin64;%QUARTUS_ROOTDIR%\bin32;%QUARTUS_ROOTDIR%\bin;%PATH%

quartus_pgm.exe -c "USB-Blaster" -m JTAG -o p;DE0-CV_top.sof

echo Press any key to finish.
pause > nul

exit
