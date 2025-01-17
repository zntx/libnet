


# linnaem url tag "tag" version "1,23"
function(git_download)

	math(EXPR end_index "${ARGC} - 1")

	foreach(index RANGE 0 ${end_index})
		#set(this_argv ARGV${index})

		if( ${ARGV${index}} STREQUAL "branch")
			message( "Index: ${index}, Argument: ${ARGV${index}}")
			math(EXPR tag_index "${index} + 1")
			message( "Index2: ${tag_index}, Argument: ${ARGV${tag_index}}")
			set(git_tag  ${ARGV${tag_index}})
		endif()

		if( ${ARGV${index}} STREQUAL "version")
			message( "Index: ${index}, Argument: ${ARGV${index}}")
			math(EXPR tag_index "${index} + 1")
			message( "Index2: ${tag_index}, Argument: ${ARGV${tag_index}}")
			set(git_version  ${ARGV${tag_index}})
		endif()

	endforeach()
	
	execute_process(
		COMMAND git clone ${ARGV1}  ${ARGV0}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3drpart
		RESULT_VARIABLE GIT_RESULT
		OUTPUT_VARIABLE GIT_BRANCH
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if(GIT_RESULT EQUAL "0")
		#message("仓库 ${REPOSITORY_URL} 当前所在分支为 ${GIT_BRANCH}")
	else()
		message(FATAL_ERROR "执行 git 命令失败")
	endif()


	if(${git_tag} )
		execute_process(
			COMMAND "git" checkout -b ${git_tag}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3drpart/${ARGV0}
			RESULT_VARIABLE GIT_RESULT
			OUTPUT_VARIABLE GIT_BRANCH
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		if(GIT_RESULT EQUAL "0")
		#message("仓库 ${REPOSITORY_URL} 当前所在分支为 ${GIT_BRANCH}")
		else()
			message(FATAL_ERROR "执行 git 命令失败")
		endif()
	endif()

	if(${git_version} )
		execute_process(
			COMMAND git reset --hard ${git_version}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3drpart/${ARGV0}
			RESULT_VARIABLE GIT_RESULT
			OUTPUT_VARIABLE GIT_BRANCH
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		if(GIT_RESULT EQUAL "0")
		#message("仓库 ${REPOSITORY_URL} 当前所在分支为 ${GIT_BRANCH}")
		else()
			message(FATAL_ERROR "执行 git 命令失败")
		endif()
	endif()



	
endfunction(git_download)





#Depend(net)
#Depend(net git "url") branch 
#Depend(net path "path" version  1.3)
function(Depend )

	if(${ARGC} GREATER_EQUAL 1)
		if(EXISTS ${CMAKE_SOURCE_DIR}/3drpart/${ARGV0})
			message("---->${CMAKE_SOURCE_DIR}/3drpart/${ARGV0} exit" )
			add_subdirectory(${CMAKE_SOURCE_DIR}/3drpart/${ARGV0}  ${CMAKE_SOURCE_DIR}/3drpart/${ARGV0}/build)
			include_directories(${CMAKE_SOURCE_DIR}/3drpart/${ARGV0})
			return()
		endif()
	else()
		message( FATAL_ERROR "parameter error")
	endif()

	if(${ARGC} GREATER_EQUAL 3)
		# 使用range()函数截取参数列表的子集，从第2个参数开始
		math(EXPR end_index "${ARGC} - 1")

		set(args ${ARGV0})
		foreach(index RANGE 2 ${end_index})
			#set(this_argv ARGV${index})
			message( "Index: ${index}, Argument: ${ARGV${index}}")
			list(APPEND args ${ARGV${index}})
		endforeach()
		git_download(${args})
	else()
		message("未知选项")
	endif()

	message( "${CMAKE_SOURCE_DIR}/3drpart/${ARGV0")
	add_subdirectory(${CMAKE_SOURCE_DIR}/3drpart/${ARGV0})
	include_directories(${CMAKE_SOURCE_DIR}/3drpart/${ARGV0})

endfunction()
