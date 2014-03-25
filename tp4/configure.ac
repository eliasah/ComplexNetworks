# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(abouhaydar, 4.0)
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_PROG_MAKE_SET

if test "x$GCC" = "xyes"; then
  CFLAGS="-Wall"
fi

AC_CONFIG_FILES([
        Makefile
        src/Makefile
        ])
AC_OUTPUT
