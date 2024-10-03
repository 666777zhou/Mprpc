aux_source_directory(. SRC_LIST)
add_library(mprpc ${SRC_LIST})
set_target_properties(mprpc PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)
