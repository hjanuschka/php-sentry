/* sentry extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_sentry.h"
#include "Zend/zend_builtin_functions.h"
#include "ext/standard/php_string.h"
#include "zend.h"
#include "zend_ini_scanner.h"
#include "zend_language_scanner.h"
#include <zend_language_parser.h>
#include "ext/standard/php_var.h"
#include "zend_exceptions.h"

ZEND_DECLARE_MODULE_GLOBALS(sentry)

/* {{{ void sentry_test1()
 */
PHP_FUNCTION(sentry_test1)
{
	zend_parse_parameters_none();

	php_printf("The extension %s is loaded and working!\r\n", "sentry");
}
/* }}} */

/* {{{ string sentry_test2( [ string $var ] )
 */
PHP_FUNCTION(sentry_test2)
{
	char *var = "World";
	size_t var_len = sizeof("World") - 1;
	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(var, var_len)
	ZEND_PARSE_PARAMETERS_END();

	retval = strpprintf(0, "Hello %s", var);

	RETURN_STR(retval);
}
/* }}}*/


PHP_FUNCTION(sentry_restore_error_handler)
{
	if (SENTRY_G(orig_restore_error_handler)) {
		SENTRY_G(orig_restore_error_handler)(INTERNAL_FUNCTION_PARAM_PASSTHRU);

		if (&SENTRY_G(user_error_handler)) {
			zval_ptr_dtor(&SENTRY_G(user_error_handler));
			//FREE_ZVAL(SENTRY_G(user_error_handler));
		}

		/* Delete the top element from our stack */
		if (zend_ptr_stack_num_elements(&SENTRY_G(user_error_handlers)) > 0) {
			zval *tmp = (zval *)zend_ptr_stack_pop(&SENTRY_G(user_error_handlers));
			zval_dtor(tmp);
			//efree(&tmp);
			tmp = NULL;

			if (zend_ptr_stack_num_elements(&SENTRY_G(user_error_handlers)) > 0) {
				tmp = (zval *)zend_ptr_stack_pop(&SENTRY_G(user_error_handlers));
				zend_ptr_stack_push(&SENTRY_G(user_error_handlers), tmp);

				if (&SENTRY_G(user_error_handler)) {
					zval_ptr_dtor(&SENTRY_G(user_error_handler));
					//FREE_ZVAL(SENTRY_G(user_error_handler));
				}
				ZVAL_STRING(&SENTRY_G(user_error_handler), Z_STRVAL_P(tmp));
			}
		}
	}
}


PHP_FUNCTION(sentry_set_error_handler)
{
	if (SENTRY_G(orig_set_error_handler)) {
		SENTRY_G(orig_set_error_handler)(INTERNAL_FUNCTION_PARAM_PASSTHRU);

		/* Take the user error handler and push to our stack */
		if (&EG(user_error_handler)) {
			zval tmp;

			/* Override the error handler with our callback */
			if (!strcmp(Z_STRVAL_P(&EG(user_error_handler)), "__sentry_error_handler_callback")) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Do not call set_error_handler(\"__sentry_error_handler_callback\")");
			}

			if (zend_ptr_stack_num_elements(&SENTRY_G(user_error_handlers)) > 0) {
				zval *old_handler = (zval *)zend_ptr_stack_pop(&SENTRY_G(user_error_handlers));
				zend_ptr_stack_push(&SENTRY_G(user_error_handlers), old_handler);

				zval_dtor(return_value);
				ZVAL_STRING(return_value, Z_STRVAL_P(old_handler));
			}

			ZVAL_STRING(&tmp, Z_STRVAL_P(&EG(user_error_handler)));


			ZVAL_STRING(&SENTRY_G(user_error_handler), Z_STRVAL_P(&EG(user_error_handler)));

			/* Create a new handler */
			zend_ptr_stack_push(&SENTRY_G(user_error_handlers), &tmp);

			zval_dtor(&EG(user_error_handler));
			ZVAL_STRING(&EG(user_error_handler), "__sentry_error_handler_callback");
		} else {
			zval_ptr_dtor(&SENTRY_G(user_error_handler));
			//efree(&SENTRY_G(user_error_handler));
		}
	}
}

/* event must be initialized with MAKE_STD_ZVAL or similar and array_init before sending here */
void php_sentry_capture_error_ex(zval *event, int type, const char *error_filename, const uint error_lineno, zend_bool free_event, const char *format, va_list args TSRMLS_DC)
{
	zval **ppzval;
	va_list args_cp;
	int len;
	char *buffer;
	char uuid_str[200 + 1];

	TSRMLS_FETCH();



	/* Capture backtrace */
		zval btrace;
// TODO: introduce a directive for the amount of stack frames returned instead of hard coded 1000?
		zend_fetch_debug_backtrace(&btrace, 0, 0 ,1000);
		add_assoc_zval(event, "backtrace", &btrace);

	va_copy(args_cp, args);
	len = vspprintf(&buffer, PG(log_errors_max_len), format, args_cp);
	va_end(args_cp);

	add_assoc_string(event,	"error_message", buffer);
	add_assoc_string(event,	"filename",	(char *)error_filename);

	add_assoc_long(event, "line_number", error_lineno);
	add_assoc_long(event, "error_type", type);

zend_long lkey;
    zend_string *skey;
    zval *val;

		zend_class_entry *default_ce, *exception_ce;
	/* Send to backend */
    //SEND ERROR HERE
   	php_printf("SENTRY PHP-EXT Catched:\n");
   	php_printf("==============\n");
		if(SENTRY_G(last_exception)) {

		zval *emsg, rv1;

		default_ce = Z_OBJCE_P(SENTRY_G(last_exception));
		exception_ce =  Z_OBJCE_P(SENTRY_G(last_exception));
		emsg =    zend_read_property(default_ce, SENTRY_G(last_exception), "message",    sizeof("message")-1,    0 TSRMLS_CC, &rv1);


			php_printf("message: Unkown Exception %s catched!\n", Z_STRVAL_P(emsg));
		} else {
			php_printf("message: %s\n", buffer);
		}

	php_printf("\tFrame(0):\n");
	php_printf("\t\tfile: %s\n", error_filename);
	php_printf("\t\tlineo: %d\n", error_lineno);
	php_printf("\t\ttype: %d\n", type);


	//TODO if global backtrace is available take this
    HashTable *hash_arr = Z_ARRVAL(btrace);





	if(SENTRY_G(last_exception)) {
		zval *trace, rv;

		default_ce = Z_OBJCE_P(SENTRY_G(last_exception));
		exception_ce =  Z_OBJCE_P(SENTRY_G(last_exception));
		trace =    zend_read_property(default_ce, SENTRY_G(last_exception), "trace",    sizeof("trace")-1,    0 TSRMLS_CC, &rv);
		hash_arr = Z_ARRVAL_P(trace);

	}

    zval *ele_value = NULL;

	int frame = 1;
	ZEND_HASH_FOREACH_VAL(hash_arr,  ele_value) {
		zval * file, * lineo, *function, *class;
		file = zend_hash_str_find(Z_ARRVAL_P(ele_value), "file", sizeof("file")-1);
		lineo = zend_hash_str_find(Z_ARRVAL_P(ele_value), "line", sizeof("line")-1);
		class = zend_hash_str_find(Z_ARRVAL_P(ele_value), "class", sizeof("class")-1);
		function = zend_hash_str_find(Z_ARRVAL_P(ele_value), "function", sizeof("function")-1);
		php_printf("\tFrame(%d):\n", frame);
		php_printf("\t\t file: %s\n", Z_STRVAL_P(file));
		php_printf("\t\t lineo: %ld\n", Z_LVAL_P(lineo));
		php_printf("\t\t class: %s\n", Z_STRVAL_P(class));
		php_printf("\t\t function: %s\n", Z_STRVAL_P(function));
		frame++;
    }
    ZEND_HASH_FOREACH_END();

   	php_printf("/SENTRY PHP-EXT Catched:\n");

	//(uuid_str, event, type, error_filename, error_lineno TSRMLS_CC);

	if (free_event) {
		zval_dtor(event);
		//efree(event);
	}
}

static void php_sentry_exception_hook(zval *exception TSRMLS_DC)
{
	//TODO keep track of exception - those are the unhandled ones
    zend_class_entry *exception_ce;
    zval *code_val;
    zval **name_maps_data;
    long code;
    const char *exception_name;
    uint exception_name_len = 0;
	do {
		if(!exception) {
			break;
		}
		SENTRY_G(last_exception) = exception;
		/*
		zval *message, *file, *line,*trace, *xdebug_message_trace, *previous_exception, rv;

		php_printf("### Sentry Unhandled Exception\n");
		zend_class_entry *default_ce, *exception_ce;

		default_ce = Z_OBJCE_P(exception);
		exception_ce = Z_OBJCE_P(exception);

        message = zend_read_property(default_ce, exception, "message", sizeof("message")-1, 0 TSRMLS_CC, &rv);
	    file =    zend_read_property(default_ce, exception, "file",    sizeof("file")-1,    0 TSRMLS_CC, &rv);
		line =    zend_read_property(default_ce, exception, "line",    sizeof("line")-1,    0 TSRMLS_CC, &rv);
		trace =    zend_read_property(default_ce, exception, "trace",    sizeof("trace")-1,    0 TSRMLS_CC, &rv);
		php_var_dump(trace, 0);
		zval * ele_value;
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(trace),  ele_value) {
			php_var_dump(ele_value, 0);
		}
		ZEND_HASH_FOREACH_END();
		*/




	}while(0);
}

/* Wrapper that calls the original callback or our callback */
void php_sentry_capture_error(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
	    TSRMLS_FETCH();
		zval event;



		array_init(&event);

		php_sentry_capture_error_ex(&event, type, error_filename, error_lineno, 1, format, args TSRMLS_CC);
		SENTRY_G(orig_error_cb)(type, error_filename, error_lineno, format, args);
}

static void php_sentry_override_error_handling(TSRMLS_D)
{
	zend_function *orig_set_error_handler, *orig_restore_error_handler;

	SENTRY_G(orig_error_cb) = zend_error_cb;
	zend_error_cb          = php_sentry_capture_error;
	zend_throw_exception_hook = php_sentry_exception_hook;

	if ((orig_set_error_handler = (void **)zend_hash_str_find(EG(function_table), "set_error_handler", sizeof("set_error_handler")-1)) != NULL) {
		SENTRY_G(orig_set_error_handler) = orig_set_error_handler->internal_function.handler;
		orig_set_error_handler->internal_function.handler = zif_sentry_set_error_handler;
	}
	if ((orig_restore_error_handler = zend_hash_str_find(EG(function_table),"restore_error_handler", sizeof("restore_error_handler") -1 )) != NULL) {
		SENTRY_G(orig_restore_error_handler) = orig_restore_error_handler->internal_function.handler;
		orig_restore_error_handler->internal_function.handler = zif_sentry_restore_error_handler;
	}

	zend_ptr_stack_init(&SENTRY_G(user_error_handlers));
}

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(sentry)
{
#if defined(ZTS) && defined(COMPILE_DL_SENTRY)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
    php_sentry_override_error_handling(TSRMLS_C);


	return SUCCESS;
}

PHP_GINIT_FUNCTION(sentry)
{
	sentry_globals->enabled	   		= 1;



}

/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(sentry)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "sentry support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_sentry_test1, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_sentry_test2, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ sentry_functions[]
 */
static const zend_function_entry sentry_functions[] = {
	PHP_FE(sentry_test1,		arginfo_sentry_test1)
	PHP_FE(sentry_test2,		arginfo_sentry_test2)
	PHP_FE_END
};
/* }}} */

/* {{{ sentry_module_entry
 */
zend_module_entry sentry_module_entry = {
	STANDARD_MODULE_HEADER,
	"sentry",					/* Extension name */
	sentry_functions,			/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(sentry),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(sentry),			/* PHP_MINFO - Module info */
	PHP_SENTRY_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SENTRY
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(sentry)
#endif
