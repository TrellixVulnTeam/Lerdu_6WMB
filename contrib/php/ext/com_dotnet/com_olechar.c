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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   |         Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"


PHP_COM_DOTNET_API OLECHAR *php_com_string_to_olestring(char *string, uint string_len, int codepage TSRMLS_DC)
{
	OLECHAR *olestring = NULL;
	DWORD flags = codepage == CP_UTF8 ? 0 : MB_PRECOMPOSED | MB_ERR_INVALID_CHARS;
	BOOL ok;

	if (string_len == -1) {
		/* determine required length for the buffer (includes NUL terminator) */
		string_len = MultiByteToWideChar(codepage, flags, string, -1, NULL, 0);
	} else {
		/* allow room for NUL terminator */
		string_len++;
	}

	if (string_len > 0) {
		olestring = (OLECHAR*)safe_emalloc(string_len, sizeof(OLECHAR), 0);
		ok = MultiByteToWideChar(codepage, flags, string, string_len, olestring, string_len);
	} else {
		ok = FALSE;
		olestring = (OLECHAR*)emalloc(sizeof(OLECHAR));
		*olestring = 0;
	}

	if (!ok) {
		char *msg = php_win_err(GetLastError());

		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Could not convert string to unicode: `%s'", msg);

		LocalFree(msg);
	}

	return olestring;
}

PHP_COM_DOTNET_API char *php_com_olestring_to_string(OLECHAR *olestring, uint *string_len, int codepage TSRMLS_DC)
{
	char *string;
	uint length = 0;
	BOOL ok;

	length = WideCharToMultiByte(codepage, 0, olestring, -1, NULL, 0, NULL, NULL);

	if (length) {
		string = (char*)safe_emalloc(length, sizeof(char), 0);
		length = WideCharToMultiByte(codepage, 0, olestring, -1, string, length, NULL, NULL);
		ok = length > 0;
	} else {
		string = (char*)emalloc(sizeof(char));
		*string = '\0';
		ok = FALSE;
		length = 0;
	}

	if (!ok) {
		char *msg = php_win_err(GetLastError());

		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Could not convert string from unicode: `%s'", msg);

		LocalFree(msg);
	}

	if (string_len) {
		*string_len = length-1;
	}

	return string;
}
