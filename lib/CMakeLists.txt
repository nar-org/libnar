project(libnar)

find_package(Boost 1.55 REQUIRED COMPONENTS exception)

include_directories(
    include
    ${Boost_INCLUDE_DIRS}
)

set(HEADERS
    include/nar/error.hpp
    include/nar/standard.hpp
    include/nar/roundup.hpp

    include/nar/header.hpp
)

set(SOURCES
    src/roundup.cpp
    src/header.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
)
