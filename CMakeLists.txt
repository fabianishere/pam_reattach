cmake_minimum_required(VERSION 3.5)
project(reattach C)

option(ENABLE_PAM "Build PAM module that reattaches on authentication" ON)
option(ENABLE_CLI "Build CLI application that reattaches" OFF)

add_library(reattach include/reattach.h src/reattach.c src/xpc.h src/bootstrap.h)
target_include_directories(reattach PUBLIC include)
set_target_properties(reattach PROPERTIES
	PUBLIC_HEADER "include/reattach.h"
	C_STANDARD 90
)
install(
	TARGETS reattach
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	PUBLIC_HEADER DESTINATION include/
)
install(FILES man/reattach_aqua.3 DESTINATION "share/man/man3")

if (ENABLE_PAM)
	# Locate PAM dependency
	set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
	find_package(PAM REQUIRED)

	add_library(pam_reattach MODULE src/pam.c)
	set_target_properties(pam_reattach PROPERTIES PREFIX "") # Remove lib prefix
	target_link_libraries(pam_reattach reattach pam)
	install(TARGETS pam_reattach DESTINATION lib/pam)
	install(FILES man/pam_reattach.8 DESTINATION "share/man/man8")
endif()

if (ENABLE_CLI)
	add_executable(reattach-to-session-namespace src/cli.c)
	target_link_libraries(reattach-to-session-namespace reattach)
	install(TARGETS reattach-to-session-namespace DESTINATION bin)
	install(FILES man/reattach-to-session-namespace.8 DESTINATION "share/man/man8")
endif()
