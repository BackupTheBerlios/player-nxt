PROJECT(libnxtdc)

ADD_EXECUTABLE(test_lib error.c lowlevel.c main.cpp)
TARGET_LINK_LIBRARIES(test_lib usb)