add_executable(ab main.cpp)
target_link_libraries(ab ab_static xml2 z ${LUA_LIB} dl gcrypt pthread rt gnutls)

install(TARGETS ab
	RUNTIME DESTINATION bin
	)
