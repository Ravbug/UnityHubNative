mkdir -p build && cd build
cmake -G "Xcode" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release --target install 
