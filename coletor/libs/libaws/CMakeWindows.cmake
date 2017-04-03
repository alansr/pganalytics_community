# Copyright Forrest Heller 2010 
# Released under the Apache 2 license

# This file gets included when you build libaws on Windows
# You need to provide binaries, headers, and libs for:
# OpenSSL
# Curl
# Libxml
# Pthread (hint: pthreadVCE)

IF(MSVC)
# Flags for Visual Studio
#string pooling 
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GF")
#Multi threaded debug dll lib (/MDd) and Runtime checks (/RTC1)
SET(CMAKE_CXX_FLAGS_DEBUG "/MDd /RTCsu /RTC1 ${CMAKE_CXX_FLAGS_DEBUG} ")
SET(ExceptionHandling TRUE)
ENDIF(MSVC)

######## Fill in these macros for library paths #############
# This does not explicitly include libraries and headers, but
# guides FIND_PACKAGE

SET(W32_INCLUDE "include")
SET(W32_LIB "lib")
GET_FILENAME_COMPONENT(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
GET_FILENAME_COMPONENT(W32_PATH "${CURRENT_DIR}/win32" ABSOLUTE)
SET(OPENSSL_INCLUDE_PATHS "${W32_PATH}/OpenSSL/${W32_INCLUDE}")
SET(OPENSSL_LIBRARY_PATHS "${W32_PATH}/OpenSSL/${W32_LIB}")
SET(LIBXML2_INCLUDE_PATHS "${W32_PATH}/libxml/${W32_INCLUDE}")
SET(LIBXML2_LIBRARY_PATHS "${W32_PATH}/libxml/${W32_LIB}")
SET(CURL_INCLUDE_PATHS "${W32_PATH}/curl/${W32_INCLUDE}")
SET(CURL_LIBRARY_PATHS "${W32_PATH}/curl/${W32_LIB}")
SET(STD_HEADERS_INCLUDE_PATH "${W32_PATH}/stdheaders/${W32_INCLUDE}")
SET(PTHREAD_INCLUDE_PATHS "${W32_PATH}/pthread/${W32_INCLUDE}")
SET(PTHREAD_LIB_PATHS "${W32_PATH}/pthread/${W32_LIB}")
#libcrypto is statically compiled with Windows OpenSSL libs
SET(LIBCRYPTO "")
SET(LIBPTHREADS "${PTHREAD_LIB_PATHS}/pthreadVCE.lib")

INCLUDE_DIRECTORIES(${PTHREAD_INCLUDE_PATHS} ${STD_HEADERS_INCLUDE_PATH} "${CURRENT_DIR}/src/win32")
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${OPENSSL_INCLUDE_PATHS} ${LIBXML2_INCLUDE_PATHS} ${CURL_INCLUDE_PATHS} ${PTHREAD_INCLUDE_PATHS} ${STD_HEADERS_INCLUDE_PATH})
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${OPENSSL_LIBRARY_PATHS} ${LIBXML2_LIBRARY_PATHS} ${CURL_LIBRARY_PATHS} ${PTHREAD_LIB_PATHS})
SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${CMAKE_INCLUDE_PATH})
