cmake_minimum_required(VERSION 2.6)

PLAYERDRIVER_OPTION (differential build_differential ON)

if (NOT HAVE_STL)
    PLAYERDRIVER_OPTION (differential build_differential OFF "STL not found")
else()
    PLAYERDRIVER_OPTION (differential build_differential ON)
endif()

PLAYERDRIVER_ADD_DRIVER(
    differential build_differential
    
    SOURCES
    chronos.cc
    driver.cc

    CFLAGS
    -Wall
    -g
    )
