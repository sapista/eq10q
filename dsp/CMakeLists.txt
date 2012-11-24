###DSP CMake

#denormals are handled using -ffast-math
ADD_DEFINITIONS(-Wall -O3 -fPIC -DPIC -msse -mfpmath=sse -ffast-math)

##Build Mono Filter lib
set( Num_Of_Channels  1)
configure_file("${PROJECT_SOURCE_DIR}/dsp/filter.h" "${PROJECT_SOURCE_DIR}/dsp/filter_mono.h")
configure_file("${PROJECT_SOURCE_DIR}/dsp/filter.c" "${PROJECT_SOURCE_DIR}/dsp/filter_mono.c")
add_library(lib_filter_mono STATIC filter_mono.c)

##Build Mono Filter lib
set( Num_Of_Channels  2)
configure_file("${PROJECT_SOURCE_DIR}/dsp/filter.h" "${PROJECT_SOURCE_DIR}/dsp/filter_stereo.h")
configure_file("${PROJECT_SOURCE_DIR}/dsp/filter.c" "${PROJECT_SOURCE_DIR}/dsp/filter_stereo.c")
add_library(lib_filter_stereo STATIC filter_stereo.c)

##Build the vumeter
add_library( lib_vumeter  STATIC vu.c)

##Build the smooth
add_library( lib_smooth  STATIC smooth.c)

##Build the db converter
add_library( lib_db  STATIC db.c)

#CLEAN generated .c files on run 'make clean'
SET(generatedDspConfigFiles 
      "${PROJECT_SOURCE_DIR}/dsp/filter_mono.h"
      "${PROJECT_SOURCE_DIR}/dsp/filter_mono.c"
      "${PROJECT_SOURCE_DIR}/dsp/filter_stereo.h"
      "${PROJECT_SOURCE_DIR}/dsp/filter_stereo.c"
      )
SET_DIRECTORY_PROPERTIES(PROPERTIES  ADDITIONAL_MAKE_CLEAN_FILES "${generatedDspConfigFiles}")
