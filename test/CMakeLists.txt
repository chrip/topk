
################################
# Unit Tests
################################
include_directories(../src)

configure_file(test_data/vector_space.json ${CMAKE_CURRENT_BINARY_DIR}/test_data/vector_space.json COPYONLY)
configure_file(test_data/query_vector.json ${CMAKE_CURRENT_BINARY_DIR}/test_data/query_vector.json COPYONLY)

add_executable( vector_space_test vector_space_test.cpp)
target_link_libraries(vector_space_test ${PROJECT_NAME} gtest gtest_main)
add_test(NAME vector_space COMMAND vector_space_test)

add_executable( query_vector_test query_vector_test.cpp)
target_link_libraries(query_vector_test ${PROJECT_NAME} gtest gtest_main)
add_test(NAME query_vector COMMAND query_vector_test)

add_executable( top_k_queue_test top_k_queue_test.cpp)
target_link_libraries(top_k_queue_test ${PROJECT_NAME} gtest gtest_main)
add_test(NAME top_k_queue COMMAND top_k_queue_test)

add_executable( top_k_queue_nrd_test top_k_queue_nrd_test.cpp)
target_link_libraries(top_k_queue_nrd_test ${PROJECT_NAME} gtest gtest_main)
add_test(NAME top_k_queue_nrd COMMAND top_k_queue_nrd_test)
