add_executable(gpio_example gpio.c)

target_link_libraries (gpio_example LINK_PUBLIC ${PROJECT_NAME})
