include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(itr STATIC ITR.cpp data.cpp comp_search.cpp angle_based_classifier.cpp linesearch.cpp vlbfgs.cpp)
install(TARGETS itr DESTINATION lib)

