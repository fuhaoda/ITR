add_executable(test EXCLUDE_FROM_ALL test.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(test itr gtest_main)

add_custom_command(
  TARGET test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_SOURCE_DIR}/test/sample100.csv
  ${CMAKE_CURRENT_BINARY_DIR}/sample100.csv
  )
