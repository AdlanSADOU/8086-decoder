
if not exist build (mkdir build)

pushd Build
@REM cl ../src/main.cpp /Zi
call cl ../src/main.cpp
popd