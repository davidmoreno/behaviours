SET(SOURCES python3.cpp)

add_library(python3 SHARED ${SOURCES})
target_link_libraries(python3 python3.3m)


install(TARGETS python3
	LIBRARY DESTINATION lib/ab
	)
	
install(DIRECTORY static/nodes static/img
	DESTINATION shared/ab/python3
	)
