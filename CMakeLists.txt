cmake_minimum_required(VERSION 3.10)
project(gitnote)

# Set this source directory as the tree root for project.
set(ae2f_ProjRoot ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Tree root for project")
set(ae2f_submod  .submod CACHE STRING "Relative path to submodule (subdirectory)")
set(ae2f_CoreScript_Wh ${ae2f_ProjRoot}/${ae2f_submod}/ae2f/CoreScript)

# add subdirectory
if(NOT TARGET ae2f::CoreScript)
	# Execute git to fetch CoreScript when repository does not exist
	if(NOT EXISTS ${ae2f_CoreScript_Wh})
		execute_process(
			COMMAND git clone https://codeberg.org/ae2f/CoreScript
			${ae2f_CoreScript_Wh}
			)
	endif()
	add_subdirectory(${ae2f_CoreScript_Wh} ${ae2f_submod}/ae2f/CoreScript)
endif()

if(NOT TARGET ae3f::gitnote)
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

if(NOT TARGET ae3f::gitnote)
	if(GITNOTE_SHARED)
		set(GITNOTE_SHARED 1)
	else()
		set(GITNOTE_SHARED 0)
	endif()

	set(GITNOTE_HASH_LENGTH 64)
	set(GITNOTE_TMPFILE_NAME ".gitnote")

	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/inc/gitnote/link.h.in
		${CMAKE_CURRENT_SOURCE_DIR}/inc/gitnote/.link.h
		)

	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/lib/cfg/HASH_LENGTH.h.in
		${CMAKE_CURRENT_SOURCE_DIR}/lib/cfg/.HASH_LENGTH.h
		)

	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/lib/cfg/TMPFILE_NAME.h.in
		${CMAKE_CURRENT_SOURCE_DIR}/lib/cfg/.TMPFILE_NAME.h
		)

	ae2f_CoreLibFetchX_DNS(ae3f ae3f easyredir master codeberg.org)
	
	#######################################################################

	set(GITNOTE_VENV_DIR	${CMAKE_BINARY_DIR}/${ae2f_submod}/.venv/python)
	set(GITNOTE_PYTHON	python)

	set(BINDIR		${GITNOTE_VENV_DIR}/bin)
	set(PIP			${BINDIR}/pip)
	set(PYINSTALLER		${BINDIR}/pyinstaller)

	set(CMAKEDIR		cmake -E make_directory)
	set(CMOVEDIR		cmake -E copy_directory)
	set(PYSRCDIR		${CMAKE_CURRENT_SOURCE_DIR}/py)
	set(PYBINDIR		${CMAKE_CURRENT_BINARY_DIR})

	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(PYFLAG	--onefile --console --strip)
	else()
		set(PYFLAG	--onefile --console)
	endif()

	if(NOT EXISTS ${GITNOTE_VENV_DIR})
		message(STATUS "${GITNOTE_VENV_DIR} is not present.")

		message(STATUS ${GITNOTE_PYTHON} -m venv ${GITNOTE_VENV_DIR})
		execute_process(
			COMMAND ${GITNOTE_PYTHON} -m venv ${GITNOTE_VENV_DIR}
			)


		message(STATUS ${PIP} install -q notion-client pyinstaller markdown2 cython)
		execute_process(
			COMMAND ${PIP} install -q notion-client pyinstaller markdown2 cython
			)
	endif()

	execute_process(COMMAND ${CMAKEDIR} ${PYBINDIR})

	add_custom_target(
		gitnote-py ALL
		COMMAND	${CMAKE_COMMAND} -E copy ${PYSRCDIR}/utils.pyx ${PYBINDIR}/
		COMMAND	${CMAKE_COMMAND} -E env TARDIR=${PYBINDIR}/ ${GITNOTE_VENV_DIR}/bin/python ${PYSRCDIR}/setup.py 
		build_ext --inplace
		COMMAND	${CMAKE_COMMAND} -E rename ${PYBINDIR}/utils.c ${PYBINDIR}/utils.h
		
		WORKING_DIRECTORY	${PYBINDIR}	
		SOURCES
		${PYSRCDIR}/setup.py
		${PYSRCDIR}/utils.pyx

		BYPRODUCTS
		${PYBINDIR}/utils.pyx
		${PYBINDIR}/utils.so
		${PYBINDIR}/utils.h

		VERBATIM
		)

	#######################################################################

	file(GLOB_RECURSE libgitnote_sources "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c")
	file(GLOB_RECURSE gluegitnote_sources "${CMAKE_CURRENT_SOURCE_DIR}/pyglue/*.c")
	add_library(gitnote-common INTERFACE)
	add_library(libgitnote ${libgitnote_sources} ${PYBINDIR}/utils.h)
	add_library(gluegitnote ${gluegitnote_sources} ${PYBINDIR}/utils.h)

	add_executable(gitnote-raw app/main.c ${libgitnote_sources})
	add_executable(ae3f::gitnote-raw ALIAS gitnote-raw)

	target_link_libraries(gitnote-common		INTERFACE ae3f::libeasyredir)
	target_link_libraries(gitnote-raw PRIVATE libgitnote)
	target_link_libraries(libgitnote PRIVATE gluegitnote)

	target_include_directories(gitnote-common	INTERFACE inc)
	target_include_directories(gitnote-common	INTERFACE lib)

	target_include_directories(gluegitnote	PRIVATE	${CMAKE_CURRENT_BINARY_DIR})
	target_include_directories(gluegitnote	PRIVATE ${Python3_INCLUDE_DIRS})
	target_link_libraries(gluegitnote	PRIVATE ${Python3_LIBRARIES})
	target_link_libraries(libgitnote	PUBLIC gitnote-common)
	target_link_libraries(gluegitnote	PUBLIC gitnote-common)
	target_link_libraries(gitnote-raw	PUBLIC gitnote-common)

	ae2f_UltraErrWERROR(gitnote-raw PRIVATE)
	ae2f_UltraErrWERROR(libgitnote PRIVATE)

	#######################################################################

	add_dependencies(gitnote-common gitnote-py)
endif()
