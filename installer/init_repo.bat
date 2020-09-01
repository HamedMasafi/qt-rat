@echo off
setlocal

set exe_server_32=..\..\build\rat\Desktop_Qt_5_14_0_MinGW_32_bit\Release\server\server.exe
set exe_client_32=D:\dev\Qt\rat\build\client\Qt_lite\Release\client.exe
set exe_server_64=..\..\build\rat\Desktop_Qt_5_14_0_MinGW_64_bit\Release\server\server.exe
set exe_client_64=D:\dev\Qt\rat\build\client\Qt_lite\Release\client.exe
rem ..\..\build\rat\Qt_lite\Release\client\client.exe

set binarycreator=c:\Qt\Tools\QtInstallerFramework\3.2\bin\binarycreator.exe
set zip="C:\Program Files\7-Zip\7z.exe"

if %1==32 (
	set qt_root=c:\Qt\5.14.0\mingw73_32\
	set platform=windows-x86
	set server_exe=%exe_server_32%
	echo Building for 32 bit installer
) else if %1==64 (
	set qt_root=c:\Qt\5.14.0\mingw73_64\
	set platform=windows-x64
	set server_exe=%exe_server_64%
	echo Building for 64 bit installer
) else (
	echo Select 32 of 64 bit
	exit /b %errorlevel%
)

call :check_files
call :make_paths
call :copy_qt_files
call :copy_server
rem call :copy_client_maker
call :copy_client_exe
call :cleanup
call :make_installer

exit /b 0

:check_files
    call :check_file_exists %exe_client_32%
    call :check_file_exists %exe_client_64%
    call :check_file_exists %exe_server_32%
    call :check_file_exists %exe_server_64%
    call :check_file_exists %zip%
    call :check_file_exists %binarycreator%
exit /b

:check_file_exists
    if not exist "%~1" (
        echo The file '%~1' does not exists
        cmd /c exit -1073741510
    )
exit /b

:make_paths
	mkdir %platform%
	
	xcopy /v /e /c /y template_packages %platform%\
exit /b 0

:copy_qt_files
	set p=%platform%\ir.tooska.rat.qt\data
	
	mkdir %p%
	mkdir %p%\platforms
	mkdir %p%\styles
	
	rem xcopy %qt_root%\bin\libgcc_s_dw2-1.dll %p% /y /q
	xcopy %qt_root%\bin\libgcc_s_seh-1.dll %p% /y /q
	xcopy %qt_root%\bin\libstdc++-6.dll %p% /y /q
	xcopy %qt_root%\bin\libwinpthread-1.dll %p% /y /q
	xcopy %qt_root%\bin\Qt5Core.dll %p% /y /q
	xcopy %qt_root%\bin\Qt5Gui.dll %p% /y /q
	xcopy %qt_root%\bin\Qt5Network.dll %p% /y /q
	xcopy %qt_root%\bin\Qt5Svg.dll %p% /y /q
	xcopy %qt_root%\bin\Qt5Widgets.dll %p% /y /q
	
	xcopy %qt_root%\plugins\platforms\qwindows.dll %p%\platforms /y /q
	xcopy %qt_root%\plugins\styles\qwindowsvistastyle.dll %p%\styles /y /q
exit /b 0

:copy_server
	set p=%platform%\ir.tooska.rat.server\data
	mkdir %p%

	xcopy %server_exe% %p% /Y
exit /b 0

:copy_client_exe
	set p=%platform%\ir.tooska.rat.clientmaker.windows\data
	set pc=%p%\builder\windows
	mkdir %p%
	mkdir %pc%
    
	
	echo %pc%\base.dat
	copy %exe_client_64% %pc%\base.dat /y
exit /b 0

:copy_client_maker
    set p=%platform%\ir.tooska.rat.clientmaker\data
	set pc=%p%\builder\windows
	set p32=%p%\files\windows.x86
	set p64=%p%\files\windows.x86-64
	set pzip=%p%\files\windows.zip
	
    mkdir %p%
	mkdir %pc%
    mkdir %p%\files
	
	rem call :zip_file %p32%.zip %exe_client_32%
	rem call :zip_file %p64%.zip %exe_client_64%
	call :zip_file %pzip% %exe_client_64%
    
	xcopy ..\tools\ClientMaker\ClientMaker\bin\Release\ClientMaker.exe %pc% /y /q
	xcopy ..\tools\ClientMaker\ClientMaker\bin\Release\*.dll %pc% /y /q
	xcopy ..\tools\ClientMaker\Client\bin\Release\Client.exe %pc% /y /q
exit /b 0

:make_installer
	echo Creating installer
	%binarycreator% -c config.xml -p %platform% rat-installer-%platform%.exe
	rem call :zip_file rat-installer-%platform%.7z rat-installer-%platform%.exe
	"C:\Program Files\7-Zip\7z.exe" a -mx9 -t7z rat-installer-%platform%.7z rat-installer-%platform%.exe
exit /b 0

:zip_file
	del %~1
	"C:\Program Files\7-Zip\7z.exe" a -mx9 -tzip %~1 %~2
exit /b

:cleanup
	cd %platform%
	del /q /f /s ".gitkeep"
	cd ..	
exit /b