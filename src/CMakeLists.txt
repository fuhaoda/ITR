include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(itr STATIC ITR.cpp Data.cpp SearchEngine.cpp Covariate.cpp)
install(TARGETS itr DESTINATION lib)
