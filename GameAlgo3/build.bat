mkdir build64 & pushd build64
cmake .. -G "Visual Studio 14 2015 Win64" -DCMAKE_CXX_FLAGS=/D_UNICODE
popd
cmake --build build64 --config Release
cd build64/Release
shlomizabari.exe