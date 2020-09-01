set qt32=
set qt64=

xcopy D:\dev\Qt\build\rat\Desktop_Qt_5_14_0_MinGW_32_bit\Release\server\server.exe packages\windows.x86\packages\data /Y
xcopy D:\dev\Qt\build\rat\Desktop_Qt_5_14_0_MinGW_64_bit\Release\server\server.exe packages\windows.x86-64\packages\data /Y

"C:\Program Files\7-Zip\7z.exe" a -mx9 -tzip client_32.zip D:\dev\Qt\build\rat\Desktop_Qt_5_14_0_MinGW_32_bit\Release\client\client.exe
"C:\Program Files\7-Zip\7z.exe" a -mx9 -tzip client_64.zip D:\dev\Qt\build\rat\Desktop_Qt_5_14_0_MinGW_32_bit\Release\client\client.exe

xcopy client_32.zip windows\x86\files\windows\x86\client_exe.zip /Y
xcopy client_32.zip windows\x86-64\files\windows\x86\client_exe.zip /Y

xcopy client_64.zip windows\x86\files\windows\x86-64\client_exe.zip /Y
xcopy client_64.zip windows\x86-64\files\windows\x86-64\client_exe.zip /Y

del client_32.zip
del client_64.zip

:copy_client
	"C:\Program Files\7-Zip\7z.exe" a -mx9 -tzip client_32.zip D:\dev\Qt\build\rat\Desktop_Qt_5_14_0_MinGW_%~2_bit\Release\client\client.exe
	xcopy client_%~1.zip windows\%~1\files\windows\%~1\client_exe.zip /Y
	xcopy client_%~1.zip windows\%~1\files\windows\%~1\client_exe.zip /Y
	del client_%~1.zip
exit /b 0

:copy_clientmaker
	xcopy ..\tools\ClientMaker\ClientMaker\bin\Release\ClientMaker.exe packages\windows.%~1\data\builder\windows /Y
	xcopy ..\tools\ClientMaker\ClientMaker\bin\Release\*.dll packages\windows.%~1\data\builder\windows /Y
	xcopy ..\tools\ClientMaker\Client\bin\Release\Client.exe packages\windows.%~1\data\builder\windows /Y
EXIT /B 0

call :copy_client x86, 32
call :copy_client x86-64, 64

call :copy_clientmaker x86
call :copy_clientmaker x86-64