set(SRC_LIST CallerService.cc ../user.pb.cc)

add_executable(caller ${SRC_LIST})
target_link_libraries(caller mprpc protobuf)