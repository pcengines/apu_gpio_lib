add_executable(blinky blinky.c)
add_executable(simswap simswap.c)

target_link_libraries (blinky LINK_PUBLIC ${PROJECT_NAME})
target_link_libraries (simswap LINK_PUBLIC ${PROJECT_NAME})

if(${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
add_executable(apu2_gpiod apu2_gpiod.c)
target_link_libraries (apu2_gpiod LINK_PUBLIC ${PROJECT_NAME})
endif()
