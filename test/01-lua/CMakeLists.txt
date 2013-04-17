SET(LIBS ab_static z xml2 ${LUA_LIB} boost_thread-mt dl)

add_executable(01-simple 01-simple.cpp)
target_link_libraries(01-simple ${LIBS} -shared-libgcc)

add_executable(02-luanode 02-luanode.cpp)
target_link_libraries(02-luanode ${LIBS})

add_executable(03-nodemethods 03-nodemethods.cpp)
target_link_libraries(03-nodemethods ${LIBS})

# # add_executable(04-luacv 04-luacv.cpp)
# # target_link_libraries(04-luacv ${LIBS} ${OPENCV_LIBS})
# 
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lena.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
