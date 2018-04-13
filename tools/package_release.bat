@echo off

if not defined OPENMF_INSTALL_DIR (
	echo OPENMF_INSTALL_DIR has to be defined!
	exit
)

echo Packaging release build of OpenMF...

echo Purging previous releases...
pushd %OPENMF_INSTALL_DIR%
del * /f/q/S
popd

echo Copying binaries...
xcopy /e %OPENMF_DIR%\build\bin\Release\* %OPENMF_INSTALL_DIR%\

echo Copying OpenSceneGraph DLLs...
xcopy /e %OSG_DIR%\build\bin %OPENMF_INSTALL_DIR%\

echo Copying SDL2.dll ...
xcopy %SDL2_ROOT%\lib\x64\SDL2.dll %OPENMF_INSTALL_DIR%\

echo Copying zlib.dll ...
xcopy %OSG_THIRD_PARTY_DIR%\bin\zlib.dll %OPENMF_INSTALL_DIR%\

echo Purging extra files...
pushd %OPENMF_INSTALL_DIR%
del *.exp
del *.pdb
del *.lib
popd