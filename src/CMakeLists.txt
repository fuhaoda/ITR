FILE(GLOB sources *.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/src)

add_library(itr STATIC ${sources})
install(TARGETS itr DESTINATION lib)
