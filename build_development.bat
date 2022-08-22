@echo off

REM Settings
set ProjectName=HorrorGame

FOR /F "tokens=2* skip=2" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\5.0" /v "InstalledDirectory"') do set EnginePath=%%b

if not exist "%EnginePath%" (
	@echo "UE5.0 Is not installed. Unable to proceed"
	pause
	exit
	)

set VersionSelector=%EnginePath%\.\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe
set UBTPath=%EnginePath%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe
set ProjectPath=%~dp0%ProjectName%.uproject

REM Generate project files
@echo on
"%VersionSelector%" -switchversionsilent "%ProjectPath%" "%EnginePath%"
"%UBTPath%" -projectfiles -progress -project="%ProjectPath%"

REM Build project
"%UBTPath%" %ProjectName%Editor Win64 Development "%ProjectPath%" 

pause