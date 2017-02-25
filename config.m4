dnl $Id$
dnl config.m4 for extension datatype

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(datatype, for datatype support,
dnl Make sure that the comment is aligned:
[  --with-datatype             Include datatype support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(datatype, whether to enable datatype support,
dnl Make sure that the comment is aligned:
dnl [  --enable-datatype           Enable datatype support])

if test "$PHP_DATATYPE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-datatype -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/datatype.h"  # you most likely want to change this
  dnl if test -r $PHP_DATATYPE/$SEARCH_FOR; then # path given as parameter
  dnl   DATATYPE_DIR=$PHP_DATATYPE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for datatype files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       DATATYPE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$DATATYPE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the datatype distribution])
  dnl fi

  dnl # --with-datatype -> add include path
  dnl PHP_ADD_INCLUDE($DATATYPE_DIR/include)

  dnl # --with-datatype -> check for lib and symbol presence
  dnl LIBNAME=datatype # you may want to change this
  dnl LIBSYMBOL=datatype # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $DATATYPE_DIR/$PHP_LIBDIR, DATATYPE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_DATATYPELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong datatype lib version or lib not found])
  dnl ],[
  dnl   -L$DATATYPE_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(DATATYPE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(datatype, datatype.c, $ext_shared)
fi
