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

 	PHP_ARG_WITH(curl, for cURL support, [  --with-curl[=DIR]		SOLR : libcurl install prefix])

		if test -r $PHP_CURL/include/curl/easy.h; then
			CURL_DIR=$PHP_CURL
			AC_MSG_RESULT(curl headers found in $PHP_CURL)
		else
			AC_MSG_CHECKING(for cURL in default path)
			for i in /usr/local /usr /usr/include/x86_64-linux-gnu/; do
	  		if test -r $i/include/curl/easy.h; then
					CURL_DIR=$i
					AC_MSG_RESULT(found in $i)
					break
	  		fi
			done
		fi


CURL_CONFIG="curl-config"
AC_MSG_CHECKING(for cURL 7.15.0 or greater)

if ${CURL_DIR}/bin/curl-config --libs > /dev/null 2>&1; then
	CURL_CONFIG=${CURL_DIR}/bin/curl-config
else
	if ${CURL_DIR}/curl-config --libs > /dev/null 2>&1; then
  		CURL_CONFIG=${CURL_DIR}/curl-config
	fi
fi

curl_version_full=`$CURL_CONFIG --version`
curl_version=`echo ${curl_version_full} | sed -e 's/libcurl //' | $AWK 'BEGIN { FS = "."; } { printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`

if test "$curl_version" -ge 7015000; then
	AC_MSG_RESULT($curl_version_full)
	CURL_LIBS=`$CURL_CONFIG --libs`
else
	AC_MSG_ERROR([Curl is required for sentry extension])
fi

    PHP_CHECK_LIBRARY(curl,curl_easy_perform,
    [
        AC_DEFINE(HAVE_CURL,1,[ ])
    ],[
        AC_MSG_ERROR(There is something wrong. Please check config.log for more information.)
    ],[
        $CURL_LIBS -L$CURL_DIR/$PHP_LIBDIR
    ])

    PHP_ADD_INCLUDE($CURL_DIR/include)
    PHP_EVAL_LIBLINE($CURL_LIBS, SENTRY_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(curl, $CURL_DIR/lib, SENTRY_SHARED_LIBADD)
		PHP_SUBST(SENTRY_SHARED_LIBADD)



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
  PHP_ADD_EXTENSION_DEP(sentry, json)
fi
