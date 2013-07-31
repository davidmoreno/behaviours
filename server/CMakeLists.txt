include_directories(${XML2_HEADERS})

add_library(abserver_static STATIC utils.cpp nodemanager.cpp statichandler.cpp)
add_executable(abserver main.cpp)

target_link_libraries(abserver abserver_static onioncpp_static onion_handlers_static
	onion_static ab_static xml2 z ${LUA_LIB} dl gcrypt pthread rt gnutls)

install(TARGETS abserver 
	RUNTIME DESTINATION bin
	)
	
install(DIRECTORY static/js static/css static/doc static/img static/nodes
	DESTINATION share/ab/static/00-base
	)
install(FILES static/favicon.ico static/index.html
	DESTINATION share/ab/static/00-base
	)
