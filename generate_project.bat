mkdir Bin\
for /d %%I in (Bin\*) do xcopy Dependencies\dll\* %%~fsI /Y

mkdir Lib\
xcopy /s /Y Dependencies\lib\** Lib\
cd Premake\
call premake5.exe vs2022
pause
