include_directories(${XML2_HEADERS})

add_library(abserver_static STATIC nodemanager.cpp)
add_executable(server main.cpp nodemanager.cpp)

target_link_libraries(server abserver_static onioncpp_static onion_handlers_static onion_static ab_static xml2 z ${LUA_LIB} dl gcrypt pthread rt boost_system boost_thread-mt gnutls)
