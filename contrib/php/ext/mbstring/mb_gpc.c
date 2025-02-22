/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rui Hirokawa <hirokawa@php.net>                              |
   |         Moriyoshi Koizumi <moriyoshi@php.net>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_variables.h"
#include "mbstring.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_mail.h"
#include "ext/standard/url.h"
#include "main/php_output.h"
#include "ext/standard/info.h"

#include "php_variables.h"
#include "php_globals.h"
#include "rfc1867.h"
#include "php_content_types.h"
#include "SAPI.h"
#include "TSRM.h"

#include "mb_gpc.h"
/* }}} */

#if HAVE_MBSTRING

ZEND_EXTERN_MODULE_GLOBALS(mbstring)

/* {{{ MBSTRING_API SAPI_TREAT_DATA_FUNC(mbstr_treat_data)
 * http input processing */
MBSTRING_API SAPI_TREAT_DATA_FUNC(mbstr_treat_data)
{
	char *res = NULL, *separator=NULL;
	const char *c_var;
	zval *array_ptr;
	int free_buffer=0;
	enum mbfl_no_encoding detected;
	php_mb_encoding_handler_info_t info;

	if (arg != PARSE_STRING) {
		char *value = zend_ini_string("mbstring.internal_encoding", sizeof("mbstring.internal_encoding"), 0);
		_php_mb_ini_mbstring_internal_encoding_set(value, value ? strlen(value): 0 TSRMLS_CC);
	}

	if (!MBSTRG(encoding_translation)) {
		php_default_treat_data(arg, str, destArray TSRMLS_CC);
		return;
	}

	switch (arg) {
		case PARSE_POST:
		case PARSE_GET:
		case PARSE_COOKIE:
			ALLOC_ZVAL(array_ptr);
			array_init(array_ptr);
			INIT_PZVAL(array_ptr);
			switch (arg) {
				case PARSE_POST:
					PG(http_globals)[TRACK_VARS_POST] = array_ptr;
					break;
				case PARSE_GET:
					PG(http_globals)[TRACK_VARS_GET] = array_ptr;
					break;
				case PARSE_COOKIE:
					PG(http_globals)[TRACK_VARS_COOKIE] = array_ptr;
					break;
			}
			break;
		default:
			array_ptr=destArray;
			break;
	}

	if (arg==PARSE_POST) { 
		sapi_handle_post(array_ptr TSRMLS_CC);
		return;
	}

	if (arg == PARSE_GET) {		/* GET data */
		c_var = SG(request_info).query_string;
		if (c_var && *c_var) {
			res = (char *) estrdup(c_var);
			free_buffer = 1;
		} else {
			free_buffer = 0;
		}
	} else if (arg == PARSE_COOKIE) {		/* Cookie data */
		c_var = SG(request_info).cookie_data;
		if (c_var && *c_var) {
			res = (char *) estrdup(c_var);
			free_buffer = 1;
		} else {
			free_buffer = 0;
		}
	} else if (arg == PARSE_STRING) {		/* String data */
		res = str;
		free_buffer = 1;
	}

	if (!res) {
		return;
	}

	switch (arg) {
	case PARSE_POST:
	case PARSE_GET:
	case PARSE_STRING:
		separator = (char *) estrdup(PG(arg_separator).input);
		break;
	case PARSE_COOKIE:
		separator = ";\0";
		break;
	}
	
	switch(arg) {
	case PARSE_POST:
		MBSTRG(http_input_identify_post) = mbfl_no_encoding_invalid;
		break;
	case PARSE_GET:
		MBSTRG(http_input_identify_get) = mbfl_no_encoding_invalid;
		break;
	case PARSE_COOKIE:
		MBSTRG(http_input_identify_cookie) = mbfl_no_encoding_invalid;
		break;
	case PARSE_STRING:
		MBSTRG(http_input_identify_string) = mbfl_no_encoding_invalid;
		break;
	}

	info.data_type              = arg;
	info.separator              = separator; 
	info.force_register_globals = 0;
	info.report_errors          = 0;
	info.to_encoding            = MBSTRG(internal_encoding);
	info.to_language            = MBSTRG(language);
	info.from_encodings         = MBSTRG(http_input_list);
	info.num_from_encodings     = MBSTRG(http_input_list_size); 
	info.from_language          = MBSTRG(language);

	MBSTRG(illegalchars) = 0;

	detected = _php_mb_encoding_handler_ex(&info, array_ptr, res TSRMLS_CC);
	MBSTRG(http_input_identify) = detected;

	if (detected != mbfl_no_encoding_invalid) {
		switch(arg){
		case PARSE_POST:
			MBSTRG(http_input_identify_post) = detected;
			break;
		case PARSE_GET:
			MBSTRG(http_input_identify_get) = detected;
			break;
		case PARSE_COOKIE:
			MBSTRG(http_input_identify_cookie) = detected;
			break;
		case PARSE_STRING:
			MBSTRG(http_input_identify_string) = detected;
			break;
		}
	}

	if (arg != PARSE_COOKIE) {
		efree(separator);
	}

	if (free_buffer) {
		efree(res);
	}
}
/* }}} */

/* {{{ mbfl_no_encoding _php_mb_encoding_handler_ex() */
enum mbfl_no_encoding _php_mb_encoding_handler_ex(const php_mb_encoding_handler_info_t *info, zval *arg, char *res TSRMLS_DC)
{
	char *var, *val;
	const char *s1, *s2;
	char *strtok_buf = NULL, **val_list = NULL;
	zval *array_ptr = (zval *) arg;
	int n, num, *len_list = NULL;
	unsigned int val_len, new_val_len;
	mbfl_string string, resvar, resval;
	enum mbfl_no_encoding from_encoding = mbfl_no_encoding_invalid;
	mbfl_encoding_detector *identd = NULL; 
	mbfl_buffer_converter *convd = NULL;
	int prev_rg_state = 0;

	mbfl_string_init_set(&string, info->to_language, info->to_encoding);
	mbfl_string_init_set(&resvar, info->to_language, info->to_encoding);
	mbfl_string_init_set(&resval, info->to_language, info->to_encoding);

	/* register_globals stuff
	 * XXX: this feature is going to be deprecated? */

	if (info->force_register_globals && !(prev_rg_state = PG(register_globals))) {
		zend_alter_ini_entry("register_globals", sizeof("register_globals"), "1", sizeof("1")-1, PHP_INI_PERDIR, PHP_INI_STAGE_RUNTIME);
	}

	if (!res || *res == '\0') {
		goto out;
	}
	
	/* count the variables(separators) contained in the "res".
	 * separator may contain multiple separator chars.
	 */
	num = 1;
	for (s1=res; *s1 != '\0'; s1++) {
		for (s2=info->separator; *s2 != '\0'; s2++) {
			if (*s1 == *s2) {
				num++;
			}	
		}
	}
	num *= 2; /* need space for variable name and value */
	
	val_list = (char **)ecalloc(num, sizeof(char *));
	len_list = (int *)ecalloc(num, sizeof(int));

	/* split and decode the query */
	n = 0;
	strtok_buf = NULL;
	var = php_strtok_r(res, info->separator, &strtok_buf);
	while (var)  {
		val = strchr(var, '=');
		if (val) { /* have a value */
			len_list[n] = php_url_decode(var, val-var);
			val_list[n] = var;
			n++;
			
			*val++ = '\0';
			val_list[n] = val;
			len_list[n] = php_url_decode(val, strlen(val));
		} else {
			len_list[n] = php_url_decode(var, strlen(var));
			val_list[n] = var;
			n++;
			
			val_list[n] = "";
			len_list[n] = 0;
		}
		n++;
		var = php_strtok_r(NULL, info->separator, &strtok_buf);
	} 
	num = n; /* make sure to process initilized vars only */
	
	/* initialize converter */
	if (info->num_from_encodings <= 0) {
		from_encoding = mbfl_no_encoding_pass;
	} else if (info->num_from_encodings == 1) {
		from_encoding = info->from_encodings[0];
	} else {
		/* auto detect */
		from_encoding = mbfl_no_encoding_invalid;
		identd = mbfl_encoding_detector_new((enum mbfl_no_encoding *)info->from_encodings, info->num_from_encodings, MBSTRG(strict_detection));
		if (identd) {
			n = 0;
			while (n < num) {
				string.val = (unsigned char *)val_list[n];
				string.len = len_list[n];
				if (mbfl_encoding_detector_feed(identd, &string)) {
					break;
				}
				n++;
			}
			from_encoding = mbfl_encoding_detector_judge(identd);
			mbfl_encoding_detector_delete(identd);
		}
		if (from_encoding == mbfl_no_encoding_invalid) {
			if (info->report_errors) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to detect encoding");
			}
			from_encoding = mbfl_no_encoding_pass;
		}
	}

	convd = NULL;
	if (from_encoding != mbfl_no_encoding_pass) {
		convd = mbfl_buffer_converter_new(from_encoding, info->to_encoding, 0);
		if (convd != NULL) {
			mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
			mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));
		} else {
			if (info->report_errors) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create converter");
			}
			goto out;
		}
	}

	/* convert encoding */
	string.no_encoding = from_encoding;

	n = 0;
	while (n < num) {
		string.val = (unsigned char *)val_list[n];
		string.len = len_list[n];
		if (convd != NULL && mbfl_buffer_converter_feed_result(convd, &string, &resvar) != NULL) {
			var = (char *)resvar.val;
		} else {
			var = val_list[n];
		}
		n++;
		string.val = val_list[n];
		string.len = len_list[n];
		if (convd != NULL && mbfl_buffer_converter_feed_result(convd, &string, &resval) != NULL) {
			val = resval.val;
			val_len = resval.len;
		} else {
			val = val_list[n];
			val_len = len_list[n];
		}
		n++;
		/* we need val to be emalloc()ed */
		val = estrndup(val, val_len);
		if (sapi_module.input_filter(info->data_type, var, &val, val_len, &new_val_len TSRMLS_CC)) {
			/* add variable to symbol table */
			php_register_variable_safe(var, val, new_val_len, array_ptr TSRMLS_CC);
		}
		efree(val);
		
		if (convd != NULL){
			mbfl_string_clear(&resvar);
			mbfl_string_clear(&resval);
		}
	}

out:
	/* register_global stuff */
	if (info->force_register_globals && !prev_rg_state) {
		zend_alter_ini_entry("register_globals", sizeof("register_globals"), "0", sizeof("0")-1, PHP_INI_PERDIR, PHP_INI_STAGE_RUNTIME);
	}

	if (convd != NULL) {
		MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
		mbfl_buffer_converter_delete(convd);
	}
	if (val_list != NULL) {
		efree((void *)val_list);
	}
	if (len_list != NULL) {
		efree((void *)len_list);
	}

	return from_encoding;
}
/* }}} */

/* {{{ SAPI_POST_HANDLER_FUNC(php_mb_post_handler) */
SAPI_POST_HANDLER_FUNC(php_mb_post_handler)
{
	enum mbfl_no_encoding detected;
	php_mb_encoding_handler_info_t info;

	MBSTRG(http_input_identify_post) = mbfl_no_encoding_invalid;

	info.data_type              = PARSE_POST;
	info.separator              = "&";
	info.force_register_globals = 0;
	info.report_errors          = 0;
	info.to_encoding            = MBSTRG(internal_encoding);
	info.to_language            = MBSTRG(language);
	info.from_encodings         = MBSTRG(http_input_list);
	info.num_from_encodings     = MBSTRG(http_input_list_size); 
	info.from_language          = MBSTRG(language);

	detected = _php_mb_encoding_handler_ex(&info, arg, SG(request_info).post_data TSRMLS_CC);

	MBSTRG(http_input_identify) = detected;
	if (detected != mbfl_no_encoding_invalid) {
		MBSTRG(http_input_identify_post) = detected;
	}
}
/* }}} */

#endif /* HAVE_MBSTRING */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */

