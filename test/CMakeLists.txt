list(APPEND test_sources ${json_equation_sources})
add_executable(json_equation_test ${test_sources} ${CMAKE_CURRENT_LIST_DIR}/json_equation_test.cpp)