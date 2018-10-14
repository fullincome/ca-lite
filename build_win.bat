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
    7z a %APPVEYOR_REPO_TAG_NAME%-release_win32.zip .\release\deploy\*
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
    copy /y "%QT_MINGW_DIR%\bin\libstdc++-6.dll" .\debug\deploy\
    7z a %APPVEYOR_REPO_TAG_NAME%-debug_win32.zip .\debug\deploy\*
)
