cmake_minimum_required(VERSION 3.10)

# meta
project(cavezofphear VERSION 0.5.3)

# options
OPTION(SYSTEMWIDE "Build for systemwide installation" OFF)

# depends
find_package(Curses REQUIRED)

include(GNUInstallDirs)

# sources
set(PHEAR_HEADERS
	src/chk.h
	src/common.h
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

# install
if(SYSTEMWIDE)
	install(TARGETS phear RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_DATADIR}/phear)
endif()
