CLS
echo Update project plugins...

@Echo Off
If Not Exist Plugins\GameConfigModel (
	git clone https://github.com/YuriTrofimov/GameConfigModel.git Plugins\GameConfigModel
) Else (
	cd Plugins\GameConfigModel
	git pull
	cd ..\..\
)

call clean.bat
call build_development.bat
pause