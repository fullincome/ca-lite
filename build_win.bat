if "%CONFIGURATION%" == "Release" (
    mkdir release release\deploy
    cd release
    qmake.exe ..\ca-lite.pro -spec win32-g++ "CONFIG+=release"
    mingw32-make.exe
    cd ..

    copy .\release\ca-lite.exe .\release\deploy\ca-lite.exe
    windeployqt.exe --release .\release\deploy\ca-lite.exe
    copy "%QT_MINGW_DIR%\bin\libwinpthread-1.dll" .\release\deploy\
    copy /y "%QT_MINGW_DIR%\bin\libstdc++-6.dll" .\release\deploy\
    7z a %APPVEYOR_REPO_TAG_NAME%_win32.zip .\release\deploy\*
)

if "%CONFIGURATION%" == "StaticRelease" (
    7z x -y -r qtbase-static.zip -o%QT_STATIC_DIR%
    set "PATH=%QT_STATIC_DIR%\bin;%PATH%"
    mkdir static_release static_release\deploy
    cd static_release
    qmake.exe ..\ca-lite.pro -spec win32-msvc "CONFIG+=release" "CONFIG+=static"
    nmake release
    cd ..

    copy .\static_release\release\ca-lite.exe .\static_release\deploy\ca-lite.exe
    7z a %APPVEYOR_REPO_TAG_NAME%-standalone_win32.zip .\static_release\deploy\ca-lite.exe
)

if "%CONFIGURATION%" == "Debug" (
    mkdir debug debug\deploy
    cd debug
    qmake.exe ..\ca-lite.pro -spec win32-g++ "CONFIG+=debug" "CONFIG+=qml_debug"
    mingw32-make.exe
    cd ..

    copy .\debug\ca-lite.exe .\debug\deploy\ca-lite.exe
    windeployqt.exe .\debug\deploy\ca-lite.exe
    copy "%QT_MINGW_DIR%\bin\libwinpthread-1.dll" .\debug\deploy\
    :: windeployqt update wrong libstdc++-6.dll, copy it manually
    copy /y "%QT_MINGW_DIR%\bin\libstdc++-6.dll" .\debug\deploy\
    7z a %APPVEYOR_REPO_TAG_NAME%-debug_win32.zip .\debug\deploy\*
)
