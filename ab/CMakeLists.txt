add_executable(abrun main.cpp)
target_link_libraries(abrun ab_static ${XML2_LIB} ${Z_LIB} ${LUA_LIB} dl ${GCRYPT_LIB} pthread rt ${GNUTLS_LIB})

install(TARGETS abrun
	RUNTIME DESTINATION bin
	)
