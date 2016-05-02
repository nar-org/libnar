cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

include(cmake/check-cpp-flags.cmake)

add_subdirectory(lib)
add_subdirectory(app)

# TODO(NicolasDP) make this optional
enable_testing()
add_subdirectory(tests)
add_custom_target(test-build ALL
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMAND ${CMAKE_CTEST_COMMAND}
  DEPENDS libnar-tests
)
