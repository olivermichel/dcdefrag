set(CATCH_VERSION 1.8.0)

if(NOT EXISTS ${CMAKE_HOME_DIRECTORY}/ext/include/catch/catch.h)
    file(DOWNLOAD
            https://github.com/philsquared/Catch/releases/download/v${CATCH_VERSION}/catch.hpp
            ${CMAKE_HOME_DIRECTORY}/ext/include/catch/catch.h
            )
endif()