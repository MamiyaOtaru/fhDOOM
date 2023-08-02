mkdir build\msvc2022-x86
pushd build\msvc2022-x86
cmake -G "Visual Studio 17 2022" -A Win32 %* ../..
popd
pause