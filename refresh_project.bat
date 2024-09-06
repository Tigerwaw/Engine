mkdir Bin\
for /d %%I in (Bin\*) do xcopy Dependencies\dll\* %%~fsI /Y

mkdir Lib\
xcopy /s /Y Dependencies\lib\** Lib\

mkdir Lib\Debug\AssetManager
mkdir Lib\Release\AssetManager
mkdir Lib\Retail\AssetManager

mkdir Lib\Debug\GameEngine
mkdir Lib\Release\GameEngine
mkdir Lib\Retail\GameEngine

mkdir Lib\Debug\GraphicsEngine
mkdir Lib\Release\GraphicsEngine
mkdir Lib\Retail\GraphicsEngine

mkdir Lib\Debug\ImGui
mkdir Lib\Release\ImGui
mkdir Lib\Retail\ImGui

mkdir Lib\Debug\Logger
mkdir Lib\Release\Logger
mkdir Lib\Retail\Logger

cd Premake\
call premake5.exe vs2022
pause
