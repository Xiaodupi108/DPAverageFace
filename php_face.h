/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: zhaoxy,wind                                                  |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_FACE_H
#define PHP_FACE_H

extern zend_module_entry face_module_entry;
#define phpext_face_ptr &face_module_entry

#define PHP_FACE_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_FACE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_FACE_API __attribute__ ((visibility("default")))
#else
#	define PHP_FACE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(face);
PHP_MSHUTDOWN_FUNCTION(face);
PHP_RINIT_FUNCTION(face);
PHP_RSHUTDOWN_FUNCTION(face);
PHP_MINFO_FUNCTION(face);

PHP_FUNCTION(confirm_face_compiled);	/* For testing, remove later. */
PHP_FUNCTION(getAverageFace);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(face)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(face)
*/

/* In every utility function you add that needs to use variables 
   in php_face_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as FACE_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define FACE_G(v) TSRMG(face_globals_id, zend_face_globals *, v)
#else
#define FACE_G(v) (face_globals.v)
#endif

#endif	/* PHP_FACE_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
