dnl $Id$
dnl config.m4 for extension sentry

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(sentry, for sentry support,
dnl Make sure that the comment is aligned:
dnl [  --with-sentry             Include sentry support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(sentry, whether to enable sentry support,
dnl Make sure that the comment is aligned:
[  --enable-sentry          Enable sentry support], no)

if test "$PHP_SENTRY" != "no"; then
  dnl Write more examples of tests here...

  dnl # get library FOO build options from pkg-config output
  dnl AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  dnl AC_MSG_CHECKING(for libfoo)
  dnl if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists foo; then
  dnl   if $PKG_CONFIG foo --atleast-version 1.2.3; then
  dnl     LIBFOO_CFLAGS=\`$PKG_CONFIG foo --cflags\`
  dnl     LIBFOO_LIBDIR=\`$PKG_CONFIG foo --libs\`
  dnl     LIBFOO_VERSON=\`$PKG_CONFIG foo --modversion\`
  dnl     AC_MSG_RESULT(from pkgconfig: version $LIBFOO_VERSON)
  dnl   else
  dnl     AC_MSG_ERROR(system libfoo is too old: version 1.2.3 required)
  dnl   fi
  dnl else
  dnl   AC_MSG_ERROR(pkg-config not found)
  dnl fi
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, SENTRY_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-sentry -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/sentry.h"  # you most likely want to change this
  dnl if test -r $PHP_SENTRY/$SEARCH_FOR; then # path given as parameter
  dnl   SENTRY_DIR=$PHP_SENTRY
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for sentry files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       SENTRY_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$SENTRY_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the sentry distribution])
  dnl fi

  dnl # --with-sentry -> add include path
  dnl PHP_ADD_INCLUDE($SENTRY_DIR/include)

  dnl # --with-sentry -> check for lib and symbol presence
  dnl LIBNAME=SENTRY # you may want to change this
  dnl LIBSYMBOL=SENTRY # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SENTRY_DIR/$PHP_LIBDIR, SENTRY_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_SENTRYLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong sentry lib version or lib not found])
  dnl ],[
  dnl   -L$SENTRY_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(SENTRY_SHARED_LIBADD)

  dnl # In case of no dependencies
  AC_DEFINE(HAVE_SENTRY, 1, [ Have sentry support ])

  PHP_NEW_EXTENSION(sentry, sentry.c, $ext_shared)
fi
