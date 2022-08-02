@echo on
REM Set path variable to destination directory
set  my_destinationDir=C:\ProgramData\National Instruments\TestStand 2021 (64-bit)\Cfg
echo %my_destinationDir%
ren "%my_destinationDir%\Users.ini" "Users_Original.ini"
xcopy /y ".\Environment\Common\Cfg\Users.ini" "%my_destinationDir%"
ren "%my_destinationDir%\Templates.ini" "Templates_Original.ini"
xcopy /y ".\Environment\Common\Cfg\Templates.ini" "%my_destinationDir%"