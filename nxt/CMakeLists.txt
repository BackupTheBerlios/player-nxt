cmake_minimum_required(VERSION 2.6)

PLAYERDRIVER_OPTION (nxt build_nxt ON)

find_file (HAVE_USB libusb.h /usr/include /usr/include/libusb-1.0)

if (NOT HAVE_USB)
    PLAYERDRIVER_OPTION (nxt build_nxt OFF "libusb-1.0 not found (package libusb-1.0-0-dev in debian)")
elseif (NOT HAVE_STL)
    PLAYERDRIVER_OPTION (nxt build_nxt OFF "STL not found")
else()
    PLAYERDRIVER_OPTION (nxt build_nxt ON)
endif()

PLAYERDRIVER_ADD_DRIVER(
    nxt build_nxt
    
    SOURCES
    src/chronos.cc
    src/nxt_driver.cc
    src/nxtdc.cc

    CFLAGS
    -Wall
    -g
    -I/usr/include/libusb-1.0

    LINKFLAGS
    -lusb-1.0
    )
