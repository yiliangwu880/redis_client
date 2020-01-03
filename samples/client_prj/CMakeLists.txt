# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

set(project_name "client_prj")
# 项目信息
project (${project_name} CXX)

# include directories
INCLUDE_DIRECTORIES(system ../../include/)
INCLUDE_DIRECTORIES(after  ${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(after  ${PROJECT_SOURCE_DIR}/../base/)
INCLUDE_DIRECTORIES(after  ${PROJECT_SOURCE_DIR}/../external/)


# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
file(GLOB_RECURSE SRCS  ${PROJECT_SOURCE_DIR} "../base/*.cpp")
file(GLOB_RECURSE SRCS2  ${PROJECT_SOURCE_DIR} "*.cc")

# 指定生成目标
ADD_EXECUTABLE(${project_name}  ${SRCS}  ${SRCS2})


add_dependencies(${project_name} credis_client)
# 设置链接库
target_link_libraries(${project_name} credis_client libcevent_cpp.a)





