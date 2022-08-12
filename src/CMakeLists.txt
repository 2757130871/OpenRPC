 
aux_source_directory(. SRC_LIST)

add_library(pmrpc ${SRC_LIST}) 
# mynet 最终编译成so动态库，设置动态库的路径，放在根目录的lib文件夹下面
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
#添加所需连接的库
target_link_libraries(pmrpc muduo_net muduo_base pthread zookeeper_mt)