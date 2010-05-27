cmake_minimum_required(VERSION 2.6)

find_file (LIBUSB libusb.h /usr/include/libusb-1.0)

if (NOT LIBUSB)
    PLAYERDRIVER_OPTION (nxt build_nxt OFF "libusb-1.0 not found (package libusb-1.0-0-dev in debian)")
elseif (NOT HAVE_STL)
    PLAYERDRIVER_OPTION (nxt build_nxt OFF "STL not found")
ELSE()
    PLAYERDRIVER_OPTION (nxt build_nxt ON)
ENDIF()

#PLAYERDRIVER_REQUIRE_OS(nxt build_nxt UNIX)

PLAYERDRIVER_ADD_DRIVER(
    nxt build_nxt
    
    SOURCES
    src/nxtdc.cc
    src/nxt_driver.cc

    CFLAGS
    -Wall
    -pedantic
    -g

    LINKFLAGS
    -lusb-1.0
    )
