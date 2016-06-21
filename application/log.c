#include <stdio.h>

#include "log.h"

int main(int argc, char *argv[])
{
	log_init("this_is_a_log_example");

	log_printf(L_EMERG, "log message generated in program %s, level L_EMERG\n", argv[0]);
	log_printf(L_ALERT, "log message generated in program %s, level L_ALERT\n", argv[0]);
	log_printf(L_CRIT, "log message generated in program %s, level L_CRIT\n", argv[0]);
	log_printf(L_ERR, "log message generated in program %s, level L_ERR\n", argv[0]);
	log_printf(L_WARNING, "log message generated in program %s, level L_WARNING\n", argv[0]);
	log_printf(L_NOTICE, "log message generated in program %s, level L_NOTICE\n", argv[0]);
	log_printf(L_INFO, "log message generated in program %s, level L_INFO\n", argv[0]);
	log_printf(L_DEBUG, "log message generated in program %s, level L_DEBUG\n", argv[0]);

	log_emerg("log_emerg: log message generated in program %s\n", argv[0]);
	log_alert("log_alert: log message generated in program %s\n", argv[0]);
	log_crit("log_crit: log message generated in program %s\n", argv[0]);
	log_err("log_err: log message generated in program %s\n", argv[0]);
	log_warning("log_warning: log message generated in program %s\n", argv[0]);
	log_notice("log_notice: log message generated in program %s\n", argv[0]);
	log_info("log_info: log message generated in program %s\n", argv[0]);
	log_debug("log_debug: log message generated in program %s\n", argv[0]);

	log_deinit();

	return 0;
}
