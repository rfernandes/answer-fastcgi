cmake_minimum_required(VERSION 2.6)

project(fastcgipp)

add_definitions(
-std=c++11
)

find_package(Boost COMPONENTS system thread)

include_directories(BEFORE
  ./include
  ${CMAKE_CURRENT_BINARY_DIR}
)
include_directories(SYSTEM
	${Boost_INCLUDE_DIRS}
)


include(TestBigEndian)
test_big_endian(BIGENDIAN)
set(PACKAGE_NAME fastcgi++)
set(PACKAGE_VERSION 3.0)
configure_file(include/fastcgi++/config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")


add_library(fastcgipp SHARED
  src/http.cpp
  src/protocol.cpp
  src/request.cpp
  src/manager.cpp
  src/transceiver.cpp
  src/fcgistream.cpp
  
  include/fastcgi++/config.h.in
)

target_link_libraries(fastcgipp
${Boost_LIBRARIES}
)

# set_target_properties(fastcgipp PROPERTIES
#  PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/config.h"
# )

add_subdirectory(examples)


install (TARGETS ${PROJECT_NAME} DESTINATION lib COMPONENT library)

file (GLOB headersHPP
	include/fastcgi++/*.hpp)
file (GLOB headersH
	include/fastcgi++/*.h)
install (FILES ${headersHPP} DESTINATION include/fastcgi++/ COMPONENT headers)
install (FILES ${headersH} DESTINATION include/fastcgi++/ COMPONENT headers)

# add_subdirectory(examples)
