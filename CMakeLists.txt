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

	file(GLOB_RECURSE libgitnote_sources "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c")

	ae2f_CoreLibFetchX_DNS(ae3f ae3f easyredir master codeberg.org)
	add_library(libgitnote ${libgitnote_sources})

	add_executable(gitnote-raw app/main.c ${libgitnote_sources})
	add_executable(ae3f::gitnote-raw ALIAS gitnote-raw)

	target_link_libraries(gitnote-raw		PRIVATE ae3f::libeasyredir)
	target_link_libraries(libgitnote	PRIVATE ae3f::libeasyredir)

	target_include_directories(gitnote-raw	PRIVATE inc)
	target_include_directories(gitnote-raw	PRIVATE lib)
	target_include_directories(libgitnote	PRIVATE lib)
	target_include_directories(libgitnote	PRIVATE inc)

	ae2f_UltraErrWERROR(gitnote-raw PRIVATE)
	ae2f_UltraErrWERROR(libgitnote PRIVATE)

	#######################################################################

	set(GITNOTE_VENV_DIR	${CMAKE_BINARY_DIR}/${ae2f_submod}/.venv/python)
	set(GITNOTE_PYTHON	python)

	set(BINDIR		${GITNOTE_VENV_DIR}/bin)
	set(PIP			${BINDIR}/pip)
	set(PYINSTALLER		${BINDIR}/pyinstaller)

	set(CMAKEDIR		cmake -E make_directory)
	set(CMOVEDIR		cmake -E copy_directory)
	set(PYSRCDIR		${CMAKE_CURRENT_SOURCE_DIR}/py)
	set(PYBINDIR		${CMAKE_CURRENT_BINARY_DIR}/dist)

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


		message(STATUS ${PIP} install -q notion-client pyinstaller markdown2)
		execute_process(
			COMMAND ${PIP} install -q notion-client pyinstaller markdown2
			)
	endif()

	add_custom_command(
		COMMAND ${PYINSTALLER} ${PYFLAG} --name=get_snap ${PYSRCDIR}/get_snap.py
		OUTPUT			${PYBINDIR}/get_snap
		DEPENDS			${PYSRCDIR}/get_snap.py
		)

	add_custom_command(
		COMMAND ${PYINSTALLER} ${PYFLAG} --name=set_snap ${PYSRCDIR}/set_snap.py
		OUTPUT			${PYBINDIR}/set_snap
		DEPENDS			${PYSRCDIR}/set_snap.py
		)

	add_custom_command(
		COMMAND ${PYINSTALLER} ${PYFLAG} --name=mkdir ${PYSRCDIR}/mkdir.py
		OUTPUT			${PYBINDIR}/mkdir
		DEPENDS			${PYSRCDIR}/mkdir.py
		)

	add_custom_command(
		COMMAND ${PYINSTALLER} ${PYFLAG} --name=mkdir ${PYSRCDIR}/rmdir.py
		OUTPUT			${PYBINDIR}/rmdir
		DEPENDS			${PYSRCDIR}/rmdir.py
		)

	add_custom_command(
		COMMAND ${PYINSTALLER} ${PYFLAG} --name=modify_file ${PYSRCDIR}/modify_file.py
		OUTPUT			${PYBINDIR}/modify_file
		DEPENDS			${PYSRCDIR}/modify_file.py
		)

	add_custom_command(
		COMMAND ${PYINSTALLER} ${PYFLAG} --name=touch ${PYSRCDIR}/touch.py
		OUTPUT			${PYBINDIR}/touch
		DEPENDS			${PYSRCDIR}/touch.py
		)

	add_custom_command(
		COMMAND ${PYINSTALLER} ${PYFLAG} --name=rm ${PYSRCDIR}/rm.py
		OUTPUT			${PYBINDIR}/rm
		DEPENDS			${PYSRCDIR}/rm.py
		)

	add_custom_target(
		gitnote-py
		COMMAND	${CMAKEDIR} ${CMAKE_CURRENT_BINARY_DIR}/dist
		DEPENDS
		${PYBINDIR}/get_snap
		${PYBINDIR}/set_snap
		${PYBINDIR}/mkdir
		${PYBINDIR}/rmdir
		${PYBINDIR}/modify_file
		${PYBINDIR}/touch
		${PYBINDIR}/rm
		)

	add_dependencies(gitnote-raw gitnote-py)
endif()
