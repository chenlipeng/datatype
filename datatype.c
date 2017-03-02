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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_datatype.h"

/* If you declare any globals in php_datatype.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(datatype)
*/

/* True global resources - no need for thread safety here */
static int le_datatype;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("datatype.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_datatype_globals, datatype_globals)
    STD_PHP_INI_ENTRY("datatype.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_datatype_globals, datatype_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_datatype_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_datatype_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "datatype", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_datatype_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_datatype_init_globals(zend_datatype_globals *datatype_globals)
{
	datatype_globals->global_value = 0;
	datatype_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(datatype)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/

	//datatype_array_pickup
	//使用整个子数组作为提取后的值 默认
	REGISTER_NULL_CONSTANT("PICKUP_VK_ENTIRE", CONST_CS | CONST_PERSISTENT);
	//保留原来的Key 默认
	REGISTER_NULL_CONSTANT("PICKUP_KK_HOLD", CONST_CS | CONST_PERSISTENT);
	//使用自增长Key
	REGISTER_BOOL_CONSTANT("PICKUP_KK_AUTOINCR", 1, CONST_CS | CONST_PERSISTENT);
	//合并重复key对应的值
	REGISTER_LONG_CONSTANT("PICKUP_VD_MERGE", DATATYPE_ARRAY_PICKUP_VD_MERGE, CONST_CS | CONST_PERSISTENT);
	//重复key的值, 后面覆盖前面
	REGISTER_LONG_CONSTANT("PICKUP_VD_OVERWRITE", DATATYPE_ARRAY_PICKUP_VD_OVERWRITE, CONST_CS | CONST_PERSISTENT);
	//重复key的值, 保留前面, 跳过后面
	REGISTER_LONG_CONSTANT("PICKUP_VD_SKIP", DATATYPE_ARRAY_PICKUP_VD_SKIP, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(datatype)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(datatype)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(datatype)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(datatype)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "datatype support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ array_column_param_helper
 * Specialized conversion rules for array_column() function
 */
static inline
zend_bool array_column_param_helper(zval **param,
                                    const char *name TSRMLS_DC) {
	switch (Z_TYPE_PP(param)) {
		case IS_DOUBLE:
			convert_to_long_ex(param);
			/* fallthrough */
		case IS_LONG:
			return 1;

		case IS_OBJECT:
			convert_to_string_ex(param);
			/* fallthrough */
		case IS_STRING:
			return 1;

		case IS_ARRAY:
			return 1;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The %s key should be either a string or an integer", name);
			return 0;
	}
}/*}}}*/

/* {{{	datatype_array_pickup
 * */
PHP_FUNCTION(datatype_array_pickup) {
	zval **zcolumn = NULL, **zkey = NULL, **data;
	HashTable *arr_hash;
	long ondup = DATATYPE_ARRAY_PICKUP_VD_SKIP, auto_incr = -1;
	HashPosition pointer;

	//获取参数
	//	参数一
	//		必须为数组	当为一维数组时,不会提取其中的元素;
	//	参数二
	//		为NULL或其他类型时	使用整个子数组作为提取后的值;	PICKUP_VK_ENTIRE
	//		为数组时	使用子数组中指定的几列组成新数组作为提取后的值;
	//		为string时	使用指定子数组中一列值作为提取后的值
	//	参数三
	//		为NULL时	(默认值)保留原来的Key;	PICKUP_KK_HOLD
	//		为TRUE时	使用自增长Key;	PICKUP_KK_AUTOINCR
	//		为String时	(其他情况时)使用子数组中一列值作为提取后的key
	//	参数四 为整型
	//		为0时		(默认值)合并重复key对应的值	PICKUP_VD_MERGE
	//		为1时		重复key的值, 后面覆盖前面	PICKUP_VD_OVERWRITE
	//		为2时		(及其他情况时)重复key的值, 保留前面, 跳过后面	PICKUP_VD_SKIP
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "hZ!|Z!l", &arr_hash, &zcolumn, &zkey, &ondup) == FAILURE) {
		return;
	}

	//参数校验
	if ((zcolumn && !array_column_param_helper(zcolumn, "column" TSRMLS_CC)) ||
	    (zkey && !array_column_param_helper(zkey, "index" TSRMLS_CC))) {
		RETURN_FALSE;
	}

	//初始化返回值
	array_init(return_value);
	//结果提取
	for (zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
			zend_hash_get_current_data_ex(arr_hash, (void**)&data, &pointer) == SUCCESS;
			zend_hash_move_forward_ex(arr_hash, &pointer)) {
		zval **zcolval, **zkeyval = NULL;
		HashTable *ht;

		//元素不是数组 则跳过
		if (Z_TYPE_PP(data) != IS_ARRAY) {
			continue;
		}

		ht = Z_ARRVAL_PP(data);
		//提取参数值
		if (zcolumn && (Z_TYPE_PP(zcolumn) == IS_STRING)) {
			//TODO: 当找不到对应值 可以设为空
			if (zend_hash_find(ht, Z_STRVAL_PP(zcolumn), Z_STRLEN_PP(zcolumn) + 1, (void**)&zcolval) == FAILURE) {
				continue;
			}
		} else if (zcolumn && Z_TYPE_PP(zcolumn) == IS_ARRAY) {
			zval **inner_data, *inner_zcolval;
			HashPosition inner_pointer;
			HashTable *inner_ht = Z_ARRVAL_PP(zcolumn);

			MAKE_STD_ZVAL(inner_zcolval);
			array_init(inner_zcolval);	//使用之前需要初始化 MAKE_STD_ZVAL
			zcolval = &inner_zcolval;
			for (zend_hash_internal_pointer_reset_ex(inner_ht, &inner_pointer);
					zend_hash_get_current_data_ex(inner_ht, (void**)&inner_data, &inner_pointer) == SUCCESS;
					zend_hash_move_forward_ex(inner_ht, &inner_pointer)) {
				zval **temp_zcolval = NULL;
				//key
				if (Z_TYPE_PP(inner_data) == IS_LONG && zend_hash_index_find(ht, Z_LVAL_PP(inner_data), (void**)&temp_zcolval) == SUCCESS) {
					Z_DELREF_PP(temp_zcolval);	//important
					add_index_zval(inner_zcolval, Z_LVAL_PP(inner_data), *temp_zcolval);
				} else if (Z_TYPE_PP(inner_data) == IS_STRING && zend_hash_find(ht, Z_STRVAL_PP(inner_data), Z_STRLEN_PP(inner_data), (void**)&temp_zcolval) == SUCCESS) {
					Z_DELREF_PP(temp_zcolval);	//important
					add_assoc_zval(inner_zcolval, Z_STRVAL_PP(inner_data), *temp_zcolval);
				}
			}
		} else {
			zcolval = data;
		}

		//key
		//		为NULL时	(默认值)保留原来的Key;	PICKUP_KK_HOLD
		//		为TRUE时	使用自增长Key;	PICKUP_KK_AUTOINCR
		//		为String时	(其他情况时)使用子数组中一列值作为提取后的key
		if (!zkey) {
			//默认 保留原来的key
			zval *tempzkeyval;
			MAKE_STD_ZVAL(tempzkeyval);
			zend_hash_get_current_key_zval_ex(arr_hash, tempzkeyval, &pointer);
			zkeyval = &tempzkeyval;
		} else if (Z_TYPE_PP(zkey) == IS_BOOL && Z_BVAL_PP(zkey)) {
			//使用自增长Key
			MAKE_STD_ZVAL(*zkeyval);
			ZVAL_LONG(*zkeyval, ++auto_incr);
		} else if (Z_TYPE_PP(zkey) == IS_STRING && zend_hash_find(ht, Z_STRVAL_PP(zkey), Z_STRLEN_PP(zkey) + 1, (void**)&zkeyval) == FAILURE) {
			continue;
		}

		//ondup
		//		为0时		(默认值)合并重复key对应的值	PICKUP_VD_MERGE
		//		为1时		重复key的值, 后面覆盖前面	PICKUP_VD_OVERWRITE
		//		为2时		(及其他情况时)重复key的值, 保留前面, 跳过后面	PICKUP_VD_SKIP
		
		if (zcolumn && Z_TYPE_PP(zcolumn) != IS_ARRAY) {
			Z_ADDREF_PP(zcolval);
		}

		if (DATATYPE_ARRAY_PICKUP_VD_MERGE == ondup) {
			zval **value_merge, *vvalue_merge;
			//元素为一个数组集
			if (Z_TYPE_PP(zkeyval) == IS_STRING) {
				if (zend_hash_find(Z_ARRVAL_P(return_value), Z_STRVAL_PP(zkeyval), Z_STRLEN_PP(zkeyval) + 1, (void**)&value_merge) == FAILURE) {
					MAKE_STD_ZVAL(vvalue_merge);
					array_init(vvalue_merge);
					add_assoc_zval(return_value, Z_STRVAL_PP(zkeyval), vvalue_merge);
					value_merge = &vvalue_merge;
				}
			} else if (Z_TYPE_PP(zkeyval) == IS_LONG) {
				if (zend_hash_index_find(Z_ARRVAL_P(return_value), Z_LVAL_PP(zkeyval), (void**)&value_merge) == FAILURE) {
					MAKE_STD_ZVAL(vvalue_merge);
					array_init(vvalue_merge);
					add_index_zval(return_value, Z_LVAL_PP(zkeyval), vvalue_merge);
					value_merge = &vvalue_merge;
				}
			}
			add_next_index_zval(*value_merge, *zcolval);
		} else if (DATATYPE_ARRAY_PICKUP_VD_OVERWRITE == ondup) {
			if (Z_TYPE_PP(zkeyval) == IS_STRING) {
				add_assoc_zval(return_value, Z_STRVAL_PP(zkeyval), *zcolval);
			} else if (Z_TYPE_PP(zkeyval) == IS_LONG) {
				add_index_zval(return_value, Z_LVAL_PP(zkeyval), *zcolval);
			}
		} else {
			if (Z_TYPE_PP(zkeyval) == IS_STRING && !(zend_hash_exists(Z_ARRVAL_P(return_value), Z_STRVAL_PP(zkeyval), Z_STRLEN_PP(zkeyval) + 1))) {
				add_assoc_zval(return_value, Z_STRVAL_PP(zkeyval), *zcolval);
			} else if (Z_TYPE_PP(zkeyval) == IS_LONG && !(zend_hash_index_exists(Z_ARRVAL_P(return_value), Z_LVAL_PP(zkeyval)))) {
				add_index_zval(return_value, Z_LVAL_PP(zkeyval), *zcolval);
			}
		}
	}
}/*}}}*/

/* {{{ zend_hash_apply_callback
 * zend_hash_apply的回调函数 用于遍历一个数组 对其中的元素进行trim操作
 * */
int zend_hash_apply_callback(zval **val TSRMLS_CC) {
	char *what = " ";
	int what_len = strlen(what);
	//进行trim操作 使用的方法为一个PHPAPI
	php_trim(Z_STRVAL_PP(val), Z_STRLEN_PP(val), what, what_len, *val, 3 TSRMLS_CC);
	return ZEND_HASH_APPLY_KEEP;
}/*}}}*/

/* {{{ datatype_array_fromCommaExp
 * 从逗号表达式转换数组 可以选择对元素进行trim
 * proto: public static function fromCommaExp($commaExp, $isTrim = TRUE)
 * */
PHP_FUNCTION(datatype_array_fromCommaExp) {
	zend_bool is_trim = 1;
	char *comma_exp;
	long comma_exp_len;
	zval zdelim, zstr;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &comma_exp, &comma_exp_len, &is_trim) == FAILURE) {
		return;
	}

	array_init(return_value);
	ZVAL_STRINGL(&zstr, comma_exp, comma_exp_len, 0);
	ZVAL_STRINGL(&zdelim, ",", sizeof(",") - 1, 0);
	php_explode(&zdelim, &zstr, return_value, LONG_MAX);

	printf("%d\n", Z_TYPE_P(return_value));
	if (is_trim) {
		zend_hash_apply(Z_ARRVAL_P(return_value), zend_hash_apply_callback TSRMLS_CC);
	}
}/*}}}*/

/* {{{ 检查一组元素全部都在另一组元素中出现
 * proto: public static function isAllInclude($needle, $haystack, $strict = FALSE)
 * */
PHP_FUNCTION(datatype_array_isAllInclude) {

}/*}}}*/

/* {{{ datatype_functions[]
 *
 * Every user visible function must have an entry in datatype_functions[].
 */
const zend_function_entry datatype_functions[] = {
	PHP_FE(confirm_datatype_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(datatype_array_pickup,	NULL)
	PHP_FE(datatype_array_fromCommaExp,	NULL)
	PHP_FE_END	/* Must be the last line in datatype_functions[] */
};
/* }}} */

/* {{{ datatype_module_entry
 */
zend_module_entry datatype_module_entry = {
	STANDARD_MODULE_HEADER,
	"datatype",
	datatype_functions,
	PHP_MINIT(datatype),
	PHP_MSHUTDOWN(datatype),
	PHP_RINIT(datatype),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(datatype),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(datatype),
	PHP_DATATYPE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_DATATYPE
ZEND_GET_MODULE(datatype)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
