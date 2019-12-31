#环境变量说明:
#CMAKE_SOURCE_DIR  CMakeList.txt所在的目录
#CMAKE_SOURCE_DIR,PROJECT_SOURCE_DIR,_SOURCE_DIR： 这三个变量内容一致，都指的是工程的顶级目录。
#PROJECT_BINARY_DIR 指执行 cmake的路径

PROJECT(dbproxy)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#cotire设置gcc的预编译头相关
SET (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
INCLUDE(cotire)

# include directories
INCLUDE_DIRECTORIES(system ./include/)
INCLUDE_DIRECTORIES(system ./external/protobuf/include/)
INCLUDE_DIRECTORIES(after  ${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(after  ./external)
INCLUDE_DIRECTORIES(after  ./proto_cpp/)
INCLUDE_DIRECTORIES(after  ./com/)
INCLUDE_DIRECTORIES(after  ./external/cpp_cfg/com/)

#运行cmake输出终端消息，调试可以用
message(PROJECT_SOURCE_DIR_is: ${PROJECT_SOURCE_DIR})

# lib directories
LINK_DIRECTORIES(
  #项目生成的静态链接库
  ${CMAKE_SOURCE_DIR}/lib

  #项目引用第三方库
  ${PROJECT_SOURCE_DIR}/external/protobuf/lib
  ${PROJECT_SOURCE_DIR}/external/svr_util/lib
  ${PROJECT_SOURCE_DIR}/external/libevent_cpp/lib
  ${PROJECT_SOURCE_DIR}/external/access_svr/acc_driver/lib
  ${PROJECT_SOURCE_DIR}/external/access_svr/acc_proto/lib
  ${PROJECT_SOURCE_DIR}/external/msg_forward/mf_driver/lib
  ${PROJECT_SOURCE_DIR}/external/msg_forward/mf_proto/lib


  #系统目录
  /usr/local/lib
  /use/lib
)


#指定编译器
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -lrt -pthread -DDEBUG -Werror -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -lrt -pthread   -Werror -DDEBUG -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
else( CMAKE_BUILD_TYPE STREQUAL "Release" )
    #set(CMAKE_BUILD_TYPE "Release")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -lrt -DGMPERMISSION -pthread -Werror -Wno-unused-result -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free ")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -lrt -O2 -DGMPERMISSION -pthread -Werror -Wno-unused-result -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

#自定义命令,根据依赖关系导出proto cpp文件
##################################################
file(GLOB_RECURSE PROTO_FILES  ${CMAKE_SOURCE_DIR} "proto/*.proto")

#对文件列表变量用 STRING(REPLACE 。。。批量改名无效的，因此写个实现
foreach(element ${PROTO_FILES})
	message(element=${element})
	STRING(REPLACE ".proto" ".pb.h" h_element ${element} ) #改扩展名
	STRING(REPLACE "/proto" "/proto_cpp" h_element ${h_element} ) #该路径
	set(PROTO_CPP_FILES ${PROTO_CPP_FILES} ${h_element})
endforeach()

message(PROTO_CPP_FILES=${PROTO_CPP_FILES})
add_custom_command(
	OUTPUT ${PROTO_CPP_FILES}
	DEPENDS ${PROTO_FILES}
	COMMAND sh ./proto/proto_2_cpp.sh
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} #COMMAND 执行目录
	COMMENT "========export cpp from proto========"
)
add_custom_target(TargetPB ALL DEPENDS ${PROTO_CPP_FILES})
##################################################


ADD_SUBDIRECTORY(simulate_client)
ADD_SUBDIRECTORY(login)
ADD_SUBDIRECTORY(statistics)
ADD_SUBDIRECTORY(team)
ADD_SUBDIRECTORY(zone)
