project(libnar)

include_directories(include)

set(HEADERS
    include/nar/error.hpp
    include/nar/standard.hpp
    include/nar/roundup.hpp

    include/nar/header.hpp
)

set(SOURCES
    src/roundup.cpp
    src/header.cpp
    src/error.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
    ${HEADERS}
)
