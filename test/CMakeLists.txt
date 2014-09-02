
################################
# Unit Tests
################################
include_directories(../src)
aux_source_directory(../src SRC_LIST)
# Add test cpp file
add_executable( vectorspace_test vectorspace_test.cpp ${SRC_LIST})
# Link test executable against gtest & gtest_main
target_link_libraries(vectorspace_test gtest gtest_main)
add_test( vectorspace_test vectorspace_test )

