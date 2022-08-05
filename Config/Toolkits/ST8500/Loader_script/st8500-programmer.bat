@echo off

set DEFAULT_COMM=COM27
set DISK="D:\"
set PE=ST8500_G3_v15814451_6.1.19.img
set RTE=STARCOM_G3_RT_FW_r1_9rc1_BA_key0.img
set COMMAND=.\image_loader_st8500.exe


set /p COM_id="Enter COM port number: "

IF [%COM_id%]==[] (
	set COMM=%DEFAULT_COMM%
) ELSE (
	set COMM=COM%COM_id%
)

echo Selected COM port is :  %COMM%

REM set /p DISK_id="Enter NUCLEO-G070RB disk drive letter: "

REM IF [%DISK_id%]==[] (
REM 	echo Loading STM32 host application on disk:  %DISK%
REM 	xcopy /s imgs\%CORTEX%  %DISK%
REM ) ELSE (
REM 	echo Loading STM32 host application on disk:  %DISK_id%:\
REM 	xcopy /s imgs\%CORTEX%  %DISK_id%:\
REM )
	
%COMMAND% --port %COMM% --G3_erase
pause> nul | set /p "=Press RESET (black) push button on the EVALKITST8500-1 board then press ENTER to continue"
%COMMAND% --port %COMM% --get_running_protocol
%COMMAND% --port %COMM% --hi_boot
echo "Downloading PE Image: " %PE%
echo "Downloading RTE Image: " %RTE%
pause> nul | set /p "=Press ENTER to continue with images download"
%COMMAND% --port %COMM% --pe imgs\%PE% --rte imgs\%RTE%

echo "FW download done"
pause> nul | set /p "=Press RESET (black) push button on the NUCLEO board to start the ST8500 module, then press ENTER to close the window"