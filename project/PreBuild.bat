@echo off
rem change to current bat file dir
cd /d "%~dp0"
rem get txt version info
for /f "tokens=1,2,3 delims==." %%a in (version.txt) do (
	set verh=%%a
	set verm=%%b
)
echo TXT_VER:%verh%.%verm%

rem get version info from version.h
for /f "skip=2 tokens=3 delims== " %%h in (..\include\version.h) do (
	set fmwh=%%h
	goto _breakh
)
:_breakh
for /f "skip=3 tokens=3 delims== " %%m in (..\include\version.h) do (
	set fmwm=%%m
	goto _breakm
)
:_breakm
echo FMW_VER:%fmwh%.%fmwm%

set fmw_ver=%fmwh%.%fmwm%
set txt_ver=%verh%.%verm%

rem compare version
if "%fmw_ver%"=="%txt_ver%" (
rem version no change plus buildcounter 1
	for /f "delims==" %%i in (buildno.txt) do set /A build_counter= %%i+1
) else (
rem version change set buildcounter 1
	echo %fmw_ver%> version.txt
	set /A build_counter=1
)
rem write buildcounter back
echo Build No %build_counter%
echo %build_counter% > buildno.txt 
rem generate build counter cpp header
echo #define VERSION_BUILD %build_counter% > ../include/buildno.h
	

