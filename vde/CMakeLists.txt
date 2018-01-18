cmake_minimum_required(VERSION 3.5)
project(vde)

set(GUROBI_PATH   /Library/gurobi701)
set(CLOUDSIM_PATH /Users/olli/src/cloudsim)
set(PLEXUM_INC    /Users/olli/src/plexum/include)
set(BUILD_SERVER  false)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    SET(IS_DARWIN TRUE)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    SET(IS_LINUX TRUE)
endif ()

set(CMAKE_CXX_STANDARD 11)

include(cmake/json.cmake)
include(cmake/args.cmake)

if (${IS_DARWIN})
    set(CLOUDSIM_LIB_SO ${CLOUDSIM_PATH}/build/libcloudsim.dylib)
else ()
    set(CLOUDSIM_LIB_SO ${CLOUDSIM_PATH}/build/libcloudsim.so)
endif ()

set(CLOUDSIM_LIB_A  ${CLOUDSIM_PATH}/build/libcloudsim.a)
set(CLOUDSIM_INC    ${CLOUDSIM_PATH}/include)

if (${IS_DARWIN})
    set(GUROBI_PREFIX mac64)
else ()
    set(GUROBI_PREFIX linux64)
endif()

set(GUROBI_LIB_SO     ${GUROBI_PATH}/${GUROBI_PREFIX}/lib/libgurobi70.so)
set(GUROBI_LIB_CPP_A  ${GUROBI_PATH}/${GUROBI_PREFIX}/lib/libgurobi_c++.a)
set(GUROBI_INC        ${GUROBI_PATH}/${GUROBI_PREFIX}/include)

if (BUILD_SERVER)
    set(POCO_PREFIX       "/usr/local")
    set(POCO_INCLUDE_DIR" ${POCO_PREFIX}/include")
    set(POCO_LIB_DIR      "${POCO_PREFIX}/lib")
    set(POCO_LIBS         "${POCO_LIB_DIR}/libPocoNet.dylib"
                          "${POCO_LIB_DIR}/libPocoUtil.dylib"
                          "${POCO_LIB_DIR}/libPocoFoundation.dylib")
endif ()

include_directories(${PLEXUM_INC})
include_directories(${CLOUDSIM_INC})
include_directories(${GUROBI_INC})

# require both gurobi .so lib and c++ extensions in .a lib
link_libraries(${CLOUDSIM_LIB_SO})
link_libraries(${GUROBI_LIB_CPP_A})
link_libraries(${GUROBI_LIB_SO})

if (BUILD_SERVER)
    link_libraries("${POCO_LIBS}")
endif()

set(SIM_SRC util/bipartite_matching.cc
            algo/tavdce.cc
            server/network_manager.cc
            util/sim_helpers.h)

set(GEN_SRC util/sim_helpers.h)

if (BUILD_SERVER)
    set(VDCE_SERVER_SRC util/bipartite_matching.cc
                        algo/rabbani_vdce/vdce0.cc
                        algo/tavdce.cc
                        algo/report.cc
                        util/util.cc
                        server/server.cc
                        server/request_handler_factory.cc
                        server/rest_request_handler.cc
                        server/network_manager.cc)
endif ()

set(MCF_TEST_SRC util/bipartite_matching.cc)

# stand alone simulator
add_executable(sim sim_main.cc ${SIM_SRC})
target_include_directories(sim PUBLIC ext/include)

# workload generator
add_executable(gen gen_main.cc ${GEN_SRC})
target_include_directories(gen PUBLIC ext/include)

if (BUILD_SERVER)
    # server application
    add_executable(vdce_server vdce_server.cc ${VDCE_SERVER_SRC})
    target_include_directories(vdce_server PUBLIC $(POCO_INCLUDE_DIR) ext/include)
endif ()

add_executable(mcf_test mcf_test.cc ${MCF_TEST_SRC})
target_include_directories(mcf_test PUBLIC ext/include)

add_executable(sim_test sim_test_main.cc ${SIM_SRC})
target_include_directories(sim_test PUBLIC ext/include)