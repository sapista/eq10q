##EQ10Q TopLevel CMake
cmake_minimum_required(VERSION 2.8)
PROJECT(eq10q)

##ADD_DEFINITIONS(-Wall -O3 -fPIC -finline-functions -finline-functions-called-once  -msse -mfpmath=sse -std=c99)
set(CMAKE_C_FLAGS "-Wall -O3 -fPIC -finline-functions -finline-functions-called-once  -msse -mfpmath=sse -std=c99")
set(CMAKE_CXX_FLAGS "-Wall -fPIC")

#ADD_DEFINITIONS(-Wall -O0 -g -fPIC -DPIC) ###USED FOR DEBUG

#Set Install prefix
#set (CMAKE_INSTALL_PREFIX  "/usr/local/lib/lv2") #Install prefix
set (CMAKE_INSTALL_PREFIX  "$ENV{HOME}/.lv2") #Install prefix
set (LV2_BUNDLE "sapistaEQv2.lv2") #LV2 Bundel dir
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory(gui) 
add_subdirectory(dsp)

#===========================BUILD EQ10Q ALL DSP FILES=============================================

#Build EQ1QM DSP
set( Eq_Bands_Count 1)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq1qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq1qm.c")
add_library(eq1qm SHARED eq1qm.c)
target_link_libraries( eq1qm  lib_filter lib_vumeter lib_smooth)

#Build EQ1QS DSP
set( Eq_Bands_Count 1)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq1qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq1qs.c")
add_library(eq1qs SHARED eq1qs.c)
target_link_libraries( eq1qs  lib_filter lib_vumeter lib_smooth)

#Build EQ4QM DSP
set( Eq_Bands_Count 4)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq4qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq4qm.c")
add_library(eq4qm SHARED eq4qm.c)
target_link_libraries( eq4qm  lib_filter lib_vumeter lib_smooth)

#Build EQ4QS DSP
set( Eq_Bands_Count 4)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq4qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq4qs.c")
add_library(eq4qs SHARED eq4qs.c)
target_link_libraries( eq4qs  lib_filter lib_vumeter lib_smooth)

#Build EQ6QM DSP
set( Eq_Bands_Count 6)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq6qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq6qm.c")
add_library(eq6qm SHARED eq6qm.c)
target_link_libraries( eq6qm  lib_filter lib_vumeter lib_smooth)

#Build EQ6QS DSP
set( Eq_Bands_Count 6)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq6qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq6qs.c")
add_library(eq6qs SHARED eq6qs.c)
target_link_libraries( eq6qs  lib_filter lib_vumeter lib_smooth)

#Build EQ10QM DSP
set( Eq_Bands_Count 10)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq10qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq10qm.c")
add_library(eq10qm SHARED eq10qm.c)
target_link_libraries( eq10qm  lib_filter lib_vumeter lib_smooth)

#Build EQ10QS DSP
set( Eq_Bands_Count 10)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq10qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq10qs.c")
add_library(eq10qs SHARED eq10qs.c)
target_link_libraries( eq10qs  lib_filter lib_vumeter lib_smooth)

#Build GATE DSP
add_library(gate SHARED gate.c)
target_link_libraries( gate)

#remove the lib prefix to all generated .so files
set_target_properties(eq1qm eq1qs eq4qm eq4qs eq6qm eq6qs eq10qm eq10qs gate PROPERTIES PREFIX "")

#CLEAN generated .c files on run 'make clean'
SET(generatedEqConfigFiles 
      ${PROJECT_SOURCE_DIR}/eq1qm.c;${PROJECT_SOURCE_DIR}/eq1qs.c
      ${PROJECT_SOURCE_DIR}/eq4qm.c;${PROJECT_SOURCE_DIR}/eq4qs.c
      ${PROJECT_SOURCE_DIR}/eq6qm.c;${PROJECT_SOURCE_DIR}/eq6qs.c
      ${PROJECT_SOURCE_DIR}/eq10qm.c;${PROJECT_SOURCE_DIR}/eq10qs.c
      )
SET_DIRECTORY_PROPERTIES(PROPERTIES  ADDITIONAL_MAKE_CLEAN_FILES "${generatedEqConfigFiles}")


##===========================THE LV2 INSTALLER================================================
#INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/${LV2_BUNDLE}" DESTINATION "${LV2_BUNDLE}")
INSTALL( FILES "manifest.ttl" "eq1qm.ttl" "eq1qs.ttl" "eq4qm.ttl" "eq4qs.ttl" "eq6qm.ttl" "eq6qs.ttl" "eq10qm.ttl" "eq10qs.ttl" "gate.ttl" DESTINATION "${LV2_BUNDLE}")
INSTALL( TARGETS eq1qm eq1qs eq4qm eq4qs eq6qm eq6qs eq10qm eq10qs gate DESTINATION "${LV2_BUNDLE}")

INCLUDE(CPack)
# cpack -G TGZ
 