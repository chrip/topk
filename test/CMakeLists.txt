
################################
# Unit Tests
################################
include_directories(../src)


add_executable( vectorspace_test vectorspace_test.cpp)
target_link_libraries(vectorspace_test ${PROJECT_NAME} gtest gtest_main)
add_test(NAME vectorspace COMMAND vectorspace_test)

add_executable( queryvector_test queryvector_test.cpp)
target_link_libraries(queryvector_test ${PROJECT_NAME} gtest gtest_main)
add_test(NAME queryvector COMMAND queryvector_test)

add_executable( topkqueue_test topkqueue_test.cpp)
target_link_libraries(topkqueue_test ${PROJECT_NAME} gtest gtest_main)
add_test(NAME topkqueue COMMAND topkqueue_test)
