
EVLKST8500GH FIRMWARE UPGRADE TOOL


*********************************************************************************************************************
IMPORTANT NOTE: it is MANDATORY to install STM32 Cube Programmer to upgrade the NUCLEO-G070RB board (from www.st.com)
*********************************************************************************************************************

The firmware upgrade tool is provided in the \FW upgrade folder.
The structure of this folder is the following:
•	\imgs folder with image files of PE and RTE protocol firmware and ST8500_SPI_Loader.img for flash programming
•	image_loader_st8500.exe executable file for ST8500 programming
•	ST8500_G3_Hybrid_PE_RTE_upload.bat with the full procedure for firmware download/upgrade
•	NUCLEO-programmer_Coordinator_868.bat for programming the NUCLEO-G070RB as G3-PLC PAN Coordinator (868 MHz)
•	NUCLEO-programmer_Coordinator_915.bat for programming the NUCLEO-G070RB as G3-PLC PAN Coordinator (915 MHz)
•	NUCLEO-programmer_Device_868.bat for programming the NUCLEO-G070RB as G3-PLC PAN Device (868 MHz)
•	NUCLEO-programmer_Device_915.bat for programming the NUCLEO-G070RB as G3-PLC PAN Device (915 MHz)


STM32 PROGRAMMING
After installing the STM32 Cube Programmer, connect the USB cable to the NUCLEO-G070RB USB connector and launch the Coordinator or Device .bat file.
There is a version for the 868 RF module and a version for the 915 RF module for both Coordinator and Device. 
Please select the proper image depending on the RF module you are using.
Please connect only one EVLKST8500GH at a time.

ST8500 PROGRAMMING
The correlated COM port can be found into Windows Device Manager tool (standard COM port) and is requested at the begin 
of ST8500_G3_Hybrid_PE_RTE_upload.bat execution. The target image file names, if different from the default, must be modified in this file.

NOTE: Before proceeding with the FW download, the position of STM32 MODE and PLC BOOT1 switches must be set according 
to indications shown into ST8500_G3_Hybrid_PE_RTE_upload.bat to operate on ST8500 with boot from ST8500 SPI flash: 
please ensure that STM32 MODE switch position is "1" (DOWN) and PLC BOOT1 is "1" (DOWN).

The main content of ST8500_G3_Hybrid_PE_RTE_upload.bat is shown below.
•	set DEFAULT_COMM=COM9
Set the default COM port (COM9 in this example)

•	set /p COM_id="Enter COM port number (e.g. 9) and press RETURN: "
Wait for current COM port number

•	IF [%COM_id%]==[] (
	set COMM=%DEFAULT_COMM%
   ) ELSE (
	set COMM=COM%COM_id%)
With these instructions, COM port number is default value if not indicated, otherwise it is the keyboard input value

•	.\image_loader_st8500.exe --port %COMM% --get_running_protocol_and_erase
This command detects the protocol currently running into ST8500 and executes the corresponding SPI FLASH erase command to prepare for programming

•	.\image_loader_st8500.exe --port %COMM% --hi_boot
This command uploads the ST8500_SPI_Loader.img file on ST8500 RAM

•	.\image_loader_st8500.exe --port %COMM% --pe imgs\<PE_image>.img --rte imgs\<RTE_image>.img
Write FW images into ST8500 SPI flash

•	.\image_loader_st8500.exe --port %COMM% --G3_erase_NVM
Erases the Non-Volatile-Memory inside the ST8500 SPI flash memory (sector 30 and 31)

The image files <PE_image>.img, <RTE_image>.img are provided in the package. 
Please note that the actual file names might be different with different release versions. 
Please refer to the README.txt in each release package.
