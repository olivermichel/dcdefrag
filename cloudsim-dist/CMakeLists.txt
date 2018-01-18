cmake_minimum_required(VERSION 3.5)
project(cloudsim)

set(CMAKE_CXX_STANDARD 11)

include(cmake/catch.cmake)
include(cmake/json.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_SRC
        test/edge_test.cc
        test/mapping_test.cc
        test/new_network_request_test.cc
        test/node_test.cc
        test/physical_edge_test.cc
        test/physical_network_test.cc
        test/physical_node_test.cc
        test/physical_resource_test.cc
        test/request_test.cc
        test/reservable_test.cc
        test/test_main.cc
        test/util_test.cc
        test/touchable_test.cc
        test/virtual_edge_test.cc
        test/virtual_network_test.cc
        test/virtual_node_test.cc
        test/virtual_resource_test.cc
        test/integration_test.cc)

set(LIB_SRC
        include/cloudsim/util.h
        src/algorithm.cc
        src/cloudsim.cc
        src/edge.cc
        src/greedy.cc
        src/logic_error.cc
        src/mapping.cc
        src/new_network_request.cc
        src/node.cc
        src/physical_edge.cc
        src/physical_network.cc
        src/physical_node.cc
        src/physical_resource.cc
        src/request.cc
        src/reservable.cc
        src/runtime_error.cc
        src/touchable.cc
        src/virtual_edge.cc
        src/virtual_network.cc
        src/virtual_node.cc
        src/virtual_resource.cc)

add_executable(test_runner ${TEST_SRC})
add_dependencies(test_runner cloudsim_shared)
target_link_libraries(test_runner cloudsim_shared)
target_include_directories(test_runner PUBLIC include ext/include)

add_executable(sandbox sandbox.cc ${LIB_SRC})
add_dependencies(test_runner cloudsim_shared)
target_link_libraries(sandbox cloudsim_shared)
target_include_directories(sandbox PUBLIC include ext/include)

add_executable(greedy_example examples/greedy_example.cc)
add_dependencies(greedy_example cloudsim_shared)
target_link_libraries(greedy_example cloudsim_shared)
target_include_directories(greedy_example PUBLIC include)

add_library(cloudsim_shared SHARED ${LIB_SRC})
add_library(cloudsim_static STATIC ${LIB_SRC})
target_include_directories(cloudsim_shared PUBLIC include ext/include)
target_include_directories(cloudsim_static PUBLIC include ext/include)
SET_TARGET_PROPERTIES(cloudsim_shared PROPERTIES OUTPUT_NAME cloudsim CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(cloudsim_static PROPERTIES OUTPUT_NAME cloudsim CLEAN_DIRECT_OUTPUT 1)
