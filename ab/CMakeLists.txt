include_directories(${XML2_HEADERS})
include_directories(${LUA_HEADERS})
#include_directories(${INCLUDEDIR}/libxml2)

SET(SOURCES manager.cpp node.cpp object.cpp xmlreader.cpp xmlwriter.cpp factory.cpp connection.cpp lua.cpp actions/luaaction.cpp events/luaevent.cpp events/timeout.cpp)

add_library(ab_static STATIC ${SOURCES})

#add_library(ab SHARED ${SOURCES})
#target_link_libraries(ab ${LUA_LIB} ${BOOST_THREAD} z xml2)

