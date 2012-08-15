cmake_minimum_required(VERSION 2.8)
PROJECT(gui)

#GCC options example TODO remove -g option in release version
ADD_DEFINITIONS(-Wall -fPIC -DPIC -g)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4)
#pkg_check_modules(SLV2 slv2)

#Set Install prefix TODO CAUTION!!! This have to be moved to the top Cmak directory this is just a test
set (CMAKE_INSTALL_PREFIX  "/usr/local/lib/lv2") #Install prefix
set (LV2_BUNDLE "sapistaEQ.lv2") #LV2 Bundel dir

#Prepare includes
set( Eq10q_gui_path "\"${CMAKE_INSTALL_PREFIX}/${LV2_BUNDLE}/gui/\"")
configure_file("${PROJECT_SOURCE_DIR}/widgets/PathConfig.h.in" "${PROJECT_SOURCE_DIR}/widgets/PathConfig.h")

add_subdirectory(widgets) 

link_directories(${GTKMM_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#Tests GUI TODO coment out for the final release
ADD_EXECUTABLE(eq10q_gui_tester  main.cpp guitester.cpp)
target_link_libraries( eq10q_gui_tester  eqwin ${GTKMM_LIBRARIES})

#===========================BUILD EQ10Q ALL GUI FILES=============================================
#Build EQ1QM GUI
set( Eq_Bands_Count 1)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq1qm/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/eq1qm_ui.cpp")
add_library(eq1qm_gui SHARED eq1qm_ui.cpp)
target_link_libraries( eq1qm_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ1QS GUI
set( Eq_Bands_Count 1)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq1qs/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/eq1qs_ui.cpp")
add_library(eq1qs_gui SHARED eq1qs_ui.cpp)
target_link_libraries( eq1qs_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ4QM GUI
set( Eq_Bands_Count 4)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq4qm/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/eq4qm_ui.cpp")
add_library(eq4qm_gui SHARED eq4qm_ui.cpp)
target_link_libraries( eq4qm_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ4QS GUI
set( Eq_Bands_Count 4)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq4qs/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/eq4qs_ui.cpp")
add_library(eq4qs_gui SHARED eq4qs_ui.cpp)
target_link_libraries( eq4qs_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ6QM GUI
set( Eq_Bands_Count 6)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq6qm/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/eq6qm_ui.cpp")
add_library(eq6qm_gui SHARED eq6qm_ui.cpp)
target_link_libraries( eq6qm_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ6QS GUI
set( Eq_Bands_Count 6)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq6qs/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/eq6qs_ui.cpp")
add_library(eq6qs_gui SHARED eq6qs_ui.cpp)
target_link_libraries( eq6qs_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ10QM GUI
set( Eq_Bands_Count 10)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq10qm/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/eq10qm_ui.cpp")
add_library(eq10qm_gui SHARED eq10qm_ui.cpp)
target_link_libraries( eq10qm_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ10QS GUI
set( Eq_Bands_Count 10)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq10qs/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/eq10qs_ui.cpp")
add_library(eq10qs_gui SHARED eq10qs_ui.cpp)
target_link_libraries( eq10qs_gui  eqwin ${GTKMM_LIBRARIES})
