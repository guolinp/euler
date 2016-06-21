#include <syslog.h>
#include <stdarg.h>

#include "log.h"

void log_init(const char *logfile)
{
	openlog(logfile, LOG_CONS | LOG_PID, 0);
}

void log_deinit(void)
{
	closelog();
}

void log_set_level(enum log_level level)
{
	return;
}

static inline int log_level_to_syslog_priority(enum log_level level)
{
	switch (level) {
	case L_EMERG:
		return LOG_EMERG;
	case L_ALERT:
		return LOG_ALERT;
	case L_CRIT:
		return LOG_CRIT;
	case L_ERR:
		return LOG_ERR;
	case L_WARNING:
		return LOG_WARNING;
	case L_NOTICE:
		return LOG_NOTICE;
	case L_INFO:
		return LOG_INFO;
	case L_DEBUG:
		return LOG_DEBUG;
	}
}

void log_printf(enum log_level level, const char *format, ...)
{
	va_list vargs;
	int priority = log_level_to_syslog_priority(level);

	va_start(vargs, format);
	vsyslog(priority, format, vargs);
	va_end(vargs);
}
