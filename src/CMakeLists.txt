set(sources "ITR.cpp" "data.cpp")
list(APPEND sources "comp_search.cpp")
list(APPEND sources "angle_based_classifier.cpp" "linesearch.cpp" "vlbfgs.cpp")
include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(itr STATIC ${sources})
install(TARGETS itr DESTINATION lib)

