add_executable(test EXCLUDE_FROM_ALL test.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(test itr gtest_main)

#add_subdirectory(util) 
