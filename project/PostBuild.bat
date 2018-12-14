@echo off
rem change to current bat file dir
cd /d "%~dp0"
rem process build number count
for /f "delims==" %%i in (buildno.txt) do set /A build_counter= %%i
for /f "delims==" %%v in (version.txt) do set ver=%%v
set source=.\Release\exe\DP_NVMe.bin
echo Version:%ver% Build:%build_counter%
if %build_counter% lss 10 (
set target=..\output\DP_NVMe_%ver%.0%build_counter%.bin
)
if %build_counter% geq 10 (
set target=..\output\DP_NVMe_%ver%.%build_counter%.bin
)
echo %source% to %target%
copy %source% %target%
rem move %target%  E:\Output\ProgMaster\
