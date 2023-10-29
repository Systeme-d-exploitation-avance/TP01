cmake_minimum_required(VERSION 3.22)
project(TP01 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(Exercice01
        Exercice01/EX01.c)

add_executable(Exercice02 
        Exercice02/EX02.c)

add_executable(Exercice03
        Exercice03/EX03.c)

add_executable(Exercice04
        Exercice04/EX04.c)