cmake_minimum_required(VERSION 2.6)
project(nfa_re)

add_executable(nfa_re main.cpp)

install(TARGETS nfa_re RUNTIME DESTINATION bin)
