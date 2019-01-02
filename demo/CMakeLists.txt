add_executable(demo EXCLUDE_FROM_ALL demo.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include)
target_link_libraries(demo itr)

add_custom_command(
  TARGET demo POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_SOURCE_DIR}/sample100.csv
  ${CMAKE_CURRENT_BINARY_DIR}/sample100.csv
  )
