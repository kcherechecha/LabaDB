cmake_minimum_required(VERSION 3.20)
project(LabaDB C)

set(CMAKE_C_STANDARD 99)

add_executable(LabaDB main.c Master.h Ind.h Slave.h Input.h Output.h Validation.h Movie.h Actor.h)
