cmake_minimum_required(VERSION 2.6)
project(russianaicup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -fno-optimize-sibling-calls -fno-strict-aliasing -Wall -Wno-unused-function -Wno-unknown-pragmas -pedantic -Wextra")

# if(CMAKE_SYSTEM MATCHES "Linux.*")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
# endif()

add_library(russianaicup
    csimplesocket/HTTPActiveSocket.cpp
    csimplesocket/ActiveSocket.cpp
    csimplesocket/PassiveSocket.cpp
    csimplesocket/SimpleSocket.cpp
    MyStrategy.cpp
    RemoteProcessClient.cpp
    Runner.cpp
    Strategy.cpp
)

add_executable(Runner DefaultRunner.cpp)
add_executable(Solution main.cpp)

target_link_libraries(Runner russianaicup)
target_link_libraries(Solution russianaicup)
