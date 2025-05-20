@echo off

set clientDir=Bin\NetworkClient\
set serverDir=Bin\NetworkServer\
set clientExe=NetworkClient_Debug.exe
set serverExe=NetworkServer_Debug.exe

set clientcmd=FULLSCREEN=0 BORDERLESS=0 WINPOSX=100 WINPOSY=256 WINSIZEX=512 WINSIZEY=512
set servercmd=FULLSCREEN=0 BORDERLESS=0 WINPOSX=1200 WINPOSY=256 WINSIZEX=512 WINSIZEY=512

pushd %clientDir%
start %clientExe% %clientcmd%
popd

pushd %serverDir%
start %serverExe% %servercmd%
popd