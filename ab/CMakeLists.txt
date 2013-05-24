add_executable(ab main.cpp)
target_link_libraries(ab onioncpp_static onion_handlers_static 
	onion_static ab_static xml2 z ${LUA_LIB} dl gcrypt pthread rt gnutls
	boost_system boost_thread-mt)

install(TARGETS ab
	RUNTIME DESTINATION bin
	)
