/*
* @author Aurelien Marchand 
*
*
* INSTALLATION
*
* gcc -o lcs.so -shared lcs.c -I /usr/include/mysql/
* (mac) gcc -bundle -o lcs.so lcs.c -I/usr/local/mysql/include
*
* Put the shared library as described in: http://dev.mysql.com/doc/refman/5.0/en/udf-compiling.html
* if SELINUX complains, run: chcon -t texrel_shlib_t /usr/lib/mysql/plugin/lcs.so
*
* Afterwards in SQL:
*
* CREATE FUNCTION lcs RETURNS STRING SONAME 'lcs.so';
* CREATE FUNCTION lcs_p RETURNS REAL SONAME 'lcs.so';
*
* -------------------------------------------------------------------------
*
* Some credit for simple levenshtein UDF to: Juan Miguel Cejuela
* and for LCS: rosettacode.org/wiki/Longest_Common_Subsequence
*
* -------------------------------------------------------------------------
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

/* (Expected) maximum number of digits to return */
#define LCS_MAX 255

/**
* LCS
*
* @param s string 1 to compare, length n
* @param t string 2 to compare, length m
* @result Longest Common Subsequence (LCS) between s and t
*
*/
my_bool lcs_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void lcs_deinit(UDF_INIT *initid);
char *lcs(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);


/**
* LCS Percentage
*
* @param s string 1 to compare, length n
* @param t string 2 to compare, length m
* @result LCS percentage between s and t
*
*/
my_bool lcs_p_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void lcs_p_deinit(UDF_INIT *initid);
double lcs_p(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

//-------------------------------------------------------------------------


my_bool lcs_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  if ((args->arg_count != 2) ||
      (args->arg_type[0] != STRING_RESULT || args->arg_type[1] != STRING_RESULT)) {
    strcpy(message, "Function requires 2 arguments, (string, string)");
    return 1;
  }

  initid->maybe_null = 1; // could return null
	initid->max_length = LCS_MAX;
  return 0;
}

void lcs_deinit(UDF_INIT *initid) {
	return;
}

char *lcs(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {
  int n = (args->args[0] == NULL) ? 0 : args->lengths[0];
  int m = (args->args[1] == NULL) ? 0 : args->lengths[1];

  if (0 == n || 0 == m || n > LCS_MAX || m > LCS_MAX)
  {
	*error = 1;
	*is_null = 1;
    return NULL;
  }

  char s[LCS_MAX];
  char t[LCS_MAX];
  strncpy(s,args->args[0],n);
  strncpy(t,args->args[1],m);
  s[n] = '\0';
  t[m] = '\0';

	*is_null = 0;

	int longest = 0;
	int match(const char *a, const char *b, int dep) {
		if (!a || !b) return 0;
		if (!*a || !*b) {
			if (dep <= longest) return 0;
			result[ longest = dep ] = 0;
			return 1;
		}
 
		if (*a == *b)
			return match(a + 1, b + 1, dep + 1) && (result[dep] = *a);
 
		return	match(a + 1, b + 1, dep) + 
			match(strchr(a, *b), b, dep) +
			match(a, strchr(b, *a), dep);
	}
 
	if(!match(s,t,0)) *is_null = 1;

	*length = longest;
	return result;
}



//-------------------------------------------------------------------------

my_bool lcs_p_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  if ((args->arg_count != 2) ||
      (args->arg_type[0] != STRING_RESULT || args->arg_type[1] != STRING_RESULT)) {
    strcpy(message, "Function requires 2 arguments, (string, string)");
    return 1;
  }

  initid->maybe_null = 0; //doesn't return null

  return 0;
}

void lcs_p_deinit(UDF_INIT *initid) {
	return;
}

double lcs_p(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
  unsigned long n = (args->args[0] == NULL) ? 0 : args->lengths[0];
  unsigned long m = (args->args[1] == NULL) ? 0 : args->lengths[1];

	if(n == 0 || m == 0) return 0.0;

	char result[LCS_MAX];
	unsigned long dist = 0;

  lcs(initid, args, result, &dist, is_null, error);
	*is_null = 0;
  return (double)2.0*dist/(n+m);
}

#endif /* HAVE_DLOPEN */
