-- Enable logging for a given type of server (applies to all of the given type)
log_channel = true;
log_world = true;
log_login = true;
log_cash = false; -- These servers don't exist
log_mts = false;

-- The format that %t expands to
log_time_format = "%MM/%DD/%YY %MI:%II:%SS";

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

-- Logging options
-- *_log_destination: the place you wish to log to; options are system_log_none, system_log_all, system_log_file, system_log_sql, and system_log_console
-- You may combine specific log types by doing something such as system_log_file_sql_console or any variation thereof

-- *_log_buffer_size: buffer for limited resources like SQL and files, log items are cached until these are full

-- *_log_format: a string that represents the form you want the log messages to take, discarded for SQL logging

-- String replacement constants:
-- %orig = server name
-- %e = event raised
-- %t = time raised (uses time format)
-- %id = additional identifier to clarify the entry
-- %msg = log message
-- All replacement identifiers are case-sensitive

-- *_log_file: the path/file that you wish to log to, supports all above replacements (time and format)

-- Login logging
login_log_destination = system_log_console;
login_log_buffer_size = 40;
login_log_format = "[%e (%t)] - %msg";
login_log_file = "logs/%orig/%YY%MM%DD.log";

-- World logging
world_log_destination = system_log_console;
world_log_buffer_size = 40;
world_log_format = "[%e (%t)] %id - %msg";
world_log_file = "logs/%orig/%YY%MM%DD.log";

-- Channel logging
channel_log_destination = system_log_console;
channel_log_buffer_size = 40;
channel_log_format = "[%e (%t)] %id - %msg";
channel_log_file = "logs/%orig/%YY%MM%DD.log";

-- Cash logging
cash_log_destination = system_log_sql;
cash_log_buffer_size = 40;
cash_log_format = "%msg";
cash_log_file = "logs/%orig/%YY%MM%DD.log";

-- MTS logging
mts_log_destination = system_log_file;
mts_log_buffer_size = 40;
mts_log_format = "[%e (%t)] %id - %msg";
mts_log_file = "logs/%orig/%YY%MM%DD.log";
