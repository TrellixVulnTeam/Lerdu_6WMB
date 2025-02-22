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
  | Author: Omar Kilani <omar@php.net>                                   |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_smart_str.h"
#include "utf8_to_utf16.h"
#include "JSON_parser.h"
#include "php_json.h"

static PHP_MINFO_FUNCTION(json);
static PHP_FUNCTION(json_encode);
static PHP_FUNCTION(json_decode);
static PHP_FUNCTION(json_last_error);

static const char digits[] = "0123456789abcdef";

ZEND_DECLARE_MODULE_GLOBALS(json)

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_json_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_json_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, json)
	ZEND_ARG_INFO(0, assoc)
	ZEND_ARG_INFO(0, depth)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_json_last_error, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ json_functions[] */
static const function_entry json_functions[] = {
	PHP_FE(json_encode, arginfo_json_encode)
	PHP_FE(json_decode, arginfo_json_decode)
	PHP_FE(json_last_error, arginfo_json_last_error)
	PHP_FE_END
};
/* }}} */

/* {{{ MINIT */
static PHP_MINIT_FUNCTION(json)
{
	REGISTER_LONG_CONSTANT("JSON_HEX_TAG",  PHP_JSON_HEX_TAG,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_HEX_AMP",  PHP_JSON_HEX_AMP,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_HEX_APOS", PHP_JSON_HEX_APOS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_HEX_QUOT", PHP_JSON_HEX_QUOT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_FORCE_OBJECT", PHP_JSON_FORCE_OBJECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_NUMERIC_CHECK", PHP_JSON_NUMERIC_CHECK, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("JSON_ERROR_NONE", PHP_JSON_ERROR_NONE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_DEPTH", PHP_JSON_ERROR_DEPTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_STATE_MISMATCH", PHP_JSON_ERROR_STATE_MISMATCH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_CTRL_CHAR", PHP_JSON_ERROR_CTRL_CHAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_SYNTAX", PHP_JSON_ERROR_SYNTAX, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_UTF8", PHP_JSON_ERROR_UTF8, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
*/
static PHP_GINIT_FUNCTION(json)
{
	json_globals->error_code = 0;
}
/* }}} */


/* {{{ json_module_entry
 */
zend_module_entry json_module_entry = {
	STANDARD_MODULE_HEADER,
	"json",
	json_functions,
	PHP_MINIT(json),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(json),
	PHP_JSON_VERSION,
	PHP_MODULE_GLOBALS(json),
	PHP_GINIT(json),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_JSON
ZEND_GET_MODULE(json)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(json)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "json support", "enabled");
	php_info_print_table_row(2, "json version", PHP_JSON_VERSION);
	php_info_print_table_end();
}
/* }}} */

static void json_escape_string(smart_str *buf, char *s, int len, int options TSRMLS_DC);

static int json_determine_array_type(zval **val TSRMLS_DC) /* {{{ */
{
	int i;
	HashTable *myht = HASH_OF(*val);

	i = myht ? zend_hash_num_elements(myht) : 0;
	if (i > 0) {
		char *key;
		ulong index, idx;
		uint key_len;
		HashPosition pos;

		zend_hash_internal_pointer_reset_ex(myht, &pos);
		idx = 0;
		for (;; zend_hash_move_forward_ex(myht, &pos)) {
			i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pos);
			if (i == HASH_KEY_NON_EXISTANT) {
				break;
			}

			if (i == HASH_KEY_IS_STRING) {
				return 1;
			} else {
				if (index != idx) {
					return 1;
				}
			}
			idx++;
		}
	}

	return PHP_JSON_OUTPUT_ARRAY;
}
/* }}} */

static void json_encode_array(smart_str *buf, zval **val, int options TSRMLS_DC) /* {{{ */
{
	int i, r;
	HashTable *myht;

	if (Z_TYPE_PP(val) == IS_ARRAY) {
		myht = HASH_OF(*val);
		r = (options & PHP_JSON_FORCE_OBJECT) ? PHP_JSON_OUTPUT_OBJECT : json_determine_array_type(val TSRMLS_CC);
	} else {
		myht = Z_OBJPROP_PP(val);
		r = PHP_JSON_OUTPUT_OBJECT;
	}

	if (myht && myht->nApplyCount > 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "recursion detected");
		smart_str_appendl(buf, "null", 4);
		return;
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, '[');
	} else {
		smart_str_appendc(buf, '{');
	}

	i = myht ? zend_hash_num_elements(myht) : 0;

	if (i > 0)
	{
		char *key;
		zval **data;
		ulong index;
		uint key_len;
		HashPosition pos;
		HashTable *tmp_ht;
		int need_comma = 0;

		zend_hash_internal_pointer_reset_ex(myht, &pos);
		for (;; zend_hash_move_forward_ex(myht, &pos)) {
			i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pos);
			if (i == HASH_KEY_NON_EXISTANT)
				break;

			if (zend_hash_get_current_data_ex(myht, (void **) &data, &pos) == SUCCESS) {
				tmp_ht = HASH_OF(*data);
				if (tmp_ht) {
					tmp_ht->nApplyCount++;
				}

				if (r == PHP_JSON_OUTPUT_ARRAY) {
					if (need_comma) {
						smart_str_appendc(buf, ',');
					} else {
						need_comma = 1;
					}
 
					php_json_encode(buf, *data, options TSRMLS_CC);
				} else if (r == PHP_JSON_OUTPUT_OBJECT) {
					if (i == HASH_KEY_IS_STRING) {
						if (key[0] == '\0' && Z_TYPE_PP(val) == IS_OBJECT) {
							/* Skip protected and private members. */
							if (tmp_ht) {
								tmp_ht->nApplyCount--;
							}
							continue;
						}

						if (need_comma) {
							smart_str_appendc(buf, ',');
						} else {
							need_comma = 1;
						}

						json_escape_string(buf, key, key_len - 1, options & ~PHP_JSON_NUMERIC_CHECK TSRMLS_CC);
						smart_str_appendc(buf, ':');

						php_json_encode(buf, *data, options TSRMLS_CC);
					} else {
						if (need_comma) {
							smart_str_appendc(buf, ',');
						} else {
							need_comma = 1;
						}

						smart_str_appendc(buf, '"');
						smart_str_append_long(buf, (long) index);
						smart_str_appendc(buf, '"');
						smart_str_appendc(buf, ':');

						php_json_encode(buf, *data, options TSRMLS_CC);
					}
				}

				if (tmp_ht) {
					tmp_ht->nApplyCount--;
				}
			}
		}
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, ']');
	} else {
		smart_str_appendc(buf, '}');
	}
}
/* }}} */

#define REVERSE16(us) (((us & 0xf) << 12) | (((us >> 4) & 0xf) << 8) | (((us >> 8) & 0xf) << 4) | ((us >> 12) & 0xf))

static void json_escape_string(smart_str *buf, char *s, int len, int options TSRMLS_DC) /* {{{ */
{
	int pos = 0;
	unsigned short us;
	unsigned short *utf16;

	if (len == 0) {
		smart_str_appendl(buf, "\"\"", 2);
		return;
	}

	if (options & PHP_JSON_NUMERIC_CHECK) {
		double d;
		int type;
		long p;

		if ((type = is_numeric_string(s, len, &p, &d, 0)) != 0) {
			if (type == IS_LONG) {
				smart_str_append_long(buf, p);
			} else if (type == IS_DOUBLE) {
				if (!zend_isinf(d) && !zend_isnan(d)) {
					char *tmp;
					int l = spprintf(&tmp, 0, "%.*k", (int) EG(precision), d);
					smart_str_appendl(buf, tmp, l);
					efree(tmp);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "double %.9g does not conform to the JSON spec, encoded as 0", d);
					smart_str_appendc(buf, '0');
				}
			}
			return;
		}
		
	}

	utf16 = (unsigned short *) safe_emalloc(len, sizeof(unsigned short), 0);

	len = utf8_to_utf16(utf16, s, len);
	if (len <= 0) {
		if (utf16) {
			efree(utf16);
		}
		if (len < 0) {
			JSON_G(error_code) = PHP_JSON_ERROR_UTF8;
			if (!PG(display_errors)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid UTF-8 sequence in argument");
			}
			smart_str_appendl(buf, "null", 4);
		} else {
			smart_str_appendl(buf, "\"\"", 2);
		}
		return;
	}

	smart_str_appendc(buf, '"');

	while (pos < len)
	{
		us = utf16[pos++];

		switch (us)
		{
			case '"':
				if (options & PHP_JSON_HEX_QUOT) {
					smart_str_appendl(buf, "\\u0022", 6);
				} else {
					smart_str_appendl(buf, "\\\"", 2);
				}
				break;

			case '\\':
				smart_str_appendl(buf, "\\\\", 2);
				break;

			case '/':
				smart_str_appendl(buf, "\\/", 2);
				break;

			case '\b':
				smart_str_appendl(buf, "\\b", 2);
				break;

			case '\f':
				smart_str_appendl(buf, "\\f", 2);
				break;

			case '\n':
				smart_str_appendl(buf, "\\n", 2);
				break;

			case '\r':
				smart_str_appendl(buf, "\\r", 2);
				break;

			case '\t':
				smart_str_appendl(buf, "\\t", 2);
				break;

			case '<':
				if (options & PHP_JSON_HEX_TAG) {
					smart_str_appendl(buf, "\\u003C", 6);
				} else {
					smart_str_appendc(buf, '<');
				}
				break;

			case '>':
				if (options & PHP_JSON_HEX_TAG) {
					smart_str_appendl(buf, "\\u003E", 6);
				} else {
					smart_str_appendc(buf, '>');
				}
				break;

			case '&':
				if (options & PHP_JSON_HEX_AMP) {
					smart_str_appendl(buf, "\\u0026", 6);
				} else {
					smart_str_appendc(buf, '&');
				}
				break;

			case '\'':
				if (options & PHP_JSON_HEX_APOS) {
					smart_str_appendl(buf, "\\u0027", 6);
				} else {
					smart_str_appendc(buf, '\'');
				}
				break;

			default:
				if (us >= ' ' && (us & 127) == us) {
					smart_str_appendc(buf, (unsigned char) us);
				} else {
					smart_str_appendl(buf, "\\u", 2);
					us = REVERSE16(us);

					smart_str_appendc(buf, digits[us & ((1 << 4) - 1)]);
					us >>= 4;
					smart_str_appendc(buf, digits[us & ((1 << 4) - 1)]);
					us >>= 4;
					smart_str_appendc(buf, digits[us & ((1 << 4) - 1)]);
					us >>= 4;
					smart_str_appendc(buf, digits[us & ((1 << 4) - 1)]);
				}
				break;
		}
	}

	smart_str_appendc(buf, '"');
	efree(utf16);
}
/* }}} */

PHP_JSON_API void php_json_encode(smart_str *buf, zval *val, int options TSRMLS_DC) /* {{{ */
{
	switch (Z_TYPE_P(val))
	{
		case IS_NULL:
			smart_str_appendl(buf, "null", 4);
			break;

		case IS_BOOL:
			if (Z_BVAL_P(val)) {
				smart_str_appendl(buf, "true", 4);
			} else {
				smart_str_appendl(buf, "false", 5);
			}
			break;

		case IS_LONG:
			smart_str_append_long(buf, Z_LVAL_P(val));
			break;

		case IS_DOUBLE:
			{
				char *d = NULL;
				int len;
				double dbl = Z_DVAL_P(val);

				if (!zend_isinf(dbl) && !zend_isnan(dbl)) {
					len = spprintf(&d, 0, "%.*k", (int) EG(precision), dbl);
					smart_str_appendl(buf, d, len);
					efree(d);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "double %.9g does not conform to the JSON spec, encoded as 0", dbl);
					smart_str_appendc(buf, '0');
				}
			}
			break;

		case IS_STRING:
			json_escape_string(buf, Z_STRVAL_P(val), Z_STRLEN_P(val), options TSRMLS_CC);
			break;

		case IS_ARRAY:
		case IS_OBJECT:
			json_encode_array(buf, &val, options TSRMLS_CC);
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "type is unsupported, encoded as null");
			smart_str_appendl(buf, "null", 4);
			break;
	}

	return;
}
/* }}} */

PHP_JSON_API void php_json_decode(zval *return_value, char *str, int str_len, zend_bool assoc, long depth TSRMLS_DC) /* {{{ */
{
	int utf16_len;
	zval *z;
	unsigned short *utf16;
	JSON_parser jp;

	utf16 = (unsigned short *) safe_emalloc((str_len+1), sizeof(unsigned short), 1);

	utf16_len = utf8_to_utf16(utf16, str, str_len);
	if (utf16_len <= 0) {
		if (utf16) {
			efree(utf16);
		}
		JSON_G(error_code) = PHP_JSON_ERROR_UTF8;
		RETURN_NULL();
	}

	if (depth <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Depth must be greater than zero");
		efree(utf16);
		RETURN_NULL();
	}

	ALLOC_INIT_ZVAL(z);
	jp = new_JSON_parser(depth);
	if (parse_JSON(jp, z, utf16, utf16_len, assoc TSRMLS_CC)) {
		*return_value = *z;
	}
	else
	{
		double d;
		int type;
		long p;

		RETVAL_NULL();
		if (str_len == 4) {
			if (!strcasecmp(str, "null")) {
				/* We need to explicitly clear the error because its an actual NULL and not an error */
				jp->error_code = PHP_JSON_ERROR_NONE;
				RETVAL_NULL();
			} else if (!strcasecmp(str, "true")) {
				RETVAL_BOOL(1);
			}
		} else if (str_len == 5 && !strcasecmp(str, "false")) {
			RETVAL_BOOL(0);
		}

		if ((type = is_numeric_string(str, str_len, &p, &d, 0)) != 0) {
			if (type == IS_LONG) {
				RETVAL_LONG(p);
			} else if (type == IS_DOUBLE) {
				RETVAL_DOUBLE(d);
			}
		}

		if (Z_TYPE_P(return_value) != IS_NULL) {
			jp->error_code = PHP_JSON_ERROR_NONE;
		}

		zval_dtor(z);
	}
	FREE_ZVAL(z);
	efree(utf16);
	JSON_G(error_code) = jp->error_code;
	free_JSON_parser(jp);
}
/* }}} */

/* {{{ proto string json_encode(mixed data [, int options])
   Returns the JSON representation of a value */
static PHP_FUNCTION(json_encode)
{
	zval *parameter;
	smart_str buf = {0};
	long options = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &parameter, &options) == FAILURE) {
		return;
	}

	JSON_G(error_code) = PHP_JSON_ERROR_NONE;

	php_json_encode(&buf, parameter, options TSRMLS_CC);

	ZVAL_STRINGL(return_value, buf.c, buf.len, 1);

	smart_str_free(&buf);
}
/* }}} */

/* {{{ proto mixed json_decode(string json [, bool assoc [, long depth]])
   Decodes the JSON representation into a PHP value */
static PHP_FUNCTION(json_decode)
{
	char *str;
	int str_len;
	zend_bool assoc = 0; /* return JS objects as PHP objects by default */
	long depth = JSON_PARSER_DEFAULT_DEPTH;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bl", &str, &str_len, &assoc, &depth) == FAILURE) {
		return;
	}

	JSON_G(error_code) = 0;

	if (!str_len) {
		RETURN_NULL();
	}

	php_json_decode(return_value, str, str_len, assoc, depth TSRMLS_CC);
}
/* }}} */

/* {{{ proto int json_last_error()
   Returns the error code of the last json_decode(). */
static PHP_FUNCTION(json_last_error)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(JSON_G(error_code));
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
