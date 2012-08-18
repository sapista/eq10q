##Widgets CMake

link_directories(${GTKMM_LIBRARY_DIRS}) # ${SLV2_LIBRARY_DIRS} ) Removed SLV2 dependency

include_directories(${GTKMM_INCLUDE_DIRS}) # ${SLV2_INCLUDE_DIRS}) Removed SLV2 dependency

add_library(eqwin  STATIC
		eqbutton.cpp ctlbutton.cpp pixmapcombo.cpp 
		bandctl.cpp gainctl.cpp eqparams.cpp faderwidget.cpp 
		vuwidget.cpp eqwindow.cpp filter.cpp setwidgetcolors.cpp )


target_link_libraries( eqwin ${GTKMM_LIBRARIES}) #  ${SLV2_LIBRARIES}) Removed SLV2 dependency

