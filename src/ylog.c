/* ********************************************************
 * *                         YLOG                         *
 * ******************************************************** */

#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <syslog.h>
#include <stdio.h>
#include "ymem.h"
#include "ystr.h"
#include "ylog.h"

/* ******************* GLOBAL DATA ******************* */
/** @var _ylog_g Global variable which contains logging setup. */
static ylog_t _ylog_g = {
	.threshold = YLOG_WARN,
	.progname = NULL,
	.filename = NULL,
	.handler = NULL,
	.setup = YLOG_STDERR
};

/* *************** FUNCTIONS *************** */
void ylog_init(unsigned int setup, const char *progname, ...) {
	va_list ap;

	_ylog_g.progname = NULL;
	_ylog_g.filename = NULL;
	_ylog_g.handler = NULL;
	if (progname && progname[0])
		_ylog_g.progname = strdup(progname);
	va_start(ap, progname);
	if (setup & YLOG_FILE) {
		char *path = va_arg(ap, char*);
		_ylog_g.filename = strdup(path);
	}
	if (setup & YLOG_HANDLER)
		_ylog_g.handler = va_arg(ap, ylog_handler_t);
	else
		_ylog_g.handler = NULL;
	_ylog_g.setup = setup;
	va_end(ap);
}

void ylog_threshold(ylog_priority_t threshold) {
	_ylog_g.threshold = threshold;
}

void ylog_add(ylog_priority_t prio, const char *file, int line,
               const char *funcname, const char *str, ...) {
	time_t current_time;
	struct tm *tm;
	char *msg[] = {"DEBUG", "INFO", "NOTE", "WARN", "ERR", "CRIT"};
	va_list plist;
	char *tmpstr, *tmp2, *level;

	if (prio != YLOG_FORCE && prio < _ylog_g.threshold)
		return;
	va_start(plist, str);
	current_time = time(NULL);
	tm = localtime(&current_time);
	tmpstr = ys_new(NULL);
	tmp2 = ys_new(NULL);
	// create log string
	level = prio != YLOG_FORCE ? msg[(int)prio] : msg[(int)_ylog_g.threshold];
	ys_printf(&tmpstr, "(%s|%d)[%s] %s: %s", file ? file : "", line,
	          level, funcname ? funcname : "", str ? str : "");
	ys_vprintf(&tmp2, tmpstr, plist);
	va_end(plist);
	ys_trunc(tmpstr);
	// output to syslog
	if (_ylog_g.setup & YLOG_SYSLOG) {
		int syslog_prio = (prio == YLOG_INFO ? LOG_INFO :
		                   prio == YLOG_NOTE ? LOG_NOTICE :
		                   prio == YLOG_WARN ? LOG_WARNING :
		                   prio == YLOG_ERR ? LOG_ERR :
		                   prio == YLOG_CRIT ? LOG_CRIT : LOG_DEBUG);

		openlog((_ylog_g.progname ? _ylog_g.progname : ""), 0, LOG_USER);
		syslog(syslog_prio, "%s", tmp2);
		closelog();
		// return if it was only syslog logging
		if (_ylog_g.setup == YLOG_SYSLOG) {
			ys_free(&tmp2);
			ys_free(&tmpstr);
			return;
		}
	}
	// create extended log string
	ys_printf(&tmpstr, "%04d-%02d-%02d %02d:%02d:%02d %s%s\n",
	          tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	          tm->tm_hour, tm->tm_min, tm->tm_sec,
	          (_ylog_g.progname ? _ylog_g.progname : ""), tmp2);
	ys_free(&tmp2);
	// output to handler
	if (_ylog_g.handler) {
		_ylog_g.handler(tmpstr);
	}
	// output to stderr
	if (_ylog_g.setup & YLOG_STDERR) {
		fputs(tmpstr, stderr);
	}
	// output to file */
	if (_ylog_g.filename) {
		FILE *file;

		if ((file = fopen(_ylog_g.filename, "a"))) {
			fputs(tmpstr, file);
			fflush(file);
			fclose(file);
		}
	}
	ys_free(&tmpstr);
}

