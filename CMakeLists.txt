# 声明要求的 cmake 最低版本
cmake_minimum_required( VERSION 3.10 )

# 声明一个 cmake 工程
project( net )

set(CMAKE_CXX_STANDARD 11)
# 设置编译模式为Debug模式
set( CMAKE_BUILD_TYPE "Debug" )

#INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3drpart/)

#Depend(buff git "https://github.com/zntx/buff.git" )

aux_source_directory(src SOURCES)

# 生成一个静态库，会生成一个libhello.a文件
add_library( ${PROJECT_NAME} STATIC ${SOURCES}
        include/Space.h)
# 生成一个共享库，会生成一个libhello_shared.so文件
#add_library( ${CMAKE_PROJECT_NAME} SHARED ${SOURCES} )

#aux_source_directory(demo D_SOURCES)
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#
## 生成可执行文件useHello
#add_executable( testnet ${D_SOURCES} )
#
## 将可执行程序链接到上库文件
#target_link_libraries( testnet net )
#if(WIN32)
#    target_link_libraries( testnet ws2_32 )
#endif()

add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/standir_dir/include/net
        COMMENT "Copying folder from ${CMAKE_CURRENT_SOURCE_DIR}/include to ${CMAKE_SOURCE_DIR}/standir_dir/include/net after building MyLibrary"
        VERBATIM
)

add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/net.h
        ${CMAKE_SOURCE_DIR}/standir_dir/include/
)

# 创建一个自定义命令，用于复制文件夹
#add_custom_command(
#        OUTPUT "${DESTINATION_FOLDER}"
#        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/standir_dir/include/net"
#        COMMENT "Copying folder from ${CMAKE_CURRENT_SOURCE_DIR}/include to ${CMAKE_SOURCE_DIR}/standir_dir/include/net"
#        VERBATIM
#)
# 创建一个自定义目标，该目标将执行上述自定义命令
#add_custom_target(CopyFolder ALL
#    DEPENDS "${DESTINATION_FOLDER}"
#)