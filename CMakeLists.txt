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
endif()
