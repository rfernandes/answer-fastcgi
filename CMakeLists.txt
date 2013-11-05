cmake_minimum_required(VERSION 2.6)

include_directories(BEFORE ./include)

add_definitions(
-DHAVE_ENDIAN_H
-DPACKAGE_VERSION="2.1"
)

find_package(Boost COMPONENTS system thread)

add_library(libfastcgipp SHARED
  src/http.cpp
  src/protocol.cpp
  src/request.cpp
  src/manager.cpp
  src/transceiver.cpp
  src/fcgistream.cpp
  src/utf8_codecvt_facet.cpp
)

include_directories(SYSTEM
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(libfastcgipp
${Boost_LIBRARIES}
)

add_executable(echo
examples/echo.cpp
)

target_link_libraries(echo
libfastcgipp
)

add_executable(showgnu
examples/showgnu.cpp
)

target_link_libraries(showgnu

libfastcgipp
)


add_executable(timer
examples/timer.cpp
)

target_link_libraries(timer
libfastcgipp
)

add_executable(upload
examples/upload.cpp
)

target_link_libraries(upload
libfastcgipp
)

# add_executable(session
# examples/session.cpp
# )

# add_subdirectory(examples)
