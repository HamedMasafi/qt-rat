@echo off
set exe_server_64=..\..\build\rat\Desktop_Qt_5_14_0_MinGW_64_bit\Release\server\server.exe
set exe_client_64=D:\dev\Qt\rat\build\client\Qt_lite\Release\release\client.exe

copy %exe_client_64% output\builder\windows\base.dat /y
copy %exe_server_64% output /y