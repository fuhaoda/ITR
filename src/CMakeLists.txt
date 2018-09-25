include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(itr STATIC Covariate.cpp Data.cpp)
install(TARGETS itr DESTINATION lib)


