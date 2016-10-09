-- The following replacements are supported:
-- %yy = short year (99, 00, 10)
-- %YY = long year (1999, 2000, 2010)
-- %mm = integer month (1, 2, 12)
-- %MM = integer month (01, 02, 12)
-- %oo = string month (Jan, Feb, Dec)
-- %OO = string month (January, February, December)
-- %dd = integer date (1, 2, 30)
-- %DD = integer date (01, 02, 30)
-- %aa = string date (Mon, Tues, Sat)
-- %AA = string date (Monday, Tuesday, Saturday)
-- %hh = 12-hour (1, 2, 7)
-- %HH = 12-hour (01, 02, 07)
-- %mi = 24-hour (8, 12, 17)
-- %MI = 24-hour (08, 12, 17)
-- %ii = minute (7, 23, 54)
-- %II = minute (07, 23, 54)
-- %ss = second (7, 34, 44)
-- %SS = second (07, 34, 44)
-- %ww = am/pm
-- %qq = a/p
-- %WW = AM/PM
-- %QQ = A/P
-- %zz = timezone (+0600, -0800, 0000)
-- All replacement identifiers are case-sensitive

-- Logging configuration takes the form of a table
-- Required keys
-- log: a boolean indicating whether or not to log
-- destination: the place you wish to log to; options are system_log_none, system_log_all, system_log_file, system_log_sql, and system_log_console
-- -- You may combine specific log types by doing something such as system_log_file_sql_console or any variation thereof
-- format: a string that represents the form you want the log messages to take, discarded for SQL logging
-- -- String replacement constants:
-- -- %orig = server name
-- -- %e = event raised
-- -- %t = time raised (uses time format)
-- -- %id = additional identifier to clarify the entry
-- -- %msg = log message
-- -- All replacement identifiers are case-sensitive
-- Optional keys
-- time_format: an exclusive time format for the specific server, if not specified, the global one is used
-- buffer_size: buffer for limited resources like SQL and files, log items are cached until these are full
-- file: the path/file that you wish to log to, supports all above replacements (time and format)

-- The format that %t expands to (required)
log_time_format = "%MM/%DD/%YY %MI:%II:%SS";

-- Enable logging for a given type of server (applies to all of the given type)
channel = {
	["log"] = true,
	["destination"] = system_log_console,
	["format"] = "[%e (%t)] - %msg",
};

world = {
	["log"] = true,
	["destination"] = system_log_console,
	["format"] = "[%e (%t)] - %msg",
};

login = {
	["log"] = true,
	["destination"] = system_log_console,
	["format"] = "[%e (%t)] - %msg",
};

-- These servers don't exist
cash = {
	["log"] = false,
	["destination"] = system_log_sql,
	["format"] = "%msg",
	["buffer_size"] = 40,
};

mts = {
	["log"] = false,
	["destination"] = system_log_console_file,
	["format"] = "[%e (%t)] %id - %msg",
	["buffer_size"] = 40,
	["file"] = "logs/%orig/%YY%MM%DD.log",
};