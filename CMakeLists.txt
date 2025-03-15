# 声明要求的 cmake 最低版本
cmake_minimum_required( VERSION 3.10 )

# 声明一个 cmake 工程
project( net VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 11)
# 设置编译模式为Debug模式
set( CMAKE_BUILD_TYPE "Debug" )

# 判断是否为顶层 CMake
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	# ============================= linux静态.so库输出路径【默认路径在-B build目录下】 =============================
	#linux .so输出路径
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/staging_dir/lib")
 
	# ============================= 执行程序输出路径 =============================
	#执行程序、dll动态库、pdb调试文件
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/staging_dir/bin")
 
	# ============================= 归档输出路径 =============================
	# 静态库 .lib 动态库.lib地址文件 linux静态库.a 静态库pdb调试文件
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/staging_dir/lib")
endif()

set(CMAKE_PREFIX_PATH "../../staging_dir;${CMAKE_PREFIX_PATH}")

find_package(librust CONFIG)
if(librust_FOUND)
    message(STATUS "librust found. path ${LIBRUST_INCLUDE_DIRS}")
    include_directories(${LIBRUST_INCLUDE_DIRS})
    link_directories(${librust_LIBRARY_DIRS})
else()
    message(STATUS "librust not found")

    # 包含ExternalProject模块
    include(ExternalProject)
    ExternalProject_Add(
        librust
        GIT_REPOSITORY https://github.com/zntx/librust.git            # 替换为实际仓库或URL
        GIT_TAG main                                                  # 指定分支或标签
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/librust                 # 源码路径
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_LIST_DIR}/staging_dir/  # 指定安装路径
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}                         # 传递构建类型
        BUILD_ALWAYS OFF                                               # 仅在需要时重新构建
    )
endif()


aux_source_directory(src SOURCES)

# 生成一个静态库，会生成一个libhello.a文件
add_library( "${PROJECT_NAME}STATIC" STATIC ${SOURCES} )
# 生成一个共享库，会生成一个libhello_shared.so文件
#add_library( "${PROJECT_NAME}SHARED"  SHARED ${SOURCES} )

#aux_source_directory(demo D_SOURCES)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#target_link_libraries( "${PROJECT_NAME}STATIC" rust )
#target_link_libraries( "${PROJECT_NAME}SHARED" rust )

# 设置库的输出名称
set_target_properties("${PROJECT_NAME}STATIC" PROPERTIES OUTPUT_NAME "libnet")
#set_target_properties("${PROJECT_NAME}SHARED" PROPERTIES OUTPUT_NAME "libnet")

## 生成可执行文件useHello
#add_executable( testnet ${D_SOURCES} )


## 将可执行程序链接到上库文件
#target_link_libraries( testnet net )
#if(WIN32)
#    target_link_libraries( testnet ws2_32 )
#endif()

#add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/standir_dir/include/net
#        COMMENT "Copying folder from ${CMAKE_CURRENT_SOURCE_DIR}/include to ${CMAKE_SOURCE_DIR}/standir_dir/include/net after building MyLibrary"
#        VERBATIM
#)

#add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        ${CMAKE_CURRENT_SOURCE_DIR}/net.h
#        ${CMAKE_SOURCE_DIR}/standir_dir/include/
#)

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

# 安装库和头文件
install(TARGETS "${PROJECT_NAME}STATIC"
        EXPORT libnetTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        )  # 导出目标到 libnetTargets

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION include/net
        FILES_MATCHING PATTERN "*.h"
)

install(FILES ${PROJECT_SOURCE_DIR}/net.h
        DESTINATION include
)

# 设置包含目录（安装时的路径）
set(LIBNET_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include/")
set(LIBNET_LIBRARY_DIRS "${CMAKE_INSTALL_PREFIX}/lib")

set(PACKAGE_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include/")
set(PACKAGE_LIBRARY_DIRS "${CMAKE_INSTALL_PREFIX}/lib")

# 生成 CMake 配置文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/libnetConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/libnetConfig.cmake"
        INSTALL_DESTINATION lib/cmake/libnet
        PATH_VARS LIBNET_INCLUDE_DIRS LIBNET_LIBRARY_DIRS
)

# 安装 CMake 配置文件
install( FILES
        "${CMAKE_CURRENT_BINARY_DIR}/libnetConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libnetConfigVersion.cmake"
        DESTINATION lib/cmake/libnet
)

# 安装导出的目标文件
install(EXPORT libnetTargets
        FILE libnetTargets.cmake
        NAMESPACE libnet::
        DESTINATION lib/cmake/libnet)

# 生成供构建目录使用的目标文件
export(EXPORT libnetTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/libnetTargets.cmake"
        NAMESPACE libnet::)