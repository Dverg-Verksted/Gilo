
if not exist Plugins\GameConfigModel (
	git clone https://github.com/YuriTrofimov/GameConfigModel.git Plugins\GameConfigModel
)
else (
	cd Plugins\GameConfigModel
	git pull
	cd ..\..\
)

clean.bat
build_development.bat
pause