add_executable(demo EXCLUDE_FROM_ALL demo.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include)
#  ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(demo PUBLIC itr)
