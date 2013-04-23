add_executable(01-ab 01-ab.cpp)
target_link_libraries(01-ab ab_static ${LUA_LIB} boost_system boost_thread-mt dl xml2)

