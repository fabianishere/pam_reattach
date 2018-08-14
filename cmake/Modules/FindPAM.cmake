# - Try to find the pam library
# Once done this will define
#  PAM_FOUND - System has libpam
#  PAM_INCLUDE_DIRS - The libpam include directories
#  PAM_LIBRARIES - The libraries needed to use libpam

find_path(PAM_INCLUDE_DIR security/pam_modules.h PATH_SUFFIXES ..)

set(PAM_DEFINITIONS "-fPIC")

find_library(PAM_LIBRARY NAMES pam)

set(PAM_LIBRARIES ${PAM_LIBRARY})
set(PAM_INCLUDE_DIRS ${PAM_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set PAM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(PAM DEFAULT_MSG PAM_LIBRARY PAM_INCLUDE_DIR)

mark_as_advanced(PAM_INCLUDE_DIR PAM_LIBRARY)

add_library(pam SHARED IMPORTED GLOBAL)
set_property(TARGET pam PROPERTY IMPORTED_LOCATION ${PAM_LIBRARY})
set_property(TARGET pam PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PAM_INCLUDE_DIRS})

