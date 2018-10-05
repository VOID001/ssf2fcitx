cmake_minimum_required(VERSION 2.6)
project(ssf2skin)
add_executable(ssf2skin main.cpp ssfextract.cpp convert.cpp)
include(FindPkgConfig)

pkg_check_modules(SSL openssl)
pkg_check_modules(QT5 Qt5Core)

include_directories(${QT5_INCLUDE_DIRS})
target_link_libraries(ssf2skin ${SSL_LIBRARIES})
target_link_libraries(ssf2skin ${QT5_LIBRARIES})
target_link_libraries(ssf2skin libcrypto.so)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
