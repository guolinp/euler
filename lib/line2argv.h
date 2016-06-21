#ifndef __LINE2ARGV_H__
#define __LINE2ARGV_H__

/* return argument number on success
 * return 0 on failed
 * the input 'str' will be modified
 * */
size_t line2argv(char *str, char *argv[], size_t size);

#endif
