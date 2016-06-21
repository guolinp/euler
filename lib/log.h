#ifndef __LOG_H__
#define __LOG_H__

enum log_level {
	L_EMERG,
	L_ALERT,
	L_CRIT,
	L_ERR,
	L_WARNING,
	L_NOTICE,
	L_INFO,
	L_DEBUG,
};

void log_init(const char *logfile);
void log_deinit(void);
void log_set_level(enum log_level level);
void log_printf(enum log_level level, const char *format, ...);


#define log_emerg(format, args...)   log_printf(L_EMERG, format, ##args)
#define log_alert(format, args...)   log_printf(L_ALERT, format, ##args)
#define log_crit(format, args...)    log_printf(L_CRIT, format, ##args)
#define log_err(format, args...)     log_printf(L_ERR, format, ##args)
#define log_warning(format, args...) log_printf(L_WARNING, format, ##args)
#define log_notice(format, args...)  log_printf(L_NOTICE, format, ##args)
#define log_info(format, args...)    log_printf(L_INFO, format, ##args)
#define log_debug(format, args...)   log_printf(L_DEBUG, format, ##args)

#endif
