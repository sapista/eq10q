cmake_minimum_required(VERSION 2.8)
PROJECT(gui)

#GCC options example
#ADD_DEFINITIONS(-g -O2 -fsigned-char -freg-struct-return -Wall -W -Wshadow -Wstrict-prototypes -Wpointer-arith -Wcast-qual -Winline -Werror)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4)
pkg_check_modules(SLV2 slv2)

add_subdirectory(widgets) 

link_directories(${GTKMM_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
ADD_EXECUTABLE(eq10q_gui_tester  main.cpp guitester.cpp)

#GUI Linker

target_link_libraries( eq10q_gui_tester eqwin ${GTKMM_LIBRARIES})