cmake_minimum_required(VERSION 2.6)
project(ssf2skin)
enable_testing()
add_executable(ssf2skin main.cpp ssfextract.cpp convert.cpp)
include(FindPkgConfig)
file(COPY   "run_test.sh" DESTINATION ".")

pkg_check_modules(SSL openssl)
pkg_check_modules(QT5 Qt5Core)
pkg_check_modules(QTGUI Qt5Widgets)

include_directories(${QT5_INCLUDE_DIRS})
include_directories(${QTGUI_INCLUDE_DIRS})
target_link_libraries(ssf2skin ${SSL_LIBRARIES})
target_link_libraries(ssf2skin ${QT5_LIBRARIES})
target_link_libraries(ssf2skin ${QTGUI_LIBRARIES})
target_link_libraries(ssf2skin libcrypto.so)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

install(TARGETS ssf2skin RUNTIME DESTINATION bin)

add_test(NAME integrate COMMAND ./run_test.sh)
