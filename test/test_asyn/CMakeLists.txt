# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
set(project_name "test_asyn")
# 项目信息
project (${project_name} CXX)


# include directories
INCLUDE_DIRECTORIES(system ../external/)
INCLUDE_DIRECTORIES(after  ${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(after  ${PROJECT_SOURCE_DIR}/../com/)
INCLUDE_DIRECTORIES(after  ${PROJECT_SOURCE_DIR}/../../samples/external/libevent_cpp/include/)


# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
file(GLOB_RECURSE SRCS2  ${PROJECT_SOURCE_DIR} "*.cpp")
file(GLOB_RECURSE SRCS3  ${PROJECT_SOURCE_DIR} "../com/*.cpp")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${project_name})
# 指定生成目标
ADD_EXECUTABLE(${project_name} ${SRCS2} ${SRCS3})

# 设置链接库
target_link_libraries(${project_name} cevent_cpp)

