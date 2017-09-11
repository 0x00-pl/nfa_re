cmake_minimum_required(VERSION 2.6)
project(nfa_re)

#install(TARGETS nfa_re RUNTIME DESTINATION bin)

SET(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall -Wextra")

add_subdirectory(src)
add_executable(nfa_re 
test.cpp
src/regex_64.cpp
src/graph.cpp
)