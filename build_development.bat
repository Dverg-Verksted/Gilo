@echo off

set ProjectName=HorrorGame

FOR /F "tokens=2* skip=2" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\5.0" /v "InstalledDirectory"') do set EnginePath=%%b

if not exist "%EnginePath%" (
	@echo "UE5.0 Is not installed. Unable to proceed"
	pause
	exit
	)

set VersionSelector=%EnginePath%
set VersionSelector=%VersionSelector:\UE_5.0=%\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe
set UBTPath=%EnginePath%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe
set ProjectPath=%~dp0%ProjectName%.uproject

@echo on
echo Generate project files...
"%VersionSelector%" -switchversionsilent "%ProjectPath%" "%EnginePath%"
"%UBTPath%" -projectfiles -progress -project="%ProjectPath%"

echo Building project...
"%UBTPath%" %ProjectName%Editor Win64 Development "%ProjectPath%" 

pause