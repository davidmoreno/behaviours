# ${Z_LIB} ${XML2_LIB} ${GCRYPT_LIB}
include_directories(${XML2_HEADERS} ${ONION_HEADERS})

add_library(abserver_static STATIC utils.cpp nodemanager.cpp statichandler.cpp browsefiles.cpp)
add_executable(abserver main.cpp)

target_link_libraries(abserver abserver_static ${ONIONCPP_LIB} ${ONIONHANDLERS_STATIC_LIB} 
	${ONION_STATIC_LIB} ab_static ${XML2_LIB}  ${Z_LIB} ${LUA_LIB} dl ${GCRYPT_LIB} pthread rt ${GNUTLS_LIB})

install(TARGETS abserver 
	RUNTIME DESTINATION bin
	)
	
install(DIRECTORY static/js static/css static/doc static/img static/nodes
	DESTINATION share/ab/static/00-base
	)
install(FILES static/favicon.ico static/index.html
	DESTINATION share/ab/static/00-base
	)
