/*
Copyright (C) 2008-2014 Vana Development Team

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
#pragma once

#include "Configuration.h"
#include "ExternalIp.h"
#include "Ip.h"
#include "Logger.h"
#include "Types.h"
#include <memory>
#include <string>
#include <vector>

class ConfigFile;
struct LogConfig;

class AbstractServer {
public:
	virtual ~AbstractServer() = default;

	auto initialize() -> void;
	virtual auto listen() -> void = 0;
	virtual auto loadConfig() -> void = 0;
	virtual auto loadLogConfig() -> void = 0;
	virtual auto loadData() -> void = 0;
	virtual auto makeLogIdentifier() -> opt_string_t = 0;
	virtual auto shutdown() -> void;

	auto displayLaunchTime() const -> void;
	auto initializeLoggingConstants(ConfigFile &conf) const -> void;
	auto createLogger(const LogConfig &conf) -> void;
	auto setListening(bool toListen) -> void { m_toListen = toListen; }
	auto setServerType(int16_t type) -> void { m_serverType = type; }
	auto log(LogTypes::LogTypes type, const string_t &message) -> void;
	auto isListening() const -> bool { return m_toListen; }
	auto getServerType() const -> int16_t { return m_serverType; }
	auto getStartTime() const -> const time_point_t & { return m_startTime; }
	auto getExternalIps() const -> IpMatrix { return m_externalIps; }
	auto getInterPassword() const -> string_t { return m_interPassword; }
	auto getSalt() const -> string_t { return m_salt; }
	auto getLogger() const -> Logger * { return m_logger.get(); }
protected:
	AbstractServer() = default;

	int16_t m_serverType = -1;
	bool m_toListen = false;
	time_point_t m_startTime;
	string_t m_interPassword;
	string_t m_salt;
	LoginConfig m_loginConfig;
	IpMatrix m_externalIps;
	owned_ptr_t<Logger> m_logger;
private:
	auto loggerOptions(const hash_map_t<string_t, int32_t> &constants, ConfigFile &conf, const string_t &base, int32_t val, uint32_t depth) const -> void;
};