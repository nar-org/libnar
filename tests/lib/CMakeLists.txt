project(libnar-tests)

find_package(GTest REQUIRED)
find_package (Threads REQUIRED)

include_directories(
    include
    ${CMAKE_SOURCE_DIR}/lib/include
    ${GTEST_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}
    src/MagicNumber.cpp
    src/Roundup.cpp
    src/SerialisationHeader.cpp

    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    ${GTEST_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    libnar
)
add_test(library-tests ${PROJECT_NAME})
