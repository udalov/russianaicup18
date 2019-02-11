russianaicup2018
================

My solution of Russian AI Cup 2018 -- CodeBall (http://2018.russianaicup.ru/)

Results:
* Round 1 ([v1](a011e70)) -- 45th place, 92.9% wins
* Round 2 ([v2](8f3cfd6)) -- 30th place, 90.4% wins
* Finals ([v4](882693e)) -- 53rd place, 16.7% wins
* Sandbox overall -- 64th place

To compile/run, install [CMake](https://cmake.org) and run:

    mkdir out
    cd out
    cmake ..
    make -j 4
    cd ..
    out/Solution

Optionally, pass duration and seed to `out/Solution`:

    out/Solution 18000 42

Or change parameters in [src/main.cpp](src/main.cpp).

To see the custom visualization:
1. Change `#define VIS 0` to `#define VIS 1` in [src/MyStrategy.cpp](src/MyStrategy.cpp)
2. Compile and run a game
1. Run `./gradlew :run` in the `vis` directory
