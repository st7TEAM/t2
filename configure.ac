#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT(meobm.cpp)
AM_INIT_AUTOMAKE(meobm,0.1)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

#
# Checks for FreeType
#
PKG_CHECK_MODULES(FREETYPE, freetype2, have_freetype=true, AC_MSG_RESULT([no]))

if $have_freetype ; then
  AC_DEFINE(HAVE_FREETYPE, 1, [Have FreeType 2 library])
  # By freetype, we really always mean freetype+fontconfig
  FREETYPE_CFLAGS="$FREETYPE_CFLAGS $FONTCONFIG_CFLAGS"
  FREETYPE_LIBS="$FREETYPE_LIBS $FONTCONFIG_LIBS"
fi
AM_CONDITIONAL(HAVE_FREETYPE, $have_freetype)

AC_OUTPUT(Makefile)
