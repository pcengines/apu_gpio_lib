add_executable(blinky blinky.c)
add_executable(simswap simswap.c)

target_link_libraries (blinky LINK_PUBLIC ${PROJECT_NAME})
target_link_libraries (simswap LINK_PUBLIC ${PROJECT_NAME})
