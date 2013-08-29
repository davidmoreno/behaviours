cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)
project(behaviours)

if (${ARM})
	message(STATUS "Compiling for ARM")
	
	SET(EXTRALIBS pthread)
	SET(INCLUDEDIR ${CMAKE_FIND_ROOT_PATH}/usr/include)
	SET(INCLUDEMULTIARCH ${CMAKE_FIND_ROOT_PATH}/usr/include/arm-linux-gnueabihf)
	SET(LIBPATH ${CMAKE_FIND_ROOT_PATH}/usr/lib)
	SET(CMAKE_INCLUDE_PATH ${CMAKE_FIND_ROOT_PATH}/usr/include)
	find_path(ONION_HEADERS onion/log.h ${INCLUDEDIR} ${CMAKE_FIND_ROOT_PATH}/usr/local/include)
	message(STATUS "ONION: ${ONION_HEADERS}")
else (${ARM})
	SET(EXTRALIBS pthread)
  SET(INCLUDEDIR /usr/include)
endif (${ARM})

#SET(USE_ACAPELLA true)
SET(ONION_LIBS onioncpp onion_handlers_static onion_static)
SET(OPENCV_LIBS opencv_legacy opencv_ml opencv_core opencv_imgproc opencv_features2d opencv_calib3d opencv_highgui opencv_video opencv_flann opencv_objdetect)
find_library(ONIONCPP_LIB NAMES onioncpp PATHS ${LIBPATH} ${LIBPATH}/arm-linux-gnueabihf ${CMAKE_FIND_ROOT_PATH}/lib ${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf)
find_library(ONIONHANDLERS_STATIC_LIB NAMES onion_handlers_static PATHS ${LIBPATH} ${LIBPATH}/arm-linux-gnueabihf ${CMAKE_FIND_ROOT_PATH}/lib ${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf)
find_library(ONION_STATIC_LIB NAMES onion_static PATHS ${LIBPATH} ${LIBPATH}/arm-linux-gnueabihf ${CMAKE_FIND_ROOT_PATH}/lib ${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf)
find_library(CURL_LIB NAMES curl PATHS ${LIBPATH} ${LIBPATH}/arm-linux-gnueabihf ${CMAKE_FIND_ROOT_PATH}/lib ${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf)
find_library(GNUTLS_LIB NAMES gnutls PATHS ${LIBPATH}  ${LIBPATH}/arm-linux-gnueabihf ${CMAKE_FIND_ROOT_PATH}/lib ${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf)
find_library(GCRYPT_LIB NAMES gcrypt PATHS ${LIBPATH} ${LIBPATH}/arm-linux-gnueabihf ${CMAKE_FIND_ROOT_PATH}/lib ${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf)
find_library(Z_LIB NAMES z PATHS ${LIBPATH} ${LIBPATH}/arm-linux-gnueabihf ${CMAKE_FIND_ROOT_PATH}/lib ${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf)
find_library(JSON_LIB NAMES json-c json PATHS ${LIBPATH} ${LIBPATH}/arm-linux-gnueabihf ${CMAKE_FIND_ROOT_PATH}/lib ${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf)
find_library(XML2_LIB NAMES xml2 PATHS ${LIBPATH} ${LIBPATH}/arm-linux-gnueabihf ${CMAKE_FIND_ROOT_PATH}/lib ${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf)
message("-- Search for XML2 headers")
find_path(XML2_HEADERS libxml/xmlwriter.h ${INCLUDEDIR}/libxml2)
message("-- Search for LUA headers")
find_path(LUA_HEADERS lua.h ${INCLUDEDIR}/lua5.2)
message("-- Search for LUA lib")
include_directories(${LUA_HEADERS})

if (${ARM})
	find_path(LUA_MULTIARCH_HEADERS lua5.2-deb-multiarch.h ${INCLUDEMULTIARCH})
	include_directories(${LUA_MULTIARCH_HEADERS})
endif (${ARM})

find_path(JSON_HEADERS json/json.h  ${INCLUDEDIR}/json)
include_directories(${JSON_HEADERS})
message(${CMAKE_MODULE_LINKER_FLAGS})
#if (${ARM})
#  SET(LUA_LIB lua)
#else (${ARM})
  find_library(LUA_LIB NAMES lua5.2 lua liblua PATHS ${LIBPATH} ${LIBPATH}/arm-linux-gnueabihf ${CMAKE_FIND_ROOT_PATH}/lib ${CMAKE_FIND_ROOT_PATH}/lib/arm-linux-gnueabihf)
#endif (${ARM})


##
if (${ARM})
  include_directories (${AISOY1API_BUILD}/../include/)
  include_directories (${AISOY1API_BUILD}/../src/real/)
endif(${ARM})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/lib)


IF (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_definitions(-D__DEBUG__)
	SET(CMAKE_CXX_FLAGS "-Wall -Werror -O0 -g -fPIC -std=c++0x")
	MESSAGE(STATUS "Using debug mode")
ELSE (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	SET(CMAKE_CXX_FLAGS "-Wall -Werror -O0 -fPIC -std=c++0x")
ENDIF (${CMAKE_BUILD_TYPE} MATCHES "Debug")

add_definitions(-DAB_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

find_library(LIBAISOY1 NAMES aisoy1 PATH /usr/lib /usr/local/lib )




add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(server)
add_subdirectory(ab)
# add_subdirectory(test)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
