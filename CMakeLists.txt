cmake_minimum_required(VERSION 3.24)
project(Ace)

set(CMAKE_CXX_STANDARD 14)

add_executable(Ace main.cpp WindowManager.cpp Config.hpp)
target_link_libraries(Ace X11)
