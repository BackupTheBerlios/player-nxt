PROJECT(libnxtdc)

ADD_EXECUTABLE(test_lib main.cpp)

ADD_LIBRARY(nxtdc error.c lowlevel.c)