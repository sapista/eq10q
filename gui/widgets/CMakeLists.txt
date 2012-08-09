link_directories(${GTKMM_LIBRARY_DIRS} ${SLV2_LIBRARY_DIRS} )

include_directories(${GTKMM_INCLUDE_DIRS} ${SLV2_INCLUDE_DIRS})


#ADD_EXECUTABLE(eqwin.o 
#		eqbutton.cpp ctlbutton.cpp pixmapcombo.cpp 
#		bandctl.cpp gainctl.cpp eqparams.cpp faderwidget.cpp 
#		vuwidget.cpp eqwindow.cpp ../../dsp/filter.c)


add_library(eqwin  STATIC
		eqbutton.cpp ctlbutton.cpp pixmapcombo.cpp 
		bandctl.cpp gainctl.cpp eqparams.cpp faderwidget.cpp 
		vuwidget.cpp eqwindow.cpp filter.cpp )


target_link_libraries( eqwin ${GTKMM_LIBRARIES}  ${SLV2_LIBRARIES})

