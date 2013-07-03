SET(SOURCES botones.cpp)

add_library(botones SHARED ${SOURCES})

install(TARGETS botones 
	LIBRARY DESTINATION lib/ab
	)
	
install(DIRECTORY static/nodes
	DESTINATION shared/ab/static/botones
	)
install(DIRECTORY static/nodes static/js
	DESTINATION shared/ab/static/botones
	)
