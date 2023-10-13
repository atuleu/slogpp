include(CMakeParseArguments)

function(git_version)
	set(ARGS_Options)

	set(ARGS_OneValue "")

	list(
		APPEND
		ARGS_OneValue #
		PROJECT_PREFIX #
		PROJECT_SOURCE #
		FALLBACK_VERSION #
		OUTPUT_VERSION #
	)

	set(ARGS_MultiValue "")

	cmake_parse_arguments(
		ARGS "${ARGS_Options}" "${ARGS_OneValue}" "${ARGS_MultiValue}" ${ARGN}
	)

	if(NOT ARGS_PROJECT_SOURCE)
		set(ARGS_PROJECT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
	endif(NOT ARGS_PROJECT_SOURCE)

	if(NOT ARGS_PROJECT_PREFIX)
		message(FATAL_ERROR "GitVersion: Missing project prefix")
	elseif(NOT ARGS_PROJECT_PREFIX MATCHES ".*_$")
		set(ARGS_PROJECT_PREFIX ${ARGS_PROJECT_PREFIX}_)
		message(
			AUTHOR_WARNING
				"GitVersion: prefix does not end with \"_\" using: ${ARGS_PROJECT_PREFIX}"
		)
	endif()

	if(NOT ARGS_OUTPUT_VERSION)
		set(ARGS_OUTPUT_VERSION PROJECT_VERSION)
	endif(NOT ARGS_OUTPUT_VERSION)

	if(NOT ARGS_OUTPUT_VERSION)
		set(ARGS_OUTPUT_VERSION PROJECT_FULL_VERSION)
	endif(NOT ARGS_OUTPUT_VERSION)

	set(GitVersion_ARGS #
		PROJECT_SOURCE ${ARGS_PROJECT_SOURCE} #
		VERSION_FILE ${CMAKE_CURRENT_BINARY_DIR}/.version
	)

	if(DEFINED ARGS_FALLBACK_VERSION)
		list(APPEND GitVersion_ARGS FALLBACK_VERSION ${ARGS_FALLBACK_VERSION})
	endif(DEFINED ARGS_FALLBACK_VERSION)

	list(JOIN GitVersion_ARGS "\\;" GitVersion_ARGS)

	execute_process(
		COMMAND
			${CMAKE_COMMAND} #
			-DGitVersion_RUN:BOOL=True #
			-DGitVersion_ARGS:STRING=${GitVersion_ARGS} #
			-P ${CMAKE_CURRENT_FUNCTION_LIST_FILE} #
			COMMAND_ERROR_IS_FATAL ANY #
	)

	set(version_file ${CMAKE_CURRENT_BINARY_DIR}/.version)

	file(READ ${version_file} version_file_content)
	string(REPLACE " " ";" version_file_list ${version_file_content})
	list(GET version_file_list 0 base_version)
	list(GET version_file_list 1 full_version)

	add_custom_target(
		${ARGS_PROJECT_PREFIX}Version ALL
		BYPRODUCTS ${version_file}
		COMMAND
			${CMAKE_COMMAND} #
			-DGitVersion_RUN:BOOL=True #
			-DGitVersion_ARGS:STRING="${GitVersion_ARGS}" #
			-P ${CMAKE_CURRENT_FUNCTION_LIST_FILE} #
			COMMAND_ERROR_IS_FATAL ANY #
	)

	set_property(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		APPEND
		PROPERTY CMAKE_CONFIGURE_DEPENDS ${version_file}
	)

	set(${ARGS_OUTPUT_VERSION}
		${base_version}
		PARENT_SCOPE
	)

	set(${ARGS_PROJECT_PREFIX}FULL_VERSION
		${full_version}
		PARENT_SCOPE
	)

endfunction(git_version)

function(_git_version_from_git_describe)
	set(ARGS_Options "")
	list(APPEND ARGS_Options ENSURE)
	set(ARGS_OneValue)

	list(APPEND ARGS_OneValue PROJECT_SOURCE OUTPUT_VERSION OUTPUT_FULL_VERSION)

	set(ARGS_MultiValue "")

	cmake_parse_arguments(
		ARGS "${ARGS_Options}" "${ARGS_OneValue}" "${ARGS_MultiValue}" ${ARGN}
	)

	foreach(a ${ARGS_OneValue})
		if(NOT DEFINED ARGS_${a})
			message(
				FATAL_ERROR
					"GitVersion:_from_git_describe missing required ${a} arguments."
			)
		endif(NOT DEFINED ARGS_${a})
	endforeach(a ${ARGS_OneValue})

	if(NOT ARGS_ENSURE)
		set(message_type NOTICE)
	else(NOT ARGS_ENSURE)
		set(message_type FATAL_ERROR)
	endif(NOT ARGS_ENSURE)

	find_package(Git REQUIRED)

	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
		WORKING_DIRECTORY ${ARGS_PROJECT_SOURCE}
		OUTPUT_VARIABLE describe_name
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_VARIABLE git_describe_error
		ERROR_STRIP_TRAILING_WHITESPACE
		RESULT_VARIABLE git_describe_result
	)

	if(NOT git_describe_result EQUAL 0)
		message(
			${message_type}
			"GitVersion: could not run git describe: ${git_describe_error}, using fallback version"
		)
		return()
	endif(NOT git_describe_result EQUAL 0)

	# Match any part containing digits and periods (strips out rc and so on)
	if(NOT describe_name MATCHES "^([v]?([0-9\\.]+).*)")
		message(${message_type} "GitVersion: Version tag is ill-formatted\n"
				"  Describe-name: ${describe_name}, using fallback version."
		)
		return()
	endif()

	set(${ARGS_OUTPUT_VERSION}
		${CMAKE_MATCH_2}
		PARENT_SCOPE
	)

	set(${ARGS_OUTPUT_FULL_VERSION}
		${CMAKE_MATCH_1}
		PARENT_SCOPE
	)

endfunction(_git_version_from_git_describe)

function(_git_version_run)
	set(ARGS_Options)

	set(ARGS_OneValue
		ARGS_OneValue #
		PROJECT_SOURCE #
		VERSION_FILE #
		FALLBACK_VERSION #
	)
	set(ARGS_MultiValue "")

	cmake_parse_arguments(
		ARGS "${ARGS_Options}" "${ARGS_OneValue}" "${ARGS_MultiValue}" ${ARGN}
	)

	set(REQUIRED_ARGS PROJECT_SOURCE VERSION_FILE)

	foreach(a ${REQUIRED_ARGS})
		if(NOT DEFINED ARGS_${a})
			message(
				FATAL_ERROR "GitVersion:_run missing required ${a} argument"
			)
		endif(NOT DEFINED ARGS_${a})
	endforeach(a ${REQUIRED_ARGS})

	if(NOT DEFINED ARGS_PROJECT_SOURCE)
		message(
			FATAL_ERROR
				"GitVersion:_run missing required PROJECT_SOURCE argument"
		)
	endif(NOT DEFINED ARGS_PROJECT_SOURCE)

	set(git_describe_args)
	if(ARGS_FALLBACK_VERSION)
		set(describe_version ${ARGS_FALLBACK_VERSION})
	else(ARGS_FALLBACK_VERSION)
		set(git_describe_args ENSURE)
	endif(ARGS_FALLBACK_VERSION)
	_git_version_from_git_describe(
		PROJECT_SOURCE
		${ARGS_PROJECT_SOURCE} #
		OUTPUT_VERSION
		describe_version #
		OUTPUT_FULL_VERSION
		describe_full_version
		${git_describe_args}
	)

	if(EXISTS ${ARGS_VERSION_FILE})
		file(READ ${ARGS_VERSION_FILE} version_file_content)

		if(version_file_content STREQUAL
		   "${describe_version} ${describe_full_version}"
		)
			return()
		endif()
	endif(EXISTS ${ARGS_VERSION_FILE})

	file(WRITE ${ARGS_VERSION_FILE}
		 "${describe_version} ${describe_full_version}"
	)

endfunction(_git_version_run)

if(GitVersion_RUN)
	if(NOT DEFINED GitVersion_ARGS)
		message(FATAL_ERROR "GitVersion: GitVersion_ARGS not defined")
	endif(NOT DEFINED GitVersion_ARGS)
	_git_version_run(${GitVersion_ARGS})
endif(GitVersion_RUN)
