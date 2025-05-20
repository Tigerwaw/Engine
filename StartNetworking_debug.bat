@echo off

set clientDir=Bin\NetworkClient\
set serverDir=Bin\NetworkServer\
set clientExe=NetworkClient_Debug.exe
set serverExe=NetworkServer_Debug.exe

set client1cmd=FULLSCREEN=0 BORDERLESS=0 WINPOSX=100 WINPOSY=0 WINSIZEX=512 WINSIZEY=512
set client2cmd=FULLSCREEN=0 BORDERLESS=0 WINPOSX=600 WINPOSY=0 WINSIZEX=512 WINSIZEY=512
set client3cmd=FULLSCREEN=0 BORDERLESS=0 WINPOSX=100 WINPOSY=512 WINSIZEX=512 WINSIZEY=512
set client4cmd=FULLSCREEN=0 BORDERLESS=0 WINPOSX=600 WINPOSY=512 WINSIZEX=512 WINSIZEY=512
set servercmd=FULLSCREEN=0 BORDERLESS=0 WINPOSX=1200 WINPOSY=256 WINSIZEX=512 WINSIZEY=512

pushd %clientDir%
start %clientExe% %client1cmd%
start %clientExe% %client2cmd%
start %clientExe% %client3cmd%
start %clientExe% %client4cmd%
popd

pushd %serverDir%
start %serverExe% %servercmd%
popd