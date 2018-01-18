set(JSON_VERSION 2.1.1)

if(NOT EXISTS ${CMAKE_HOME_DIRECTORY}/ext/include/json/json.h)
    file(DOWNLOAD
            https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/json.hpp
            ${CMAKE_HOME_DIRECTORY}/ext/include/json/json.h
            )
endif()