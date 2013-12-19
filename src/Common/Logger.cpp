/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "Logger.h"
#include "LogReplacements.h"
#include "TimeUtilities.h"
#include "VanaConstants.h"
#include <iomanip>
#include <iostream>
#include <sstream>

string Logger::getLevelString(LogTypes::LogTypes type) {
	string ret;
	switch (type) {
		case LogTypes::Info: ret = "INFO"; break;
		case LogTypes::Warning: ret = "WARNING"; break;
		case LogTypes::Debug: ret = "DEBUG"; break;
		case LogTypes::Error: ret = "ERROR"; break;
		case LogTypes::CriticalError: ret = "CRITICAL ERROR"; break;
		case LogTypes::ServerConnect: ret = "SERVER CONNECT"; break;
		case LogTypes::ServerDisconnect: ret = "SERVER DISCONNECT"; break;
		case LogTypes::ServerAuthFailure: ret = "SERVER AUTH FAILURE"; break;
		case LogTypes::Login: ret = "LOGIN"; break;
		case LogTypes::LoginAuthFailure: ret = "USER AUTH FAILURE"; break;
		case LogTypes::Logout: ret = "LOGOUT"; break;
		case LogTypes::ClientError: ret = "CLIENT ERROR"; break;
		case LogTypes::GmCommand: ret = "GM COMMAND"; break;
		case LogTypes::AdminCommand: ret = "ADMIN COMMAND"; break;
		case LogTypes::BossKill: ret = "BOSS KILL"; break;
		case LogTypes::Trade: ret = "TRADE"; break;
		case LogTypes::ShopTransaction: ret = "SHOP"; break;
		case LogTypes::StorageTransaction: ret = "STORAGE"; break;
		case LogTypes::InstanceBegin: ret = "INSTANCE"; break;
		case LogTypes::Drop: ret = "DROP"; break;
		case LogTypes::Chat: ret = "CHAT"; break;
		case LogTypes::Whisper: ret = "WHISPER"; break;
		case LogTypes::MalformedPacket: ret = "MALFORMED PACKET"; break;
		case LogTypes::ScriptLog: ret = "SCRIPT"; break;
		default: ret = "UNSUPPORTED";
	}
	return ret;
}

string Logger::getServerTypeString(int16_t serverType) {
	string ret;
	switch (serverType) {
		case ServerTypes::Cash: ret = "Cash"; break;
		case ServerTypes::Channel: ret = "Channel"; break;
		case ServerTypes::Login: ret = "Login"; break;
		case ServerTypes::Mts: ret = "MTS"; break;
		case ServerTypes::World: ret = "World"; break;
	}
	return ret;
}

string Logger::formatLog(LogTypes::LogTypes type, Logger *logger, const opt_string &id, const string &message) {
	return LogReplacements::format(logger->getFormat(), type, logger, time(nullptr), id, message);
}

Logger::Logger(const string &filename, const string &format, const string &timeFormat, int16_t serverType, size_t bufferSize) :
	m_format(format),
	m_timeFormat(timeFormat),
	m_serverType(serverType)
{
}