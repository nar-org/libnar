project(libnar)

find_package(Boost 1.54 REQUIRED COMPONENTS exception)

include_directories(
    include
    ${Boost_INCLUDE_DIRS}
)

set(HEADERS
    include/nar/error.hpp
    include/nar/standard.hpp
    include/nar/roundup.hpp

    include/nar/generic/header.hpp
    include/nar/generic/item.hpp
    include/nar/header.hpp
    include/nar/item.hpp
)

set(SOURCES
    src/roundup.cpp
    src/header.cpp
    src/item.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
)
