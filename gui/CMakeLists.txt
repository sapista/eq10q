## GUI CMake

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4)

add_subdirectory(widgets) 

link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

# -----------------------------------------------------------------------------
# 1. GUI Tests (Builds in build directory, not bundle)
# -----------------------------------------------------------------------------
set(Eq_Gui_Test_Path "\"${PROJECT_SOURCE_DIR}/gui\"")
configure_file("${PROJECT_SOURCE_DIR}/gui/guitester.cpp" "${PROJECT_SOURCE_DIR}/gui/guitester_cfg_.cpp")
add_executable(eq10q_gui_tester main.cpp guitester_cfg_.cpp)
target_link_libraries(eq10q_gui_tester eqwin dynamicwin bassupwin midsidewin ${GTKMM_LIBRARIES})

# -----------------------------------------------------------------------------
# 2. Build EQ10Q GUI Libraries (Outputs directly to bin/EQ10QPlugins.lv2/gui/)
# -----------------------------------------------------------------------------
add_library(eq10q_gui SHARED eq10q_ui.cpp)
target_link_libraries(eq10q_gui eqwin ${GTKMM_LIBRARIES})

add_library(dynamics_gui SHARED dyn_ui.cpp)
target_link_libraries(dynamics_gui dynamicwin ${GTKMM_LIBRARIES})

add_library(bassup_gui SHARED bassup_ui.cpp)
target_link_libraries(bassup_gui bassupwin ${GTKMM_LIBRARIES})

add_library(matrix_ms_gui SHARED midside_ui.cpp)
target_link_libraries(matrix_ms_gui midsidewin ${GTKMM_LIBRARIES})

# Set properties for GUI targets
set(GUI_TARGETS eq10q_gui dynamics_gui bassup_gui matrix_ms_gui)

set_target_properties(${GUI_TARGETS} PROPERTIES 
    PREFIX "" 
    LIBRARY_OUTPUT_DIRECTORY "${LV2_BUNDLE_DIR}/gui"
)

# -----------------------------------------------------------------------------
# 3. Copy Asset Folders on 'make'
# -----------------------------------------------------------------------------
add_custom_target(copy_gui_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/knobs" "${LV2_BUNDLE_DIR}/gui/knobs"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/combopix" "${LV2_BUNDLE_DIR}/gui/combopix"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/icons" "${LV2_BUNDLE_DIR}/gui/icons"
    COMMENT "Copying GUI assets to ${LV2_BUNDLE_DIR}/gui..."
)
