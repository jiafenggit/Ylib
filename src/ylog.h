/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	ylog.h
 * @abstract	All definitions to use log files.
 * @discussion	The ylogs are a usefull mean to write logs.
 *		<ul>
 *		<li>You initialize the logs. There is four initialization 
 *		types, to specify the logs destination: the standard error 
 *		output, the system logger (syslog), to a given file, or to a 
 *		callback function (to send the logs through network, for 
 *		example). Here are some examples:
 *			<ul>
 *			<li>YLOG_INIT_STDERR();</li>
 *			<li>YLOG_INIT_SYSLOG();</li>
 *			<li>YLOG_INIT_FILE("/var/mylog");</li>
 *			<li>YLOG_INIT_HANDLER(pointer_to_function);</li>
 *			</ul><br />
 *		The init must be done in the main() function, which must have 
 *		its second parameter called "argv". The default output (without
 *		init) is set to the standard error output.<br /><br />
 *		</li>
 *		<li>It is possible to set a default priority level. This is the
 *		minimal level needed for a log to be written in log file (or 
 *		written on stderr). There is five levels (in growing order):
 *			<ul>
 *			<li>DEBUG: debug information for source code that 
 *			should be debugged</li>
 *			<li>INFO: information, messages during debug time</li>
 *			<li>NOTE: notice, normal but significant</li>
 *			<li>WARN: warning, non-blocking problem</li>
 *			<li>ERR: error, doesn't work as it should</li>
 *			<li>CRIT: critical, the program can't continue</li>
 *			</ul>
 *		Some macros helps you to set the default priority level:
 *			<ul>
 *			<li>YLOG_SET_DEBUG();</li>
 *			<li>YLOG_SET_INFO();</li>
 *			<li>YLOG_SET_NOTE();</li>
 *			<li>YLOG_SET_WARN();</li>
 *			<li>YLOG_SET_ERR();</li>
 *			<li>YLOG_SET_CRIT();</li>
 *			</ul>
 *		A normal running program will be set to WARN or ERR, thus at 
 *		debug time you will set to INFO (or DEBUG if you want to trace
 *		all the program). The default level is set to WARN.<br /><br />
 *		</li>
 *		<li>It is also possible, when the logs are written on stderr,
 *		to set the maximum log files size:
 *			<ul>
 *			<li>YLOG_SIZE_MINI();</li>
 *			<li>YLOG_SIZE_NORM();</li>
 *			<li>YLOG_SIZE_BIG();</li>
 *			<li>YLOG_SIZE_HUGE();</li>
 *			</ul>
 *		The size are 100 KO, 512 KO (default size), 2 MO and 10 MO. 
 *		When a file is full, it is moved with the date concatenate to 
 *		its name, and a new log file is created.
 *		<br /><br />
 *		</li>
 *		<li>You can add a log entry with this command:
 *			<ul>
 *			<li>YLOG_ADD(YLOG_ERR, "problem %d", 5);</li>
 *			</ul>
 *		As you can see, this macro takes a priority level, a string 
 *		that describes the log entry itself, and as many additional 
 *		parameters as needed (like in printf()). If the log's priority
 *		level is lower than the current default one, the log entry 
 *		isn't added.<br />
 *		There is another command, formerly created for temporary logs;
 *		for example, when you want to add some log entries for debug,
 *		you can put a simple:
 *			<ul>
 *			<li>YLOG("step 1");</li>
 *			<li>YLOG("step 2: '%s'", str);</li>
 *			</ul>
 *		The logs will be written with the minimal needed priority 
 *		level.<br /><br />
 *		</li>
 *		<li>At the end of the program, you close the opened log with 
 *		the command:
 *			<ul>
 *			<li>YLOG_END();</li>
 *			</ul>
 *		If some logs are added after this command, they will be 
 *		redirected to the standard error output.<br /><br />
 *		</li>
 *		<li><u>Additionnal feature: treatment by module</u><br />
 *		You can have some parts of a program that should not, usually,
 *		write logs, but sometimes they should. The priority level is
 *		not enough all the time to separate the differents parts, so
 *		there is a module treatment system.
 *			<ul>
 *			<li>First of all, you create an environment variable
 *			named 'YLOG_MODULES', that contains a list of module
 *			names (separate with colons) which are allowed to
 *			write logs.</li>
 *			<li>In the modules that should sometimes write some
 *			logs, use this macro to do it:
 *	<pre>YLOG_MOD("module_name", YLOG_PRIO_ERR, "problem %d", i);</pre>
 *			If the module name is not found in the YLOG_MODULES
 *			variable, the log is not written. If it is found, or if
 *			YLOG_MODULES is not defined, it is like a normal call
 *			to YLOG_ADD() (ie: the priority level is checked).
 *			</li>
 *			</ul>
 *		</li>
 *		</ul>
 * @version	1.0.0 Jun 26 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YLOG_H__
#define __YLOG_H__

/*! @define YLOG Add a simple log at the lowest sufficient level. The parameter
 * could be a simple character string, or a string with several arguments (like
 * in printf()). Return TRUE if the log entry was written, FALSE otherwise. */
#define YLOG(...)		ylog_add(YLOG_FORCE, __FILE__, __LINE__, \
					 __func__, __VA_ARGS__)
/*! @define YLOG_ADD Add a log with a specified priority. The last parameter 
 * could be a simple character string, or a string with several arguments (like
 * in printf()). Return TRUE if the log entry was written, FALSE otherwise. */
#define YLOG_ADD(prio, ...)	ylog_add(prio, __FILE__, __LINE__, \
					 __func__, __VA_ARGS__)

/*!
 * @enum	ylog_type_e
 *		All different destinations possible for logs.
 * @constant	YLOG_STDERR	Logs are written on the standard error output.
 * @constant	YLOG_FILE	Logs are written on a given file.
 * @constant	YLOG_SYSLOG	Logs are written on the system logger.
 * @constant	YLOG_HANDLER	Logs are sent to a given handler.
 */
enum ylog_type_e {
	YLOG_STDERR	= 1,
	YLOG_FILE	= 2,
	YLOG_SYSLOG	= 4,
	YLOG_HANDLER	= 8
};

/*! @typedef ylog_type_t Log destination. See ylog_type_e enumeration. */
typedef enum ylog_type_e ylog_type_t;

/*!
 * @enum	ylog_priority_e
 *		Several priority levels are defined in ylogs. They are similar
 *		to the syslog's ones (see 'man 3 syslog')
 * @constant	YLOG_FORCE	Force message writing (don't use it for threshold).
 * @constant	YLOG_DEBUG	Debug-level message (lower priority level).
 * @constant	YLOG_INFO	Informational message (default level).
 * @constant	YLOG_NOTE	Notice ; normal but significant.
 * @constant	YLOG_WARN	Warning.
 * @constant	YLOG_ERR	Error.
 * @constant	YLOG_CRIT	Critical message (higher priority level).
 */
enum ylog_priority_e {
	YLOG_FORCE = -1,
	YLOG_DEBUG = 0,
	YLOG_INFO,
	YLOG_NOTE,
	YLOG_WARN,
	YLOG_ERR,
	YLOG_CRIT
};

/*! @typedef ylog_priority_t Log priority levels. See ylog_priority_e. */
typedef enum ylog_priority_e ylog_priority_t;

/**
 * @typedef ylog_handler_t
 * Function pointer used for ylog handlers.
 * @param	str	Log string.
 */
typedef void (*ylog_handler_t)(const char *str);

/*!
 * @struct	ylog_s
 *		Main structure for ylogs. Must have one of it in global space.
 * @field	threshold	Current minimum priority level of written logs.
 * @field	progname	Name of the current program.
 * @field	filename	Path to the output file.
 * @field	handler		Function pointer to the log handler (set to 
 *				NULL for logs on stderr, syslog or file).
 * @field	setup		Log type (stderr, syslog, file).
 * @field	facility	Syslog facility.
 */
struct ylog_s {
	ylog_priority_t threshold;
	char *progname;
	char *filename;
	void (*handler)(const char*);
	int setup;
	int facility;
};

/*! @typedef ylog_t Main ylog structure. See ylog_main_s structure. */
typedef struct ylog_s ylog_t;

/**
 * @function	ylog_init
 *		Initialize the logging system.
 * @param	setup		Log type. Boolean comination of ylog_type_t values.
 * @param	progname	Name of the current process.
 * @param	...		List of other parameters, in this order:
 *				- Path to the log file.
 *				- Pointer to an handler function.
 */
void ylog_init(unsigned int setup, const char *progname, ...);

/**
 * @function	ylog_threshold
 *		Change the minimum priority level. Only messages with a greater or
 *		equal level will be written into logs.
 * @param	threshold	New default priority threashold.
 */
void ylog_threshold(ylog_priority_t threshold);

/**
 * @function	ylog_add
 *		Write a new log entry.
 * @param	prio	Priority level.
 * @param	file	Name of the file where this log entry is added.
 * @param	line	Number of the line where this log entry is added.
 * @param	func	Caller function's name.
 * @param	str	Main character string of the message.
 * @param	...	Variable arguments.
 */
void ylog_add(ylog_priority_t prio, const char *file, int line,
              const char *funcname, const char *str, ...);

#endif // __YLOG_H__
