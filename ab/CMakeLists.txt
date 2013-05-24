include_directories(${XML2_HEADERS})
include_directories(${LUA_HEADERS})
#include_directories(${INCLUDEDIR}/libxml2)

SET(SOURCES 
	manager.cpp node.cpp object.cpp xmlreader.cpp xmlwriter.cpp factory.cpp connection.cpp lua.cpp pluginloader.cpp builtin_nodes.cpp
	actions/luaaction.cpp  actions/webservice.cpp actions/wait.cpp actions/stop.cpp actions/load.cpp
	events/luaevent.cpp events/timeout.cpp events/alarm.cpp )

add_library(ab_static STATIC ${SOURCES})

#add_library(ab SHARED ${SOURCES})
target_link_libraries(ab_static ${LUA_LIB} z xml2 curl)

