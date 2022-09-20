echo Delete project files...
echo off

rem Visual Studio

del /Q .vs 2>nul
rd /S /Q .vs 2>nul
del *.sln 2>nul

rem Visual Studio Code

del /Q .vscode 2>nul
rd /S /Q .vscode 2>nul
del *.code-workspace 2>nul

rem Unreal Engine

del /Q Binaries 2>nul
rd /S /Q Binaries 2>nul
rem Build - build settings
del /Q Build 2>nul
rd /S /Q Build 2>nul
rem Intermediate - temporary files generated when you compile your code
del /Q Intermediate 2>nul
rd /S /Q Intermediate 2>nul
del /Q DerivedDataCache 2>nul
rd /S /Q DerivedDataCache 2>nul
del /Q Script 2>nul
rd /S /Q Script 2>nul