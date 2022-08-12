cmake_minimum_required(VERSION 3.0)
project(pmRPC)


#调试使用
 #set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#设置编译时查找的头文件路径 -I
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
#设置项目可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#设置项目库文件输出路径 
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(src)
#add_subdirectory(test)
add_subdirectory(example)

