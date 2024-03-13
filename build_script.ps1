#mkdir PackedGame

echo "Adding Unreal to path !"
$env:PATH += ";C:\Program Files\Epic Games\UE_5.1\Engine\Binaries\Win64"

echo "Adding Unreal to unreal automation tool  !"
$env:PATH += ";C:\Program Files\Epic Games\UE_5.1\Engine\Build\BatchFiles\"


RunUAT.bat BuildCookRun -project="C:\Gitlab-Runner\builds\RpajUN3z\0\Exploranation\City\digitaltwincity-plattform\digitaltwinnkpg-ue5\DigitalTwin.uproject" -noP4  -platform=Win64  -clientconfig=Development  -serverconfig=Development -cook -allmaps -build  -stage -pak -archive -archivedirectory="PackedGame" 
