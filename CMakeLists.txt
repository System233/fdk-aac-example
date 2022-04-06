
cmake_minimum_required(VERSION 3.12)
project(fdk-aac-example)

add_subdirectory(libs/fdk-aac)
add_executable(aac-enc src/aac-enc.c src/wavreader.c)
target_link_libraries(aac-enc fdk-aac)

add_executable(aac-dec src/aac-dec.c src/wavreader.c)
target_link_libraries(aac-dec fdk-aac)