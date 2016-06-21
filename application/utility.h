#ifndef __UTILITY_H__
#define __UTILITY_H__

#define P(fmt, args...) \
	fprintf(stderr, fmt, ##args)

#define T(fmt, args...) \
	fprintf(stderr, "fun=%s, line=%d: " fmt, __func__, __LINE__,  ##args)

#define R() \
	fprintf(stderr, "# I am here: fun=%s, line=%d\n", __func__, __LINE__)

#define GODIE(fmt, args...) \
do { \
	T(fmt "\n", ##args); \
	exit(-1); \
} while(0)

#define GODIE_ON(condition, fmt, args...) \
do { \
	if(!!(condition)) \
		GODIE(fmt, ##args); \
} while(0)

#define MALLOC(size) \
({ \
	void * ____p = malloc(size); \
	GODIE_ON(!____p, "out of memory\n"); \
	____p; \
})

#define FREE(p) free(p)

#define NEW_THREAD(route, data) \
({ \
	pthread_t ____tid; \
	GODIE_ON(pthread_create(&____tid, NULL, route, data) != 0, "create thread failed\n"); \
	____tid; \
})

#define ARRAY_SIZE(array)  (sizeof(array) / sizeof(array[0]))

#define AWALYS_LOOPS() for(;;)

#endif
