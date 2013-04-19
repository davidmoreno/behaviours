cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)
project(behaviours)

if (${ARM})
	message(STATUS "Compiling for ARM")
	SET(EXTRALIBS pthread)
else (${ARM})
	SET(EXTRALIBS pthread)
  SET(INCLUDEDIR /usr/include)
endif (${ARM})

#SET(USE_ACAPELLA true)
SET(ONION_LIBS onioncpp onion_handlers_static onion_static)
SET(OPENCV_LIBS opencv_legacy opencv_ml opencv_core opencv_imgproc opencv_features2d opencv_calib3d opencv_highgui opencv_video opencv_flann opencv_objdetect)

#find_library(XML2_LIB NAMES xml2 PATH ${LIBPATH})
message("-- Search for XML2 headers")
find_path(XML2_HEADERS libxml/xmlwriter.h ${INCLUDEDIR}/libxml2 ${GCCPATH}/arm-none-linux-gnueabi/libc/usr/include/libxml2/  ${GCCPATH}/* ${GCCPATH}/arm-none-linux-gnueabi/include/libxml2 )
message("-- Search for LUA headers")
find_path(LUA_HEADERS lua.h ${GCCPATH}/arm-none-linux-gnueabi/libc/usr/include/lua5.2 ${INCLUDEDIR}/lua5.2 ${GCCPATH}/* ${GCCPATH}/arm-none-linux-gnueabi/include/ ${GCCPATH}/arm-unknown-linux-gnueabi/sysroot/usr/include)
message("-- Search for LUA lib")
include_directories(${LUA_HEADERS})
if (${ARM})
  SET(LUA_LIB lua)
else (${ARM})
  find_library(LUA_LIB NAMES lua5.2 lua liblua PATH ${LIBPATH})
endif (${ARM})
message("-- Search for boost::thread")
find_library(BOOST_THREAD NAMES boost_thread boost_thread-mt PATH ${LIBPATH})

if (${ARM})
  if (${BOOST_THREAD})
  else (${BOOST_THREAD})
    SET(BOOST_THREAD boost_thread)
    message("-- Using fixed boost::thread")
  endif (${BOOST_THREAD})
endif(${ARM})



##
if (${ARM})
  include_directories (${AISOY1API_BUILD}/../include/)
  include_directories (${AISOY1API_BUILD}/../src/real/)
endif(${ARM})

include_directories (${CMAKE_CURRENT_SOURCE_DIR})


IF (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_definitions(-D__DEBUG__)
	SET(CMAKE_CXX_FLAGS "-Wall -Werror -O2 -g -fPIC -std=c++0x")
	MESSAGE(STATUS "Using debug mode")
ELSE (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	SET(CMAKE_CXX_FLAGS "-Wall -Werror -O2 -fPIC -std=c++0x")
ENDIF (${CMAKE_BUILD_TYPE} MATCHES "Debug")

add_definitions(-DAB_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

find_library(LIBAISOY1 NAMES aisoy1 PATH /usr/lib /usr/local/lib )

add_subdirectory(ab)
add_subdirectory(plugins)
add_subdirectory(server)
add_subdirectory(test)
