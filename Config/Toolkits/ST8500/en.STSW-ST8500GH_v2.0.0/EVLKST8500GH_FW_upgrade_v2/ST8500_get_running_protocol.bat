@echo off
set DEFAULT_COMM=COM7
echo.
echo.
echo ################################################################
echo Before proceeding with FW upload, please ensure that STM32 MODE  
echo switch position is "1" (DOWN) and PLC BOOT1 is "1" (DOWN)
echo ################################################################
echo.
echo ###############################
echo Press RESET on EVALKITST8500-1
echo ###############################
echo.
echo #############
echo Press RETURN
echo #############
pause>nul
echo.

set /p COM_id="Enter COM port number (e.g. 9) and press RETURN: "

IF [%COM_id%]==[] (
	set COMM=%DEFAULT_COMM%
) ELSE (
	set COMM=COM%COM_id%
)

echo.
echo Selected COM port is :  %COMM%


echo Current running protocol is: 
.\image_loader_st8500.exe --port %COMM% --get_running_protocol

echo.
pause

