cmake_minimum_required(VERSION 3.2)
project(russianaicup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -DLOCAL -fno-optimize-sibling-calls -fno-strict-aliasing -Wall -Wextra -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -Wno-unknown-pragmas -pedantic")

# if(CMAKE_SYSTEM MATCHES "Linux.*")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
# endif()

set(MY_SOURCES
    Const.cpp
    Move.cpp
    MyStrategy.cpp
    QuickStartGuy.cpp
    Simulation.cpp
    Solution.cpp
    State.cpp
    Team.cpp
    Vis.cpp
    Vec.cpp
)

set(MY_HEADERS
    Const.h
    Move.h
    MyStrategy.h
    QuickStartGuy.h
    Scenario.h
    Simulation.h
    Solution.h
    State.h
    Team.h
    Vec.h
    Vis.h
)

add_library(russianaicup
    csimplesocket/HTTPActiveSocket.cpp
    csimplesocket/ActiveSocket.cpp
    csimplesocket/PassiveSocket.cpp
    csimplesocket/SimpleSocket.cpp
    RemoteProcessClient.cpp
    Runner.cpp
    Strategy.cpp
    ${MY_SOURCES}
)

add_executable(Runner DefaultRunner.cpp)
add_executable(Solution main.cpp)

target_link_libraries(Runner russianaicup)
target_link_libraries(Solution russianaicup)

add_custom_target(dist COMMAND
    ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_CURRENT_BINARY_DIR}/solution.zip" --format=zip -- ${MY_SOURCES} ${MY_HEADERS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
