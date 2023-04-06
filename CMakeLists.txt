cmake_minimum_required(VERSION 3.24)
project(threaded)

set(CMAKE_CXX_STANDARD 23)

add_executable(threaded main.cpp SecantMethod.cc SecantMethod.tcc NPlus.cc NPlus.tcc AdditionOverflowCheck.cc AdditionOverflowCheck.tcc AdditionUnderflowCheck.cc AdditionUnderflowCheck.tcc PositiveInfinityQ.cc PositiveInfinityQ.tcc NegativeInfinityQ.cc NegativeInfinityQ.tcc)
