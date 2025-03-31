@echo off

set clientDir=Bin\NetworkClient\
set serverDir=Bin\NetworkServer\
set clientExe=NetworkClient_Debug.exe
set serverExe=NetworkServer_Debug.exe

set client1cmd=WINPOSX=100 WINPOSY=0
set client2cmd=WINPOSX=600 WINPOSY=0
set client3cmd=WINPOSX=100 WINPOSY=512
set client4cmd=WINPOSX=600 WINPOSY=512
set servercmd=WINPOSX=1200 WINPOSY=256

pushd %clientDir%
start %clientExe% %client1cmd%
start %clientExe% %client2cmd%
start %clientExe% %client3cmd%
start %clientExe% %client4cmd%
popd

pushd %serverDir%
start %serverExe% %servercmd%
popd