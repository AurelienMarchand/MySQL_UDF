# MySQL_UDF
Collection of User-Defined Functions for MySQL

## Longest Common Substring (LCS)
Case-sensitive function used to determine what substring is common to 2 input strings.

### Compilation and Installation
    gcc -o lib_lcs.so -shared lcs.c -I/usr/include/mysql

Then place lib_lcs.so to the location indicated in http://dev.mysql.com/doc/refman/5.0/en/udf-compiling.html. On my machine, it's /usr/lib/mysql/plugins/lib_lcs.so (you might need to create a symlink between /usr/lib/mysql/plugins/ and /usr/lib/)

    mv lib_lcs.so /usr/lib/mysql/plusings/lib_lcs.so
    cd /usr/lib/
    ln -s mysql/plugins/lib_lcs.so lib_lcs.so

If SELINUX complains, run

    chcon -t texrel_shlib_t /usr/lib/mysql/plugins/lib_lcs.so

Finally, once you have logged on your MySQL server, run:

    CREATE FUNCTION lcs RETURNS STRING SONAME 'lib_lcs.so';


### Examples
SELECT lcs('**a**u**r**elie**n**','m**ar**cha**n**d') returns "arn"

SELECT lcs('AURELIEN','marchand') returns NULL

SELECT lcs('employer cor**por**a**tion** **y**es','**portion** plo**y**') returns "portion y"

## Levenshtein
Attribution: Code from https://github.com/juanmirocks/Levenshtein-MySQL-UDF/blob/master/levenshtein.c

Retrieves the number of changes to go from string A to string B. A change is defined as: adding, removing, or substituting a character. Optionally die if the number passes a certain threshold of changes. This function is case-sensitive.

### Compilation and Installation
    gcc -o lib_levenshtein.so -shared levenshtein.c -I/usr/include/mysql

Then place lib_levenshtein.so to the location indicated in http://dev.mysql.com/doc/refman/5.0/en/udf-compiling.html. On my machine, it's /usr/lib/mysql/plugins/lib_lcs.so (you might need to create a symlink between /usr/lib/mysql/plugins/ and /usr/lib/)

    mv lib_levenshtein.so /usr/lib/mysql/plusings/lib_levenshtein.so
    cd /usr/lib/
    ln -s mysql/plugins/lib_levenshtein.so lib_levenshtein.so

If SELINUX complains, run

    chcon -t texrel_shlib_t /usr/lib/mysql/plugins/lib_levenshtein.so

Finally, once you have logged on your MySQL server, run:

    CREATE FUNCTION levenshtein RETURNS INT SONAME 'lib_levenshtein.so';
    CREATE FUNCTION levenshtein_k RETURNS INT SONAME 'lib_levenshtein.so';
    CREATE FUNCTION levenshtein_ratio RETURNS REAL SONAME 'lib_levenshtein.so';
    CREATE FUNCTION levenshtein_k_ratio RETURNS REAL SONAME 'lib_levenshtein.so';


### Examples
SELECT levenshtein('A~~u~~r~~e~~l~~i~~en','Arlen**e**') returns 4 (suppress u, e, i, then add e)

SELECT levenshtein('Aurelien','Marchand') returns 7 (substitute A for M, u for a, e for c, l for h, i for a, suppress e, add d)

SELECT levenshtein_k('Aurelien','Marchand',4) returns 5, meaning that the function quit after making the 4th change.
    
SELECT levenshtein_ratio('Aurelien','Marchand') returns 0.125, meaning 12.5% of the string remained intact.

SELECT levenshtein_ratio('Aurelien','Aurelien') returns 1, meaning 100% of the string remained intact.

SELECT levenshtein_k_ratio('Aurelien','Marchand',5) returns 0, meaning it was not possible to convert from string A to string B in 5 changes or less.

SELECT levenshtein_k_ratio('Aurelien','Marchand',8) returns 0.125, meaning it was possible to convert from string A to string B in 8 changes or less and 12.5% of the original string remained intact.

## Signal
Inspiration: http://rpbouman.blogspot.com/2005/11/using-udf-to-raise-errors-from-inside.html

Function call that will raise an error. This function is useless on its own, but invaluable in triggers, stored procedures or function.

### Compilation and Installation
    gcc -o lib_signal.so -shared signal.c -I/usr/include/mysql

Then place lib_signal.so to the location indicated in http://dev.mysql.com/doc/refman/5.0/en/udf-compiling.html. On my machine, it's /usr/lib/mysql/plugins/lib_lcs.so (you might need to create a symlink between /usr/lib/mysql/plugins/ and /usr/lib/)

    mv lib_signal.so /usr/lib/mysql/plusings/lib_signal.so
    cd /usr/lib/
    ln -s mysql/plugins/lib_signal.so lib_signal.so

If SELINUX complains, run

    chcon -t texrel_shlib_t /usr/lib/mysql/plugins/lib_signal.so

Finally, once you have logged on your MySQL server, run:

    CREATE FUNCTION signal RETURNS STRING SONAME 'lib_signal.so';

The sole parameter is required and must be a string less than 255 characters.

### Examples
    SELECT signal('some error message') returns "ERROR: some error message".
    
    DELIMITER ;;
    CREATE TRIGGER `result_bu` BEFORE UPDATE ON `result` FOR EACH ROW
    BEGIN
    /* if property "committed" is set to "true", we cannot change any data, ever */
    IF(OLD.committed = 'true')
    THEN
        SELECT signal('no change allowed when record has been committed') INTO @error;
    END IF;
    END
    ;;
    DELIMITER ;
