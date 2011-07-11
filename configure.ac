#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([libpbar.so], [0.1])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create])
AC_PROG_LIBTOOL
AC_LANG_CPLUSPLUS

# Checks for header files.
AC_CHECK_HEADERS([sys/ioctl.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for C++-compilable GTK+
AM_PATH_GTK_2_0_CPP

# Check terminal color support
AM_TERM_COLOR

CPPFLAGS="$CPPFLAGS -I../include"

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

echo
echo
echo "   libpbar configured with the following options:"
echo 
echo "     terminal               $term_check"
GV="$gtk_config_major_version.$gtk_config_minor_version.$gtk_config_micro_version"
echo "     GTK+ version           $GV"
echo
echo
