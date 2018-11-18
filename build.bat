@echo off & setlocal

cd %~dp0

if exist "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsMSBuildCmd.bat" call "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2017\Enterprise\Common7\Tools\VsMSBuildCmd.bat"
msbuild /version >NUL 2>NUL
if errorlevel 0 goto:build

if exist "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2017\Enterprise\MSBuild\15.0\Bin\msbuild.exe" path %PROGRAMFILES(x86)%\Microsoft Visual Studio\2017\Enterprise\MSBuild\15.0\Bin;%PATH%
msbuild /version >NUL 2>NUL
if errorlevel 0 goto:build

echo Couldn't find any MSBuild to build this project.
echo Make sure you have Visual C++ Build Tools 2017 or Visual Studio 2017 installed.
endlocal
exit /B 1

:build
call generate

set PLATFORM=Win32
set CONFIGURATION=Release
msbuild /nologo /m /v:m %* build\stereogram-model-viewer.sln

set PLATFORM=x64
set CONFIGURATION=Release
msbuild /nologo /m /v:m %* build\stereogram-model-viewer.sln
endlocal
exit /B %ERRORLEVEL%
