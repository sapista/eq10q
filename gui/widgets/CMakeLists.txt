##Widgets CMake

link_directories(${GTKMM_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS})

add_library(eqwin  STATIC
		eqbutton.cpp ctlbutton.cpp pixmapcombo.cpp 
		bandctl.cpp gainctl.cpp eqparams.cpp faderwidget.cpp 
		vuwidget.cpp eqwindow.cpp filter.cpp setwidgetcolors.cpp 
		bodeplot.cpp)

target_link_libraries( eqwin ${GTKMM_LIBRARIES})

add_library(dynamicwin  STATIC vuwidget.cpp  knob.cpp dynamicswindow.cpp setwidgetcolors.cpp)
target_link_libraries( dynamicwin ${GTKMM_LIBRARIES})