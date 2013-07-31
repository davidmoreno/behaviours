SET(SOURCES python2.cpp ab_module.cpp)

find_library(PYTHON2_LIB NAMES python2.7 PATH ${LIBPATH})
find_path(PYTHON2_HEADER Python.h ${CMAKE_INCLUDE_PATH} /usr/include/python2.7)

add_library(python2 SHARED ${SOURCES})
target_link_libraries(python2 ${PYTHON2_LIB})

include_directories(${PYTHON2_HEADER})
link_directories(${PYTHON2_LIB})

install(TARGETS python2
	LIBRARY DESTINATION lib/ab
	)
	
install(DIRECTORY static/nodes static/img static/js
	DESTINATION share/ab/static/python2
	)

install(FILES __init__.py
	DESTINATION share/ab/python2
	)
