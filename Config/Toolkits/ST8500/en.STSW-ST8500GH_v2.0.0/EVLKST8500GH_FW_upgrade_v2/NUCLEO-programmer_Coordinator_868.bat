@echo off

set CORTEX=NUCLEO-G070RB_coord_868.hex

PATH C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin

STM32_Programmer_CLI.exe -c port=SWD -w imgs\%CORTEX%

pause> nul | set /p "=Press ENTER to continue"
