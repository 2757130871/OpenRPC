set(SRC_LIST test_zkutil.cc)

#生成的可执行文件名
add_executable(zk_client ${SRC_LIST})

#需要链接的动态库
target_link_libraries(zk_client protobuf pthread pmrpc zookeeper_mt)

#设置项目可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/example/test_zkutil)