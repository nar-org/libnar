project(nar)

find_package(Boost 1.55 REQUIRED
  COMPONENTS
    exception
    program_options
    filesystem
)

include_directories(
  include
  ${CMAKE_SOURCE_DIR}/lib/include
  ${Boost_INCLUDE_DIRS}
)

set(SOURCES
    src/Options.cpp
    src/Archive.cpp
    src/fstream.cpp

    src/main.cpp
)

set(HEADERS
  include/Options.hpp
  include/Archive.hpp
  include/nar/fstream.hpp
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    libnar
)
