cmake_minimum_required(VERSION 3.12)
project(lv2_gtk_ui_bridge C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Base compiler flags
add_compile_options(-Wall -Wextra -Wno-unused-value)

# Platform-specific flags & libraries
if(APPLE)
    add_compile_options(-Wno-deprecated-declarations)
elseif(WIN32)
    add_compile_options(-mstackrealign)
    add_link_options(-static)
else()
    # Linux / POSIX
    find_library(LIB_RT rt)
endif()

# Locate PkgConfig
find_package(PkgConfig REQUIRED)

# -----------------------------------------------------------------------------
# LV2 Dependency check (Vendored headers vs. system pkg-config)
# -----------------------------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lv2/include")
    add_library(lv2_headers INTERFACE)
    target_include_directories(lv2_headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lv2/include")
else()
    pkg_check_modules(LV2 REQUIRED lv2)
endif()

# Helper macro to bind LV2 headers/flags to a target
function(target_link_lv2 TARGET_NAME)
    if(TARGET lv2_headers)
        target_link_libraries(${TARGET_NAME} PRIVATE lv2_headers)
    else()
        target_include_directories(${TARGET_NAME} PRIVATE ${LV2_INCLUDE_DIRS})
        target_link_libraries(${TARGET_NAME} PRIVATE ${LV2_LIBRARIES})
    endif()
endfunction()

# -----------------------------------------------------------------------------
# Target 1: UI Server Module (eq10q-gtk-ui-bridge.so)
# -----------------------------------------------------------------------------
add_library(eq10q-gtk-ui-bridge MODULE src/ui-server.c)

# Remove 'lib' prefix and route binary directly to the .lv2 bundle
set_target_properties(eq10q-gtk-ui-bridge PROPERTIES 
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${LV2_BUNDLE_DIR}"
)

target_link_lv2(eq10q-gtk-ui-bridge)

if(LIB_RT)
    target_link_libraries(eq10q-gtk-ui-bridge PRIVATE ${LIB_RT})
endif()

# -----------------------------------------------------------------------------
# Target 2: GTK2 UI Client Executable (eq10q-gtk2-ui-bridge)
# -----------------------------------------------------------------------------
pkg_check_modules(GTK2 gtk+-2.0)
pkg_check_modules(LILV lilv-0)
pkg_check_modules(X11 x11)

if(GTK2_FOUND AND LILV_FOUND AND X11_FOUND)
    add_executable(eq10q-gtk2-ui-bridge src/ui-client.c)

    # Route executable directly to the .lv2 bundle so the server can exec it
    set_target_properties(eq10q-gtk2-ui-bridge PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${LV2_BUNDLE_DIR}"
    )

    target_compile_definitions(eq10q-gtk2-ui-bridge PRIVATE UI_GTK2)
    target_compile_options(eq10q-gtk2-ui-bridge PRIVATE -Wno-deprecated-declarations)

    target_include_directories(eq10q-gtk2-ui-bridge PRIVATE 
        ${GTK2_INCLUDE_DIRS} 
        ${LILV_INCLUDE_DIRS} 
        ${X11_INCLUDE_DIRS}
    )

    target_link_libraries(eq10q-gtk2-ui-bridge PRIVATE 
        ${GTK2_LIBRARIES} 
        ${LILV_LIBRARIES} 
        ${X11_LIBRARIES}
        ${CMAKE_DL_LIBS} # Handles -ldl
    )

    target_link_lv2(eq10q-gtk2-ui-bridge)

    if(LIB_RT)
        target_link_libraries(eq10q-gtk2-ui-bridge PRIVATE ${LIB_RT})
    endif()
endif()
