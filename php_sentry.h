/* sentry extension for PHP */

#ifndef PHP_SENTRY_H
# define PHP_SENTRY_H

#define PHP_5_3_X_API_NO                20090626
#define PHP_5_4_X_API_NO                20100525
#define PHP_5_5_X_API_NO                20121212
#define PHP_5_6_X_API_NO                20131226

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"


typedef void (*php_sentry_orig_error_cb_t)(int, const char *, const uint, const char *, va_list);


ZEND_BEGIN_MODULE_GLOBALS(sentry)
	zend_bool enabled;			/* is the module enabled */



	php_sentry_orig_error_cb_t orig_error_cb;	/* the original zend_error_cb */

	void (*orig_set_error_handler)(INTERNAL_FUNCTION_PARAMETERS);	/* the set_error_handle entry */
	void (*orig_restore_error_handler)(INTERNAL_FUNCTION_PARAMETERS);	/* the restore error handler entry */
	void (*orig_set_exception_handler)(INTERNAL_FUNCTION_PARAMETERS);	/* the restore error handler entry */

	zend_ptr_stack user_error_handlers;		/* previous user error handlers */
	zend_stack user_error_handler_levels;	/* the levels the user error handler handles */

	zval user_error_handler;	/* the current active user error handler */
  zval breadcrumbs;
	zval * last_exception;
	int debug;

	HashTable module_error_reporting;	/* hashtable containing error reporting levels for different storage modules */


ZEND_END_MODULE_GLOBALS(sentry)

ZEND_EXTERN_MODULE_GLOBALS(sentry)


extern zend_module_entry sentry_module_entry;
# define phpext_sentry_ptr &sentry_module_entry

# define PHP_SENTRY_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_SENTRY)
ZEND_TSRMLS_CACHE_EXTERN()
# endif


#ifdef ZTS
#  define SENTRY_G(v) TSRMG(sentry_globals_id, zend_sentry_globals *, v)
#else
#  define SENTRY_G(v) (sentry_globals.v)
#endif

#ifndef MY_SENTRY_EXPORTS
#  ifdef PHP_WIN32
#    define MY_SENTRY_EXPORTS __declspec(dllexport)
#  else
#    define MY_SENTRY_EXPORTS PHPAPI
#  endif
#endif

MY_SENTRY_EXPORTS void php_sentry_original_error_cb(int type TSRMLS_DC, const char *format, ...);


#endif	/* PHP_SENTRY_H */
