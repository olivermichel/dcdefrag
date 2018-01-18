set(PLEXUM_VERSION 0.5.1)

if(NOT EXISTS ${CMAKE_HOME_DIRECTORY}/ext/include/plexum/plexum.h)
    file(DOWNLOAD
        https://github.com/olivermichel/plexum/releases/download/${PLEXUM_VERSION}/graph.h
        ${CMAKE_HOME_DIRECTORY}/ext/include/plexum/graph.h
        )
endif()
