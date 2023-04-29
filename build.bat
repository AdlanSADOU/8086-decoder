
if not exist build (mkdir build)

pushd Build
@REM cl ../src/main.cpp /Zi /link /DEBUG /SUBSYSTEM:console
cl ../src/main.cpp
popd