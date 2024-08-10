How to run the tests:
If you don't have google-test on your computer:
Run the following in the terminal:
1. vcpkg install gtest

Run the tests:
1. cmake -S . -B build (in case it doesn't work append: -G "MinGW Makefiles")
2. cmake --build build
3. cd build
4. ctest