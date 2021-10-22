cmake_minimum_required(VERSION 3.10)

# meta
project(cavezofphear VERSION 0.5.1)

# depends
find_package(Curses REQUIRED)

include(GNUInstallDirs)

# sources
set(PHEAR_HEADERS
	src/common.h
	src/proto.h
)

set(PHEAR_SOURCES
	src/chk.c
	src/editor.c
	src/frame.c
	src/gplot.c
	src/isready.c
	src/loadmap.c
	src/main.c
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

# install
install(TARGETS phear RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_DATADIR}/phear)
