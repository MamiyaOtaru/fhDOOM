mkdir build\msvc2022-x64
pushd build\msvc2022-x64
cmake -G "Visual Studio 17 2022" -A x64 %* ../..
popd
pause