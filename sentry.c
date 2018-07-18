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
#include "ext/json/php_json.h"
#include "zend_ini_scanner.h"
#include "zend_language_scanner.h"
#include <zend_language_parser.h>
#include "ext/standard/php_var.h"
#include "zend_exceptions.h"

#include <curl/curl.h>
#include <time.h>


struct MemoryStruct {
  char *memory;
  size_t size;
};
 


ZEND_DECLARE_MODULE_GLOBALS(sentry)



PHP_INI_BEGIN()
PHP_INI_ENTRY("sentry.debug", "0", PHP_INI_ALL, NULL)
PHP_INI_END()

/*CURL DEBUG*/
/*CURL DEBUG */


PHP_METHOD(SentryNative, test) /* {{{ */
{
    RETURN_STRING("Hello World");
}

PHP_METHOD(SentryNative, addBreadCrumb) /* {{{ */
{
    zval * message;
    zval * category;
    ZEND_PARSE_PARAMETERS_START(2,2)
      Z_PARAM_ZVAL(message)
      Z_PARAM_ZVAL(category)   
    ZEND_PARSE_PARAMETERS_END(); 


     time_t ctimer;
     zval crumb;
     char ctimebuff[100];
     struct tm* ctm_info;
     time(&ctimer);
     ctm_info = gmtime(&ctimer);
     strftime(ctimebuff, 99, "%Y-%m-%dT%H:%M:%S", ctm_info);


     object_init(&crumb);



    convert_to_string(message);
    convert_to_string(category);

     add_property_string(&crumb, "timestamp", ctimebuff);
     add_property_string(&crumb, "message", Z_STRVAL_P(message));
     add_property_string(&crumb, "category", Z_STRVAL_P(category));
     add_next_index_zval(&SENTRY_G(breadcrumbs), &crumb);
 

    RETURN_STRING("Hello World");
}



zend_class_entry *sentry_class;
const zend_function_entry sentrynative_functions[] = {
  PHP_ME(SentryNative, test, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_ME(SentryNative, addBreadCrumb, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_FE_END
};

/* {{{ string sentry_test2( [ string $var ] )
 */
PHP_FUNCTION(sentry_enable_debug)
{
	zend_bool value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_BOOL(value)
	ZEND_PARSE_PARAMETERS_END();

	if(value) {
		SENTRY_G(debug) = 1;
	} else {
		SENTRY_G(debug) = 0;
	}


	RETURN_BOOL(value);
}

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
      /* out of memory! */ 
      printf("not enough memory (realloc returned NULL)\n");
      return 0;
    }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

void sentry_send_event(zval * event) {

  smart_str   buff = {0};
  int options = PHP_JSON_PRETTY_PRINT;
  php_json_encode(&buff, event, options TSRMLS_CC);


	CURL *curl;
  CURLcode res;
  struct MemoryStruct output;
  output.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  output.size = 0;    /* no data at this point */ 
     
	struct curl_slist *chunk = NULL;

    curl_global_init(CURL_GLOBAL_ALL);
    
   curl = curl_easy_init();
	
	curl_slist_append(chunk, "Expect:");
  curl_slist_append(chunk, "Accept: */*");
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.54.0");
  curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
  curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	curl_easy_setopt(curl, CURLOPT_URL, "https://sentry.krone.at/api/15/store/?sentry_version=7&sentry_client=php-sentry-native%2F1.0&sentry_key=5938e51a1d9f41ff990136203c127a1a");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS,ZSTR_VAL(buff.s));
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&output);



if(sentry_debugging_enabled() == 1) {
  //printf("###################\n");
  //printf("%s\n", ZSTR_VAL(buff.s));
  //printf("###################\n");
}

  res = curl_easy_perform(curl);

  //TODO register sentry id in the NativeSentry->get_last_id()
  //php_printf("%s", output.memory);
  /* always cleanup */ 
  curl_easy_cleanup(curl);
  curl_global_cleanup();


}
PHP_FUNCTION(sentry_send_sample)
{
  
  return;
	CURL *curl;
  CURLcode res;
	struct curl_slist *chunk = NULL;

    curl_global_init(CURL_GLOBAL_ALL);
    
   curl = curl_easy_init();
	
	curl_slist_append(chunk, "Expect:");
  curl_slist_append(chunk, "Accept: */*");
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.54.0");
  curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
  curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	curl_easy_setopt(curl, CURLOPT_URL, "https://sentry.krone.at/api/15/store/?sentry_version=7&sentry_client=raven-js%2F3.26.2&sentry_key=5938e51a1d9f41ff990136203c127a1a");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_POST, 1L);

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"project\":\"15\",\"logger\":\"javascript\",\"platform\":\"javascript\",\"request\":{\"headers\":{\"User-Agent\":\"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36\",\"Referer\":\"http://jsfiddle.net/p6GS8/591/\"},\"url\":\"http://fiddle.jshell.net/_display/\"},\"exception\":{\"values\":[{\"type\":\"Error\",\"value\":\"test\",\"stacktrace\":{\"frames\":[{\"filename\":\"http://fiddle.jshell.net/_display/\",\"lineno\":55,\"colno\":9,\"function\":\"?\",\"in_app\":true}]}}],\"mechanism\":{\"type\":\"generic\",\"handled\":true}},\"transaction\":\"http://fiddle.jshell.net/_display/\",\"trimHeadFrames\":0,\"extra\":{\"session:duration\":5},\"event_id\":\"1e28ef657d604828bfad34d595b47efb\"}");


  res = curl_easy_perform(curl);

  /* always cleanup */ 
  curl_easy_cleanup(curl);
  curl_global_cleanup();
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

int sentry_debugging_enabled() {
  
  if(SENTRY_G(debug) == 1) {
      return 1;
  }
  if(INI_BOOL("sentry.debug")) {
    return 1;
  }
  return 0;
}

char * php_error_type_to_char(type) {
      char *error_type_str;
      switch (type) {
        case E_ERROR:
        case E_CORE_ERROR:
        case E_COMPILE_ERROR:
        case E_USER_ERROR:
          error_type_str = "Fatal error";
          break;
        case E_RECOVERABLE_ERROR:
          error_type_str = "Recoverable fatal error";
          break;
        case E_WARNING:
        case E_CORE_WARNING:
        case E_COMPILE_WARNING:
        case E_USER_WARNING:
          error_type_str = "Warning";
          break;
        case E_PARSE:
          error_type_str = "Parse error";
          break;
        case E_NOTICE:
        case E_USER_NOTICE:
          error_type_str = "Notice";
          break;
        case E_STRICT:
          error_type_str = "Strict Standards";
          break;
        case E_DEPRECATED:
        case E_USER_DEPRECATED:
          error_type_str = "Deprecated";
          break;
        default:
          error_type_str = "Unknown error";
          break;
      }
      return error_type_str;
}

char * get_line_of_file(const char * file, int line) {
  char * response = malloc(sizeof(char)*1024);
  FILE* fh = fopen(file, "r");
  if(!fh) {
    return NULL;
  }
  char current_line[1024];
  int linnr=1;
  while(fgets(current_line, sizeof(current_line), fh)) {
    if(linnr == line) {
      snprintf(response, 1024, "%s", current_line);
      break;
    }
    linnr++;
  }
  return response;
}

/* event must be initialized with MAKE_STD_ZVAL or similar and array_init before sending here */
void php_sentry_capture_error_ex(zval *event, int type, const char *error_filename, const uint error_lineno, zend_bool free_event, const char *format, va_list args TSRMLS_DC)
{
	va_list args_cp;
	int len;
	char *buffer;
	zend_class_entry *default_ce, *exception_ce;
	zval btrace;
	zval *emsg, rv1;
	zval *trace, rv;
    zval *ele_value = NULL;
	int frame = 1;

	TSRMLS_FETCH();

	object_init(event);


	/* Capture backtrace */
	zend_fetch_debug_backtrace(&btrace, 0, 0 ,1000);

	va_copy(args_cp, args);
	len = vspprintf(&buffer, PG(log_errors_max_len), format, args_cp);
	va_end(args_cp);


	if(SENTRY_G(last_exception) && Z_TYPE_P(SENTRY_G(last_exception)) == IS_OBJECT) {
		default_ce = Z_OBJCE_P(SENTRY_G(last_exception));
		emsg =    zend_read_property(default_ce, SENTRY_G(last_exception), "message",    sizeof("message")-1,    0 TSRMLS_CC, &rv1);
    //free(buffer);
		buffer =  Z_STRVAL_P(emsg);
	}

  time_t timer;
  char timebuff[100];
  struct tm* tm_info;
  time(&timer);
  tm_info = gmtime(&timer);
  strftime(timebuff, 99, "%Y-%m-%dT%H:%M:%S", tm_info);

  char culprit[1024];
  //TODO: get filename + method (if exception)
  snprintf(culprit, 1024, "%s:%d", error_filename, error_lineno);

  //TODO
  // add 
  //  release
  //  dist
  //  tags
  //  contexts
      // runtime


  zval contexts;
  zval runtime;
  object_init(&runtime);
  object_init(&contexts);

  add_property_string(&runtime, "name", "php");
  add_property_string(&runtime, "version", PHP_VERSION); 
  add_property_string(&runtime, "type", "runtime"); 

  add_property_zval(&contexts,"runtime", &runtime);
  add_property_zval(event,"contexts", &contexts);

  


  add_property_string(event, "message", buffer);
  add_property_string(event, "culprit", culprit);
  add_property_string(event, "platform", "php");
  add_property_string(event, "timestamp", timebuff);

  char * etype;
  etype = php_error_type_to_char(type);
  zval exception_payload;
  object_init(&exception_payload);
  add_property_string(&exception_payload, "type", etype);
  add_property_string(&exception_payload, "value", buffer);
  add_property_zval(event, "exception", &exception_payload);

  zval stacktrace;
  zval frames;
  zval frame0;
  char * frame0_line;



  object_init(&stacktrace);
  array_init(&frames);
  object_init(&frame0);

  

  



if(sentry_debugging_enabled() == 1) {

	/* Send to backend */
   	php_printf("SENTRY PHP-EXT Catched:\n");
   	php_printf("==============\n");


  php_printf("message: %s\n", buffer);
	php_printf("\tFrame(0):\n");
	php_printf("\t\tfile: %s\n", error_filename);
	php_printf("\t\tlineo: %d\n", error_lineno);
	php_printf("\t\ttype: %d\n", type);

}

    HashTable *hash_arr = Z_ARRVAL(btrace);
	if(SENTRY_G(last_exception) && Z_TYPE_P(SENTRY_G(last_exception)) == IS_OBJECT) {
		default_ce = Z_OBJCE_P(SENTRY_G(last_exception));
		trace =    zend_read_property(default_ce, SENTRY_G(last_exception), "trace",    sizeof("trace")-1,    0 TSRMLS_CC, &rv);
		hash_arr = Z_ARRVAL_P(trace);

	}
	ZEND_HASH_REVERSE_FOREACH_VAL_IND(hash_arr,  ele_value) {
		zval * file, * lineo, *function, *class;
		file = zend_hash_str_find(Z_ARRVAL_P(ele_value), "file", sizeof("file")-1);
		lineo = zend_hash_str_find(Z_ARRVAL_P(ele_value), "line", sizeof("line")-1);
		class = zend_hash_str_find(Z_ARRVAL_P(ele_value), "class", sizeof("class")-1);
		function = zend_hash_str_find(Z_ARRVAL_P(ele_value), "function", sizeof("function")-1);

    zval current_frame;
    object_init(&current_frame);

    char * current_frame_line = get_line_of_file(Z_STRVAL_P(file),Z_LVAL_P(lineo));
    add_property_string(&current_frame, "abs_path", Z_STRVAL_P(file));
    add_property_string(&current_frame, "filename", Z_STRVAL_P(file));
    add_property_string(&current_frame, "function", Z_STRVAL_P(function));
    add_property_long(&current_frame, "lineno", Z_LVAL_P(lineo));
    add_property_string(&current_frame, "context_line", current_frame_line);
    free(current_frame_line);

    add_next_index_zval(&frames, &current_frame);
 

    if(sentry_debugging_enabled() == 1) {
		  php_printf("\tFrame(%d):\n", frame);
		  php_printf("\t\tfile: %s\n", Z_STRVAL_P(file));
	  	php_printf("\t\tlineo: %ld\n", Z_LVAL_P(lineo));
		  php_printf("\t\tclass: %s\n", Z_STRVAL_P(class));
		  php_printf("\t\tfunction: %s\n", Z_STRVAL_P(function));

    }
		frame++;
  }
  ZEND_HASH_FOREACH_END();


if(sentry_debugging_enabled() == 1) {
   	php_printf("/SENTRY PHP-EXT Catched:\n");
}



  frame0_line = get_line_of_file(error_filename, error_lineno);
  add_property_string(&frame0, "abs_path", error_filename);
  add_property_string(&frame0, "filename", error_filename);
  add_property_string(&frame0, "function", "main1");
  add_property_long(&frame0, "lineno", error_lineno);
  add_property_string(&frame0, "context_line", frame0_line);
  free(frame0_line);

  add_next_index_zval(&frames, &frame0);



  add_property_zval(&stacktrace, "frames", &frames);
  add_property_zval(event, "stacktrace", &stacktrace);

  //SEND IT
  if(type == E_DEPRECATED || 
     type == E_WARNING    ||
     type == E_NOTICE) {
     zval crumb;
     time_t ctimer;
     char ctimebuff[100];
     struct tm* ctm_info;
     time(&ctimer);
     ctm_info = gmtime(&ctimer);
     strftime(ctimebuff, 99, "%Y-%m-%dT%H:%M:%S", ctm_info);


     zval crumb_data;
     object_init(&crumb_data);

     add_property_long(&crumb_data, "line", error_lineno);
     add_property_string(&crumb_data, "file", error_filename);



     object_init(&crumb);
     add_property_string(&crumb, "timestamp", ctimebuff);
     add_property_string(&crumb, "message", buffer);
     add_property_string(&crumb, "category", "error_reporting");
     add_property_zval(&crumb, "data", &crumb_data);
     add_next_index_zval(&SENTRY_G(breadcrumbs), &crumb);
  } else {
    add_property_zval(event, "breadcrumbs", &SENTRY_G(breadcrumbs));
    sentry_send_event(event);
  }

	if (free_event) {
		zval_dtor(event);
	}
}

static void php_sentry_exception_hook(zval *exception TSRMLS_DC)
{
	if(!exception) return;
	SENTRY_G(last_exception) = exception;
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

	if ((orig_set_error_handler = zend_hash_str_find_ptr(EG(function_table), "set_error_handler", sizeof("set_error_handler")-1)) != NULL) {
		SENTRY_G(orig_set_error_handler) = orig_set_error_handler->internal_function.handler;
		orig_set_error_handler->internal_function.handler = zif_sentry_set_error_handler;
	}
	if ((orig_restore_error_handler = zend_hash_str_find_ptr(EG(function_table),"restore_error_handler", sizeof("restore_error_handler") -1 )) != NULL) {
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
	sentry_globals->debug   = 0;

}

PHP_MINIT_FUNCTION(sentry)
{
    REGISTER_INI_ENTRIES();

    zend_class_entry tmp_ce;
    INIT_CLASS_ENTRY(tmp_ce, "Sentry\\Client\\Native", sentrynative_functions);
    sentry_class = zend_register_internal_class(&tmp_ce TSRMLS_CC);
    array_init(&SENTRY_G(breadcrumbs));

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(sentry)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(sentry)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "sentry support", "enabled");
	php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */


/* {{{ sentry_functions[]
 */
static const zend_function_entry sentry_functions[] = {
	PHP_FE(sentry_enable_debug,		NULL)
	PHP_FE(sentry_send_sample,		NULL)
	PHP_FE_END
};
/* }}} */

static const zend_module_dep sentry_dep_deps[] = {
    ZEND_MOD_REQUIRED("json")
    ZEND_MOD_END
};

/* {{{ sentry_module_entry
 */
zend_module_entry sentry_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
  sentry_dep_deps,
	"sentry",					/* Extension name */
	sentry_functions,			/* zend_function_entry */
	PHP_MINIT(sentry),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(sentry),							/* PHP_MSHUTDOWN - Module shutdown */
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
