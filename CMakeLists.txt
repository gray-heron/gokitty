cmake_minimum_required(VERSION 3.0)
project(gokitty)

include(CTest)
include(ExternalProject)
include(CMakeRC.cmake)

enable_testing()

# ==============================================================================
# cmake options
# ==============================================================================
option(USE_LOCAL_GTEST "uses local gtest" OFF)
option(USE_LOCAL_SPDLOG "uses local spdlog" OFF)

# ==============================================================================
# names
# ==============================================================================
set(APP_NAME ${PROJECT_NAME})

# ==============================================================================
# compiler settings
# ==============================================================================

add_compile_options(-std=c++17 -Wall -Werror)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options(-fno-limit-debug-info)
endif()

include_directories(inc)
include_directories(${CMAKE_BINARY_DIR}/gen)
include_directories(${CMAKE_BINARY_DIR}/dependencies/include)

# ==============================================================================
# dependencies
# ==============================================================================

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

ExternalProject_Add(pugixml-dependency
  URL https://github.com/zeux/pugixml/releases/download/v1.9/pugixml-1.9.tar.gz
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/dependencies -DSPDLOG_BUILD_EXAMPLES=0 -DSPDLOG_BUILD_BENCH=0 -DSPDLOG_BUILD_TESTING=0 -DSPDLOG_BUILD_TESTING=0
)

ExternalProject_Add(spdlog-dependency
  URL https://github.com/gabime/spdlog/archive/v1.1.0.tar.gz
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/dependencies
)

ExternalProject_Add(sdl2-dependency
  URL https://github.com/libSDL2pp/libSDL2pp/archive/0.16.0.tar.gz
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/dependencies -DSDL2PP_WITH_IMAGE=ON -DSDL2PP_WITH_TTF=ON -DSDL2PP_WITH_MIXER=OFF
)

find_package(Torch REQUIRED)
link_directories(${CMAKE_BINARY_DIR}/dependencies/lib)

# ==============================================================================
# resources
# ==============================================================================
cmrc_add_resource_library(resources res/default_configuration.xml)


# ==============================================================================
# cmake targets
# ==============================================================================

add_executable(${APP_NAME}
  src/log.cpp
  src/config.cpp
  src/main.cpp
)

target_link_libraries(${APP_NAME}
  ${Boost_LIBRARIES} pugixml SDL2pp torch resources
)

add_dependencies(${APP_NAME} spdlog-dependency)
add_dependencies(${APP_NAME} pugixml-dependency)
add_dependencies(${APP_NAME} sdl2-dependency)

# ==============================================================================
# tests
# ==============================================================================
ExternalProject_Add(googletest-dependency
    URL https://github.com/google/googletest/archive/release-1.8.0.tar.gz
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/dependencies"
  )
  ADD_LIBRARY (gtest_main STATIC IMPORTED)
  ADD_LIBRARY (gtest STATIC IMPORTED)
  SET_PROPERTY(TARGET gtest_main PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/dependencies/lib/libgtest_main.a)
  SET_PROPERTY(TARGET gtest PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/dependencies/lib/libgtest.a)
  set(GTEST_BOTH_LIBRARIES gtest gtest_main)
  #add_dependencies(${TEST_APP_NAME} googletest-dependency)

 