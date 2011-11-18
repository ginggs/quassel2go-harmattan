# - Try to find libgq-gconf
# Once done this will define
#
#  GQ_GCONF_FOUND    - system has libgq-gconf Library
#  GQ_GCONF_INCLUDES - the libgq-gconf include directory
#  GQ_GCONF_LIBS     - link these to use libgq-gconf

find_package(PkgConfig)
pkg_check_modules(PC_GQ_GCONF gq-gconf)

find_path(GQ_GCONF_INCLUDE_DIR gconfitem.h
          HINTS ${PC_GQ_GCONF_INCLUDEDIR} ${PC_GQ_GCONF_INCLUDE_DIRS}
          )

find_library(GQ_GCONF_LIBRARY NAMES gq-gconf libgq-gconf
             HINTS ${PC_GQ_GCONF_LIBDIR} ${PC_GQ_GCONF_LIBRARY_DIRS})

set(GQ_GCONF_INCLUDES ${GQ_GCONF_INCLUDE_DIR})
set(GQ_GCONF_LIBS ${GQ_GCONF_LIBRARY})