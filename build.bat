rem Use this batch file to build bounce for Visual Studio
rmdir /s /q build
mkdir build
cd build
cmake ..
cmake --build .
start bounce.sln