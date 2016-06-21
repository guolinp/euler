#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

/* return 0 on faild
 * return microseconds From 1970-1-1 00:00:00
 */
unsigned long timestamp_microseconds(void);

/* return 0 on faild
 * return milliseconds From 1970-1-1 00:00:00
 */
unsigned long timestamp_milliseconds(void);

/* return 0 on faild
 * return seconds From 1970-1-1 00:00:00
 */
unsigned long timestamp_seconds(void);

/* return -1 on faild
 * return  0 on success and buf is filled with time string
 *           e.g: 2016-06-18 00:10:20
 */
int timestamp_string(char *buf, int buf_size);

#endif
