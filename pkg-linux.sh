mkdir -p build && mkdir -p build/linux && cd build/linux
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ../..
cmake --build . --config release --target install
