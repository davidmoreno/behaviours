SET(SOURCES derecha.cpp ab_module.cpp)

find_library(PYTHON2_LIB NAMES python2.7 PATH ${LIBPATH})
find_path(PYTHON2_HEADER Python.h ${CMAKE_INCLUDE_PATH} /usr/include/python2.7)

add_library(Interfaz SHARED ${SOURCES})
target_link_libraries(Interfaz ${PYTHON2_LIB})

include_directories(${PYTHON2_HEADER})
link_directories(${PYTHON2_LIB})

install(TARGETS Interfaz
	LIBRARY DESTINATION lib/ab
	)
	
install(DIRECTORY static/nodes static/img static/js
	DESTINATION shared/ab/static/Interfaz
	)

install(FILES __init__.py
	DESTINATION shared/ab/Interfaz
	)
