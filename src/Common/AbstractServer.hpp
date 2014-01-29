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

#include "Configuration.hpp"
#include "ExternalIp.hpp"
#include "Ip.hpp"
#include "Logger.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <vector>

class AbstractServerConnection;
class ConfigFile;
struct LogConfig;

class AbstractServer {
public:
	virtual ~AbstractServer() = default;

	auto initialize() -> AbstractServer &;
	virtual auto shutdown() -> void;

	auto log(LogType type, const string_t &message) -> void;
	auto log(LogType type, function_t<void(out_stream_t &)> produceMessage) -> void;
	auto log(LogType type, const char *message) -> void;
	auto getServerType() const -> ServerType;
	auto getInterPassword() const -> string_t;
protected:
	AbstractServer(ServerType type);
	virtual auto loadConfig() -> void;
	virtual auto initComplete() -> void;
	virtual auto listen() -> void = 0;
	virtual auto loadData() -> void = 0;
	virtual auto makeLogIdentifier() const -> opt_string_t = 0;
	virtual auto getLogPrefix() const -> string_t = 0;

	auto getInterServerConfig() const -> const InterServerConfig &;
	auto sendAuth(AbstractServerConnection *connection) const -> void;
	auto displayLaunchTime() const -> void;
	auto buildLogIdentifier(function_t<void(out_stream_t &)> produceId) const -> opt_string_t;
private:
	auto loadLogConfig() -> void;
	auto createLogger(const LogConfig &conf) -> void;
	auto initializeLoggingConstants(ConfigFile &conf) const -> void;
	auto loggerOptions(const hash_map_t<string_t, int32_t> &constants, ConfigFile &conf, const string_t &base, int32_t val, uint32_t depth) const -> void;

	ServerType m_serverType = ServerType::None;
	time_point_t m_startTime;
	string_t m_interPassword;
	string_t m_salt;
	owned_ptr_t<Logger> m_logger;
	InterServerConfig m_interServerConfig;
	IpMatrix m_externalIps;
};