set(ARGS_VERSION 6.0.4)

if(NOT EXISTS ${CMAKE_HOME_DIRECTORY}/ext/include/args/args.h)
    file(DOWNLOAD
            https://raw.githubusercontent.com/Taywee/args/${ARGS_VERSION}/args.hxx
            ${CMAKE_HOME_DIRECTORY}/ext/include/args/args.h
            )
endif()


