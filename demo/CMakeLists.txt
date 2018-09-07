add_executable(genData EXCLUDE_FROM_ALL SimulationData.cpp genData.cpp)

add_executable(demo EXCLUDE_FROM_ALL demo.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include)
target_link_libraries(demo itr)
