@echo off

set clientDir=Bin\NetworkClient\
set serverDir=Bin\NetworkServer\
set clientExe=NetworkClient_Release.exe
set serverExe=NetworkServer_Release.exe

set client1cmd=WINPOSX=250 WINPOSY=0
set client2cmd=WINPOSX=750 WINPOSY=0
set client3cmd=WINPOSX=250 WINPOSY=512
set client4cmd=WINPOSX=750 WINPOSY=512

pushd %clientDir%
start %clientExe% %client1cmd%
start %clientExe% %client2cmd%
start %clientExe% %client3cmd%
start %clientExe% %client4cmd%
popd

pushd %serverDir%
start %serverExe%
popd