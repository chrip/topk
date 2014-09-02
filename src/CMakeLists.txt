
include_directories(.)
aux_source_directory(. SRC_LIST)
add_library(${PROJECT_NAME} ${SRC_LIST})

