include_directories(${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/src/include)
#add_library(itr STATIC ITR.cpp Data.cpp SearchEngine.cpp Covariate.cpp)
add_library(itr STATIC data.cpp comp_search.cpp)
install(TARGETS itr DESTINATION lib)
