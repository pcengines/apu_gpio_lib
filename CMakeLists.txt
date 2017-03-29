cmake_minimum_required(VERSION 3.0)

project(apu2_gpio)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 1)
set(VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared lib")

set(LIB_SRC src/apu2_gpio.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME} ${LIB_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
                      VERSION ${VERSION_STRING}
                      SOVERSION ${VERSION_MAJOR})

add_subdirectory (example)
