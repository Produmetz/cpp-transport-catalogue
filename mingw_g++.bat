mkdir build
cd build
cmake -G "MinGW Makefiles" -D CMAKE_CXX_COMPILER=g++ ..
cmake --build .
