##GUI CMake

#PROJECT(gui)

#GCC options example TODO remove -g option in release version
# TODO  segur k no e de fer un remove_definitions()???
##ADD_DEFINITIONS(-Wall -fPIC -DPIC -g)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4)

add_subdirectory(widgets) 

link_directories(${GTKMM_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS})

#Tests GUI
set( Eq_Gui_Test_Path "\"${PROJECT_SOURCE_DIR}/gui\"" )
configure_file("${PROJECT_SOURCE_DIR}/gui/guitester.cpp" "${PROJECT_SOURCE_DIR}/gui/guitester_cfg_.cpp")
ADD_EXECUTABLE(eq10q_gui_tester  main.cpp guitester_cfg_.cpp)
target_link_libraries( eq10q_gui_tester  eqwin dynamicwin bassupwin ${GTKMM_LIBRARIES})

#===========================BUILD EQ10Q ALL GUI FILES=============================================

#Build EQ1QM GUI
set( Eq_Bands_Count 1)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq1qm/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/gui/eq1qm_ui.cpp")
add_library(eq1qm_gui SHARED eq1qm_ui.cpp)
target_link_libraries( eq1qm_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ1QS GUI
set( Eq_Bands_Count 1)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq1qs/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/gui/eq1qs_ui.cpp")
add_library(eq1qs_gui SHARED eq1qs_ui.cpp)
target_link_libraries( eq1qs_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ4QM GUI
set( Eq_Bands_Count 4)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq4qm/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/gui/eq4qm_ui.cpp")
add_library(eq4qm_gui SHARED eq4qm_ui.cpp)
target_link_libraries( eq4qm_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ4QS GUI
set( Eq_Bands_Count 4)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq4qs/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/gui/eq4qs_ui.cpp")
add_library(eq4qs_gui SHARED eq4qs_ui.cpp)
target_link_libraries( eq4qs_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ6QM GUI
set( Eq_Bands_Count 6)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq6qm/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/gui/eq6qm_ui.cpp")
add_library(eq6qm_gui SHARED eq6qm_ui.cpp)
target_link_libraries( eq6qm_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ6QS GUI
set( Eq_Bands_Count 6)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq6qs/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/gui/eq6qs_ui.cpp")
add_library(eq6qs_gui SHARED eq6qs_ui.cpp)
target_link_libraries( eq6qs_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ10QM GUI
set( Eq_Bands_Count 10)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq10qm/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/gui/eq10qm_ui.cpp")
add_library(eq10qm_gui SHARED eq10qm_ui.cpp)
target_link_libraries( eq10qm_gui  eqwin ${GTKMM_LIBRARIES})

#Build EQ10QS GUI
set( Eq_Bands_Count 10)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq10qs/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/eq10q_ui.cpp" "${PROJECT_SOURCE_DIR}/gui/eq10qs_ui.cpp")
add_library(eq10qs_gui SHARED eq10qs_ui.cpp)
target_link_libraries( eq10qs_gui  eqwin ${GTKMM_LIBRARIES})

#Build GATE GUI
set( bIsCompressor "false")
set( Dyn_Title "\"~GT10Q~\\r\\nNoise Gate\"")
set( Gui_Dyn_Uri  "\"http://eq10q.sourceforge.net/gate/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/dyn_ui.cpp" "${PROJECT_SOURCE_DIR}/gui/gate_ui.cpp")
add_library(gate_gui SHARED gate_ui.cpp)
target_link_libraries( gate_gui  dynamicwin ${GTKMM_LIBRARIES})

#Build Compressor GUI
set( bIsCompressor "true")
set( Dyn_Title "\"~CS10Q~\\r\\nCompressor\"")
set( Gui_Dyn_Uri  "\"http://eq10q.sourceforge.net/compressor/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/dyn_ui.cpp" "${PROJECT_SOURCE_DIR}/gui/compressor_ui.cpp")
add_library(compressor_gui SHARED compressor_ui.cpp)
target_link_libraries( compressor_gui  dynamicwin ${GTKMM_LIBRARIES})

#Build BassUp Gui_Dyn_Uri
add_library(bassup_gui SHARED bassup_ui.cpp)
target_link_libraries( bassup_gui  bassupwin ${GTKMM_LIBRARIES})

#remove the lib prefix to all generated .so files
set_target_properties(eq1qm_gui eq1qs_gui eq4qm_gui eq4qs_gui eq6qm_gui eq6qs_gui eq10qm_gui eq10qs_gui gate_gui compressor_gui bassup_gui PROPERTIES PREFIX "")

##CLEAN generated .cpp files on run 'make clean'
SET(generatedGuiConfigFiles
      ${PROJECT_SOURCE_DIR}/gui/eq1qm_ui.cpp
      ${PROJECT_SOURCE_DIR}/gui/eq1qs_ui.cpp
      ${PROJECT_SOURCE_DIR}/gui/eq4qm_ui.cpp
      ${PROJECT_SOURCE_DIR}/gui/eq4qs_ui.cpp
      ${PROJECT_SOURCE_DIR}/gui/eq6qm_ui.cpp
      ${PROJECT_SOURCE_DIR}/gui/eq6qs_ui.cpp
      ${PROJECT_SOURCE_DIR}/gui/eq10qm_ui.cpp
      ${PROJECT_SOURCE_DIR}/gui/eq10qs_ui.cpp
      ${PROJECT_SOURCE_DIR}/gui/gate_ui.cpp
      ${PROJECT_SOURCE_DIR}/gui/compressor_ui.cpp
      )
SET_DIRECTORY_PROPERTIES(PROPERTIES  ADDITIONAL_MAKE_CLEAN_FILES "${generatedGuiConfigFiles}")

##=================================INSTALLER===================================================================================
INSTALL( TARGETS eq1qm_gui eq1qs_gui eq4qm_gui eq4qs_gui eq6qm_gui eq6qs_gui eq10qm_gui eq10qs_gui gate_gui compressor_gui bassup_gui DESTINATION "${LV2_BUNDLE}/gui")
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/gui/knobs" DESTINATION "${LV2_BUNDLE}/gui")
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/gui/combopix" DESTINATION "${LV2_BUNDLE}/gui")
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/gui/icons" DESTINATION "${LV2_BUNDLE}/gui")

