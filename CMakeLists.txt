cmake_minimum_required(VERSION 2.6)

project(fastcgipp)

add_definitions(
-DHAVE_ENDIAN_H
-DPACKAGE_VERSION="2.1"
-std=c++11
)

find_package(Boost COMPONENTS system thread)

add_library(fastcgipp SHARED
  src/http.cpp
  src/protocol.cpp
  src/request.cpp
  src/manager.cpp
  src/transceiver.cpp
  src/fcgistream.cpp
)

include_directories(BEFORE ./include)
include_directories(SYSTEM
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(fastcgipp
${Boost_LIBRARIES}
)

add_executable(echo
examples/echo.cpp
)

target_link_libraries(echo
fastcgipp
)

add_executable(showgnu
examples/showgnu.cpp
)

target_link_libraries(showgnu

fastcgipp
)


add_executable(timer
examples/timer.cpp
)

target_link_libraries(timer
fastcgipp
)

add_executable(upload
examples/upload.cpp
)

target_link_libraries(upload
fastcgipp
)

install (TARGETS ${PROJECT_NAME} DESTINATION lib COMPONENT library)

file (GLOB headersHPP
	include/answer/*.hpp)
install (FILES ${headersHPP} DESTINATION include/answer COMPONENT headers)

# add_subdirectory(examples)
