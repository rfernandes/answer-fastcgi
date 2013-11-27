add_executable(echo
echo.cpp
)

target_link_libraries(echo
fastcgipp
)

add_executable(showgnu
showgnu.cpp
)

target_link_libraries(showgnu
fastcgipp
)


add_executable(timer
timer.cpp
)

target_link_libraries(timer
fastcgipp
)

add_executable(upload
upload.cpp
)

target_link_libraries(upload
fastcgipp
)
