SET(SOURCES basic.cpp)

add_library(testbasic SHARED ${SOURCES})

install(TARGETS testbasic 
	LIBRARY DESTINATION lib/ab
	)
	
install(DIRECTORY static/nodes
	DESTINATION shared/ab/static/basictest
	)
