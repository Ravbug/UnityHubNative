@echo off

mkdir build
cd build
mkdir win
cd win
cmake -DCMAKE_BUILD_TYPE=Release ..\..
cmake --build . --config release --target install