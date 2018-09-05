add_executable(test EXCLUDE_FROM_ALL test.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include)
target_link_libraries(test itr gtest_main)
