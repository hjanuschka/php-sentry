/* sentry extension for PHP */

#ifndef PHP_SENTRY_H
# define PHP_SENTRY_H

extern zend_module_entry sentry_module_entry;
# define phpext_sentry_ptr &sentry_module_entry

# define PHP_SENTRY_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_SENTRY)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_SENTRY_H */
