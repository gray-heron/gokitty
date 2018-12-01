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
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
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
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/dependencies -DSDL2PP_WITH_MIXER=OFF -DSDL2PP_WITH_IMAGE=OFF -DSDL2PP_STATIC=ON
)

FIND_PACKAGE(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

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
  src/model_element.cpp
  src/visualisation.cpp
  src/hinge_model.cpp
  src/util.cpp
  src/data_reader.cpp

  inc/visualisation.h
  inc/model_element.h
  inc/config.h
  inc/exceptions.h
  inc/log.h
  inc/hinge_model.h
  inc/util.h
  inc/data_reader.h
)

set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 11)

target_link_libraries(${APP_NAME}
  ${Boost_LIBRARIES} pugixml SDL2pp ${SDL2_LIBRARIES} resources
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

 
