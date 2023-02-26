cmake_minimum_required(VERSION 3.20)
project(pix_player_v3)

set(CMAKE_CXX_STANDARD 17)

link_directories(winmm)
add_executable(pix_player_v3 main.cpp)
target_link_libraries(pix_player_v3 winmm)

