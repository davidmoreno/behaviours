include_directories(${XML2_HEADERS})

add_library(abserver_static STATIC utils.cpp nodemanager.cpp statichandler.cpp)
add_executable(server main.cpp)

target_link_libraries(server abserver_static onioncpp_static onion_handlers_static onion_static ab_static xml2 z ${LUA_LIB} dl gcrypt pthread rt boost_system boost_thread-mt gnutls)

install(TARGETS server 
	RUNTIME DESTINATION bin
	)
	
install(DIRECTORY static/js static/css static/doc static/img static/nodes
	DESTINATION shared/ab/00-base
	)
install(FILES static/favicon.ico static/index.html
	DESTINATION shared/ab/00-base
	)
