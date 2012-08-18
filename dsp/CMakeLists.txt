###DSP CMake

#denormals are handled using -ffast-math
# TODO enable this
#ADD_DEFINITIONS(-Wall -O3 -fPIC -DPIC -ffast-math)

add_library( dsp  STATIC filter.c smooth.c vu.c)
target_link_libraries( dsp )