cmake_minimum_required(VERSION 3.5)
project(pam_reattach C)

# Add PAM dependency
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
find_package(PAM REQUIRED)

# Remove the lib prefix
set(CMAKE_SHARED_MODULE_PREFIX "")

add_library(pam_reattach MODULE src/xpc.h src/bootstrap.h src/pam_reattach.c)
target_link_libraries(pam_reattach pam)
install(TARGETS pam_reattach DESTINATION lib/pam)
install(FILES man/pam_reattach.8 DESTINATION "share/man/man8")
