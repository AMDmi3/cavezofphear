cmake_minimum_required(VERSION 3.10)

# meta
project(cavezofphear VERSION 0.5.3)

# options
option(SYSTEMWIDE "Build for systemwide installation" OFF)
option(WITH_MANPAGE "Build manual page (requires help2man)" ON)

# depends
find_package(Curses REQUIRED)
if(WITH_MANPAGE)
	find_program(HELP2MAN NAMES help2man REQUIRED)
endif()

include(GNUInstallDirs)

# sources
set(PHEAR_HEADERS
	src/chk.h
	src/editor.h
	src/frame.h
	src/game.h
	src/gplot.h
	src/isready.h
	src/map/map.h
	src/misc.h
	src/splash.h
)

set(PHEAR_SOURCES
	src/chk.c
	src/editor.c
	src/frame.c
	src/game.c
	src/gplot.c
	src/isready.c
	src/main.c
	src/map/count.c
	src/map/draw.c
	src/map/saveload.c
	src/map/state.c
	src/misc.c
	src/splash.c
)

# targets
add_executable(phear ${PHEAR_HEADERS} ${PHEAR_SOURCES})
target_link_libraries(phear ${CURSES_LIBRARIES})
target_include_directories(phear SYSTEM PRIVATE ${CURSES_INCLUDE_DIRS})
target_compile_definitions(phear PRIVATE -DVERSION="${CMAKE_PROJECT_VERSION}")

if(NOT MSVC)
	target_compile_options(phear PRIVATE -Wall -Wextra -pedantic)
endif()

if(SYSTEMWIDE)
	target_compile_definitions(phear PRIVATE -DDATADIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/phear")
else()
	target_compile_definitions(phear PRIVATE -DDATADIR="${PROJECT_SOURCE_DIR}")
endif()

if(WITH_MANPAGE)
	add_custom_command(
		OUTPUT phear.1
		COMMAND
			${HELP2MAN}
				--output=phear.1
				--no-info
				--name="Boulder Dash / Digger-like game for consoles/terminals"
				--include ${CMAKE_CURRENT_SOURCE_DIR}/doc/include.help2man
				${CMAKE_CURRENT_BINARY_DIR}/phear
		DEPENDS
			phear
			#${CMAKE_CURRENT_SOURCE_DIR}/doc/include.help2man
			doc/include.help2man
		COMMENT "Generating manual page"
	)

	add_custom_target(man ALL DEPENDS phear.1)
endif()

# install
if(SYSTEMWIDE)
	install(TARGETS phear RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_DATADIR}/phear)

	if(WITH_MANPAGE)
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/phear.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
	endif()
endif()
