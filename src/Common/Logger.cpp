/*
Copyright (C) 2008-2016 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "Logger.hpp"
#include "Common/ServerType.hpp"
#include "Common/TimeUtilities.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace vana {

base_logger::base_logger(const string &filename, const string &format, const string &time_format, server_type type, size_t buffer_size) :
	m_format{format},
	m_time_format{time_format},
	m_server_type{type}
{
}

auto base_logger::format_log(const string &format, log_type type, base_logger *logger, const opt_string &id, const string &message) -> string {
	return format_log(format, type, logger, time(nullptr), id, message);
}

auto base_logger::format_log(const string &format, log_type type, base_logger *logger, time_t time, const opt_string &id, const string &message) -> string {
	string ret = format;
	replacement_args args{type, logger, time, id, message};
	static log_replacements replacements;

	for (const auto &kvp : replacements.m_replacements) {
		size_t x = ret.find(kvp.first);
		if (x != string::npos) {
			out_stream strm;
			kvp.second(strm, args);
			const string &y = strm.str();
			ret.replace(x, kvp.first.size(), y.c_str(), y.size());
		}
	}
	return ret;
}

base_logger::replacement_args::replacement_args(log_type type, base_logger *logger, time_t time, const opt_string &id, const string &msg) :
	type{type},
	logger{logger},
	time{time},
	id{id},
	msg{msg}
{
}

base_logger::log_replacements::log_replacements() {
	add("%yy", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_year(true, args.time);
	});
	add("%YY", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_year(false, args.time);
	});
	add("%mm", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_month(args.time);
	});
	add("%MM", [](out_stream &stream, const replacement_args &args) {
		stream << std::setw(2) << std::setfill('0') << utilities::time::get_month(args.time);
	});
	add("%oo", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_month_string(true, args.time);
	});
	add("%OO", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_month_string(false, args.time);
	});
	add("%dd", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_date(args.time);
	});
	add("%DD", [](out_stream &stream, const replacement_args &args) {
		stream << std::setw(2) << std::setfill('0') << utilities::time::get_date(args.time);
	});
	add("%aa", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_day_string(true, args.time);
	});
	add("%AA", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_day_string(false, args.time);
	});
	add("%hh", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_hour(true, args.time);
	});
	add("%HH", [](out_stream &stream, const replacement_args &args) {
		stream << std::setw(2) << std::setfill('0') << utilities::time::get_hour(true, args.time);
	});
	add("%mi", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_hour(false, args.time);
	});
	add("%MI", [](out_stream &stream, const replacement_args &args) {
		stream << std::setw(2) << std::setfill('0') << utilities::time::get_hour(false, args.time);
	});
	add("%ii", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_minute(args.time);
	});
	add("%II", [](out_stream &stream, const replacement_args &args) {
		stream << std::setw(2) << std::setfill('0') << utilities::time::get_minute(args.time);
	});
	add("%ss", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_second(args.time);
	});
	add("%SS", [](out_stream &stream, const replacement_args &args) {
		stream << std::setw(2) << std::setfill('0') << utilities::time::get_second(args.time);
	});
	add("%ww", [](out_stream &stream, const replacement_args &args) {
		stream << !(utilities::time::get_hour(false, args.time) < 12) ? "pm" : "am";
	});
	add("%WW", [](out_stream &stream, const replacement_args &args) {
		stream << !(utilities::time::get_hour(false, args.time) < 12) ? "PM" : "AM";
	});
	add("%qq", [](out_stream &stream, const replacement_args &args) {
		stream << !(utilities::time::get_hour(false, args.time) < 12) ? "p" : "a";
	});
	add("%QQ", [](out_stream &stream, const replacement_args &args) {
		stream << !(utilities::time::get_hour(false, args.time) < 12) ? "P" : "A";
	});
	add("%zz", [](out_stream &stream, const replacement_args &args) {
		stream << utilities::time::get_time_zone();
	});
	add("%id", [](out_stream &stream, const replacement_args &args) {
		if (args.id.is_initialized()) {
			stream << args.id.get();
		}
	});
	add("%t", [](out_stream &stream, const replacement_args &args) {
		stream << base_logger::format_log(args.logger->get_time_format(), args.type, args.logger, args.time, args.id, args.msg);
	});
	add("%e", [](out_stream &stream, const replacement_args &args) {
		stream << get_level_string(args.type);
	});
	add("%orig", [](out_stream &stream, const replacement_args &args) {
		stream << get_server_type_string(args.logger->get_server_type());
	});
	add("%msg", [](out_stream &stream, const replacement_args &args) {
		stream << args.msg;
	});
}

auto base_logger::log_replacements::add(const string &key, func func) -> void {
	m_replacements.emplace(key, func);
}

auto base_logger::log_replacements::get_level_string(log_type type) -> string {
	switch (type) {
		case log_type::info: return "INFO";
		case log_type::warning: return "WARNING";
		case log_type::debug: return "DEBUG";
		case log_type::error: return "ERROR";
		case log_type::debug_error: return "DEBUG ERROR";
		case log_type::critical_error: return "CRITICAL ERROR";
		case log_type::hacking: return "HACKING";
		case log_type::server_connect: return "SERVER CONNECT";
		case log_type::server_disconnect: return "SERVER DISCONNECT";
		case log_type::server_auth_failure: return "SERVER AUTH FAILURE";
		case log_type::login: return "LOGIN";
		case log_type::login_auth_failure: return "USER AUTH FAILURE";
		case log_type::logout: return "LOGOUT";
		case log_type::client_error: return "CLIENT ERROR";
		case log_type::gm_command: return "GM COMMAND";
		case log_type::admin_command: return "ADMIN COMMAND";
		case log_type::boss_kill: return "BOSS KILL";
		case log_type::trade: return "TRADE";
		case log_type::shop_transaction: return "SHOP";
		case log_type::storage_transaction: return "STORAGE";
		case log_type::instance_begin: return "INSTANCE";
		case log_type::drop: return "DROP";
		case log_type::chat: return "CHAT";
		case log_type::whisper: return "WHISPER";
		case log_type::malformed_packet: return "MALFORMED PACKET";
		case log_type::script_log: return "SCRIPT";
		case log_type::ban: return "BAN";
		case log_type::unban: return "UNBAN";
	}

	throw not_implemented_exception{"LogType"};
}

auto base_logger::log_replacements::get_server_type_string(server_type type) -> string {
	switch (type) {
		case server_type::cash: return "Cash";
		case server_type::channel: return "Channel";
		case server_type::login: return "Login";
		case server_type::mts: return "MTS";
		case server_type::world: return "World";
	}
	throw not_implemented_exception{"ServerType"};
}

}