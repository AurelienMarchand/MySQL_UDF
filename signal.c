/*
* @author Aurelien Marchand 
*
*
* INSTALLATION
*
* gcc -o signal.so -shared signal.c -I /usr/include/mysql/
* (mac) gcc -bundle -o signal.so signal.c -I/usr/local/mysql/include
*
* Put the shared library as described in: http://dev.mysql.com/doc/refman/5.0/en/udf-compiling.html
* if SELINUX complains, run: chcon -t texrel_shlib_t /usr/lib/mysql/plugin/signal.so
*
* Afterwards in SQL:
*
* CREATE FUNCTION signal RETURNS STRING SONAME 'signal.so';
*
* -------------------------------------------------------------------------
*
* Code inspired by http://rpbouman.blogspot.com/2005/11/using-udf-to-raise-errors-from-inside.html
* Thanks to Roland Bouman
*
* -------------------------------------------------------------------------
*
*
* See MySQL UDF documentation pages for details on the implementation of UDF functions.
*
*/

#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong; /* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#if defined(MYSQL_SERVER)
#include <m_string.h>
#else
/* when compiled as standalone */
#include <string.h>
#endif
#endif
#include <mysql.h>
#include <ctype.h>

#ifdef HAVE_DLOPEN

/* maximum length of error message */
#define SIGNAL_MAX 255

/**
* SIGNAL
*
* @param s string containing the error message
* @result NULL
*
*/
my_bool signal_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void signal_deinit(UDF_INIT *initid);
long long signal(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);


//-------------------------------------------------------------------------


my_bool signal_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	if(args->arg_count != 1 || args->arg_type[0] != STRING_RESULT || args->lengths[0] > SIGNAL_MAX) {
		strcpy(message, "Function requires 1 string argument less than 255 characters");
	} else {
		strncpy(message,args->args[0],args->lengths[0]);
	}
	/* always fail */
	return 1;
}

void signal_deinit(UDF_INIT *initid) {
	return;
}

long long signal(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {
	return 0;
}

#endif /* HAVE_DLOPEN */
