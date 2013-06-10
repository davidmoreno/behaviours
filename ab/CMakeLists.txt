add_executable(abrun main.cpp)
target_link_libraries(abrun ab_static xml2 z ${LUA_LIB} dl gcrypt pthread rt gnutls)

install(TARGETS abrun
	RUNTIME DESTINATION bin
	)
