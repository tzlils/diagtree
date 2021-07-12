cmake_minimum_required (VERSION 3.8)
project(diagtree VERSION 0.3.0 DESCRIPTION "A library for dialogue trees" LANGUAGES C CXX)

if (NOT DEFINED DIAGTREE_YAML_SRC_DIR)
  set(DIAGTREE_YAML_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp")
  add_subdirectory(${DIAGTREE_YAML_SRC_DIR})
endif()

# idk if this actually does anything
# add_library(diagtree ${CMAKE_CURRENT_SOURCE_DIR}/include/diagtree/tree.hpp)
# set_target_properties(diagtree PROPERTIES LINKER_LANGUAGE CXX)
# target_include_directories(diagtree PUBLIC "${DIAGTREE_YAML_SRC_DIR}/include")

option(BUILD_EXAMPLE "Build the example program" ON)
if (DEFINED BUILD_EXAMPLE)
    add_executable(example examples/example.cpp)
    target_include_directories(example PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_include_directories(example PUBLIC "${DIAGTREE_YAML_SRC_DIR}/include")
    target_link_libraries(example yaml-cpp)
endif()
