@echo off
set DEFAULT_COMM=COM4
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
pause

set /p COM_id="Enter COM port number (e.g. 9) and press RETURN: "

IF [%COM_id%]==[] (
	set COMM=%DEFAULT_COMM%
) ELSE (
	set COMM=COM%COM_id%
)

echo.
echo Selected COM port is :  %COMM%


echo Current running protocol is: 
.\image_loader_st8500.exe --port %COMM% --get_running_protocol_and_erase

echo.
echo ###############################
echo Press RESET on EVALKITST8500-1
echo ###############################
echo.
pause

echo.
echo Upload on ST8500 RAM a boot software ongoing...
.\image_loader_st8500.exe --port %COMM% --hi_boot
pause

echo Write ST8500 FW images into SPI FLASH ongoing...
.\image_loader_st8500.exe --port %COMM% --pe imgs\ST8500_G3_v1560b595_6.1.16.img --rte imgs\STARCOM_G3_RT_FW_r1_9rc1_BA_key0.img
pause
echo.
echo ###############################
echo Press RESET on EVALKITST8500-1
echo ###############################
echo.
pause
echo Erase ST8500 G3 NVM inside SPI FLASH
.\image_loader_st8500.exe --port %COMM% --G3_erase_NVM
echo.
echo #######################
echo End of the programming
echo #######################
echo.
pause

