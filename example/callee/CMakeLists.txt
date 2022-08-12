
set(SRC_LIST UserService.cc ../User.pb.cc)

#生成的可执行文件名
add_executable(provider ${SRC_LIST})

#需要链接的动态库
target_link_libraries(provider protobuf pmrpc)