## EQ10Q TopLevel CMake
cmake_minimum_required(VERSION 3.12)
project(eq10q C CXX)

# -----------------------------------------------------------------------------
# 1. Compiler Flags
# -----------------------------------------------------------------------------
set(CMAKE_C_FLAGS "-Wall -O3 -fPIC -finline-functions -finline-functions-called-once -msse -mfpmath=sse -std=c99")
set(CMAKE_CXX_FLAGS "-Wall -fPIC -std=c++11")

# Debugging options (uncomment if needed):
# set(CMAKE_C_FLAGS "-Wall -O0 -g -fPIC -finline-functions -finline-functions-called-once -msse -mfpmath=sse -std=c99")
# set(CMAKE_CXX_FLAGS "-Wall -O0 -g -fPIC -DPIC -std=c++11")

# -----------------------------------------------------------------------------
# 2. Output Bundle Configuration (Builds directly into bin/sapistaEQv2.lv2)
# -----------------------------------------------------------------------------
set(LV2_BUNDLE "EQ10QPlugins.lv2")
set(LV2_BUNDLE_DIR "${PROJECT_SOURCE_DIR}/bin/${LV2_BUNDLE}")

# Route ALL target outputs (libraries & executables) into the .lv2 bundle directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LV2_BUNDLE_DIR}")

# -----------------------------------------------------------------------------
# 3. Subdirectories (Must be called AFTER setting output directories)
# -----------------------------------------------------------------------------
add_subdirectory(gui) 
add_subdirectory(dsp)
add_subdirectory(lv2-gtk-ui-bridge)

# -----------------------------------------------------------------------------
# 4. Copy static TTL Manifest files on every 'make' execution
# -----------------------------------------------------------------------------
file(GLOB TTL_FILES "${PROJECT_SOURCE_DIR}/*.ttl")

add_custom_target(copy_ttl ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${TTL_FILES} "${LV2_BUNDLE_DIR}"
    COMMENT "Copying TTL manifest files into ${LV2_BUNDLE_DIR}..."
)

# -----------------------------------------------------------------------------
# 5. Build EQ10Q DSP Plugins
# -----------------------------------------------------------------------------

# Helper list to keep track of all DSP targets
set(ALL_DSP_TARGETS "")

# --- EQ1QM ---
set(Eq_Bands_Count 1)
set(Eq_Channels_Count 1)
set(Eq_Uri "\"http://eq10q.sourceforge.net/eq/eq1qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${CMAKE_CURRENT_BINARY_DIR}/eq1qm.c")
add_library(eq1qm SHARED "${CMAKE_CURRENT_BINARY_DIR}/eq1qm.c")
target_link_libraries(eq1qm lib_filter lib_vumeter m fftw3)
list(APPEND ALL_DSP_TARGETS eq1qm)

# --- EQ1QS ---
set(Eq_Bands_Count 1)
set(Eq_Channels_Count 2)
set(Eq_Uri "\"http://eq10q.sourceforge.net/eq/eq1qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${CMAKE_CURRENT_BINARY_DIR}/eq1qs.c")
add_library(eq1qs SHARED "${CMAKE_CURRENT_BINARY_DIR}/eq1qs.c")
target_link_libraries(eq1qs lib_filter lib_vumeter m fftw3)
list(APPEND ALL_DSP_TARGETS eq1qs)

# --- EQ4QM ---
set(Eq_Bands_Count 4)
set(Eq_Channels_Count 1)
set(Eq_Uri "\"http://eq10q.sourceforge.net/eq/eq4qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${CMAKE_CURRENT_BINARY_DIR}/eq4qm.c")
add_library(eq4qm SHARED "${CMAKE_CURRENT_BINARY_DIR}/eq4qm.c")
target_link_libraries(eq4qm lib_filter lib_vumeter m fftw3)
list(APPEND ALL_DSP_TARGETS eq4qm)

# --- EQ4QS ---
set(Eq_Bands_Count 4)
set(Eq_Channels_Count 2)
set(Eq_Uri "\"http://eq10q.sourceforge.net/eq/eq4qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${CMAKE_CURRENT_BINARY_DIR}/eq4qs.c")
add_library(eq4qs SHARED "${CMAKE_CURRENT_BINARY_DIR}/eq4qs.c")
target_link_libraries(eq4qs lib_filter lib_vumeter m fftw3)
list(APPEND ALL_DSP_TARGETS eq4qs)

# --- EQ6QM ---
set(Eq_Bands_Count 6)
set(Eq_Channels_Count 1)
set(Eq_Uri "\"http://eq10q.sourceforge.net/eq/eq6qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${CMAKE_CURRENT_BINARY_DIR}/eq6qm.c")
add_library(eq6qm SHARED "${CMAKE_CURRENT_BINARY_DIR}/eq6qm.c")
target_link_libraries(eq6qm lib_filter lib_vumeter m fftw3)
list(APPEND ALL_DSP_TARGETS eq6qm)

# --- EQ6QS ---
set(Eq_Bands_Count 6)
set(Eq_Channels_Count 2)
set(Eq_Uri "\"http://eq10q.sourceforge.net/eq/eq6qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${CMAKE_CURRENT_BINARY_DIR}/eq6qs.c")
add_library(eq6qs SHARED "${CMAKE_CURRENT_BINARY_DIR}/eq6qs.c")
target_link_libraries(eq6qs lib_filter lib_vumeter m fftw3)
list(APPEND ALL_DSP_TARGETS eq6qs)

# --- EQ10QM ---
set(Eq_Bands_Count 10)
set(Eq_Channels_Count 1)
set(Eq_Uri "\"http://eq10q.sourceforge.net/eq/eq10qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${CMAKE_CURRENT_BINARY_DIR}/eq10qm.c")
add_library(eq10qm SHARED "${CMAKE_CURRENT_BINARY_DIR}/eq10qm.c")
target_link_libraries(eq10qm lib_filter lib_vumeter m fftw3)
list(APPEND ALL_DSP_TARGETS eq10qm)

# --- EQ10QS ---
set(Eq_Bands_Count 10)
set(Eq_Channels_Count 2)
set(Eq_Uri "\"http://eq10q.sourceforge.net/eq/eq10qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${CMAKE_CURRENT_BINARY_DIR}/eq10qs.c")
add_library(eq10qs SHARED "${CMAKE_CURRENT_BINARY_DIR}/eq10qs.c")
target_link_libraries(eq10qs lib_filter lib_vumeter m fftw3)
list(APPEND ALL_DSP_TARGETS eq10qs)

# --- GATE MONO ---
set(Plugin_Is_Dynamics_Compressor "PLUGIN_IS_GATE")
set(Dyn_Uri "\"http://eq10q.sourceforge.net/gate\"")
set(Dyn_Channels_Count 1)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${CMAKE_CURRENT_BINARY_DIR}/gate_mono.c")
add_library(gate SHARED "${CMAKE_CURRENT_BINARY_DIR}/gate_mono.c")
target_link_libraries(gate lib_vumeter lib_filter m)
list(APPEND ALL_DSP_TARGETS gate)

# --- COMPRESSOR MONO ---
set(Plugin_Is_Dynamics_Compressor "PLUGIN_IS_COMPRESSOR")
set(Dyn_Uri "\"http://eq10q.sourceforge.net/compressor\"")
set(Dyn_Channels_Count 1)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${CMAKE_CURRENT_BINARY_DIR}/compressor_mono.c")
add_library(compressor SHARED "${CMAKE_CURRENT_BINARY_DIR}/compressor_mono.c")
target_link_libraries(compressor lib_vumeter lib_filter m)
list(APPEND ALL_DSP_TARGETS compressor)

# --- COMPRESSOR SIDE-CHAIN MONO ---
set(Plugin_Is_Dynamics_Compressor "PLUGIN_IS_COMPRESSOR_WITH_SC")
set(Dyn_Uri "\"http://eq10q.sourceforge.net/compressor_sc\"")
set(Dyn_Channels_Count 1)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${CMAKE_CURRENT_BINARY_DIR}/compressor_mono_sc.c")
add_library(compressor_sc SHARED "${CMAKE_CURRENT_BINARY_DIR}/compressor_mono_sc.c")
target_link_libraries(compressor_sc lib_vumeter lib_filter m)
list(APPEND ALL_DSP_TARGETS compressor_sc)

# --- GATE STEREO ---
set(Plugin_Is_Dynamics_Compressor "PLUGIN_IS_GATE")
set(Dyn_Uri "\"http://eq10q.sourceforge.net/gate_stereo\"")
set(Dyn_Channels_Count 2)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${CMAKE_CURRENT_BINARY_DIR}/gate_stereo.c")
add_library(gate_stereo SHARED "${CMAKE_CURRENT_BINARY_DIR}/gate_stereo.c")
target_link_libraries(gate_stereo lib_vumeter lib_filter m)
list(APPEND ALL_DSP_TARGETS gate_stereo)

# --- COMPRESSOR STEREO ---
set(Plugin_Is_Dynamics_Compressor "PLUGIN_IS_COMPRESSOR")
set(Dyn_Uri "\"http://eq10q.sourceforge.net/compressor_stereo\"")
set(Dyn_Channels_Count 2)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${CMAKE_CURRENT_BINARY_DIR}/compressor_stereo.c")
add_library(compressor_stereo SHARED "${CMAKE_CURRENT_BINARY_DIR}/compressor_stereo.c")
target_link_libraries(compressor_stereo lib_vumeter lib_filter m)
list(APPEND ALL_DSP_TARGETS compressor_stereo)

# --- COMPRESSOR SIDE-CHAIN STEREO ---
set(Plugin_Is_Dynamics_Compressor "PLUGIN_IS_COMPRESSOR_WITH_SC")
set(Dyn_Uri "\"http://eq10q.sourceforge.net/compressor_stereo_sc\"")
set(Dyn_Channels_Count 2)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${CMAKE_CURRENT_BINARY_DIR}/compressor_stereo_sc.c")
add_library(compressor_stereo_sc SHARED "${CMAKE_CURRENT_BINARY_DIR}/compressor_stereo_sc.c")
target_link_libraries(compressor_stereo_sc lib_vumeter lib_filter m)
list(APPEND ALL_DSP_TARGETS compressor_stereo_sc)

# --- BASSUP ---
add_library(bassup SHARED bassup.c)
target_link_libraries(bassup lib_filter m)
list(APPEND ALL_DSP_TARGETS bassup)

# --- LR2MS STEREO ---
set(Matrix_Is_LR2MS 1)
set(Matrix_Uri "\"http://eq10q.sourceforge.net/matrix_lr2ms\"")
configure_file("${PROJECT_SOURCE_DIR}/midside_matrix.c" "${CMAKE_CURRENT_BINARY_DIR}/matrix_lr2ms.c")
add_library(matrix_lr2ms SHARED "${CMAKE_CURRENT_BINARY_DIR}/matrix_lr2ms.c")
target_link_libraries(matrix_lr2ms lib_vumeter lib_filter m)
list(APPEND ALL_DSP_TARGETS matrix_lr2ms)

# --- MS2LR STEREO ---
set(Matrix_Is_LR2MS 0)
set(Matrix_Uri "\"http://eq10q.sourceforge.net/matrix_ms2lr\"")
configure_file("${PROJECT_SOURCE_DIR}/midside_matrix.c" "${CMAKE_CURRENT_BINARY_DIR}/matrix_ms2lr.c")
add_library(matrix_ms2lr SHARED "${CMAKE_CURRENT_BINARY_DIR}/matrix_ms2lr.c")
target_link_libraries(matrix_ms2lr lib_vumeter lib_filter m)
list(APPEND ALL_DSP_TARGETS matrix_ms2lr)


# Target properties for all DSP plugins (Outputs directly to bundle root)
set_target_properties(${ALL_DSP_TARGETS} PROPERTIES 
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${LV2_BUNDLE_DIR}"
)
