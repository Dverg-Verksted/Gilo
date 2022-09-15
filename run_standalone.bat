@echo off CLS
REM Settings
set ProjectName=HorrorGame

FOR /F "tokens=2* skip=2" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\5.0" /v "InstalledDirectory"') do set EnginePath=%%b

if not exist "%EnginePath%" (
	@echo "UE5.0 Is not installed. Unable to proceed"
	pause
	exit
	)

set EditorPath=%EnginePath%\Engine\Binaries\Win64\UnrealEditor.exe
set ProjectPath=%~dp0%ProjectName%.uproject

start "" "%EditorPath%" "%ProjectPath%" -game -log -WINDOWED -WinX=0 -WinY=60 -ResX=1024 -ResY=576