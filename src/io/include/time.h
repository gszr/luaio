#ifndef _TIME_H_
#define _TIME_H_

#include <sys/time.h>

#define difftime(t1, t2) ((t1) - (t2))

/* clock here returns milliseconds, not ticks count */
#define clock() \
		({ \
			struct rusage ru; \
			getrusage1(curlwp->l_proc, RUSAGE_SELF, &ru); \
			(clock_t)((ru.ru_utime.tv_sec + ru.ru_stime.tv_sec) * 1000 \
					+ (ru.ru_utime.tv_usec + ru.ru_stime.tv_usec) / 1000); \
		})
#define CLOCKS_PER_SEC 1

#define time(p) \
		({ \
			struct bintime t; \
			bintime(&t); \
			if (p != NULL) *(time_t*)p = t.sec; \
			else t.sec; \
			t.sec; \
		})


#endif
