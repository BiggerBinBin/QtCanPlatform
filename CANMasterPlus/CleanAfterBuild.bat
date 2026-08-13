@echo off
echo del obj folder
echo "%1\%2\CANMasterPlus.tlog"
rmdir /s /q "%1\%2\CANMasterPlus.tlog"
echo del teampeorary
rem del /q "%3\*.log" "%3\*.ilk" "%3\*.obj" "%3\*.ipdb"
del /s /q "%1\%2\*.log" "%1\%2\*.obj"