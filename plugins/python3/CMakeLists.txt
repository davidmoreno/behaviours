SET(SOURCES python3.cpp ab_module.cpp)

find_library(PYTHON3_LIB NAMES python3.3m python3.2mu PATH ${LIBPATH})
find_path(PYTHON3_HEADER Python.h ${CMAKE_INCLUDE_PATH} /usr/include/python3.3m  /usr/include/python3.2mu)

add_library(python3 SHARED ${SOURCES})
target_link_libraries(python3 ${PYTHON3_LIB})

include_directories(${PYTHON3_HEADER})
link_directories(${PYTHON3_LIB})

install(TARGETS python3
	LIBRARY DESTINATION lib/ab
	)
	
install(DIRECTORY static/nodes static/img static/js
	DESTINATION shared/ab/static/python3
	)

install(FILES __init__.py
	DESTINATION shared/ab/python3
	)
