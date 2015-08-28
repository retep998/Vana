/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "ConnectionManager.hpp"
#include "ExternalIp.hpp"
#include "InterServerConfig.hpp"
#include "Ip.hpp"
#include "LogConfig.hpp"
#include "Logger.hpp"
#include "SaltConfig.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Vana {
	class AbstractServerConnection;
	struct LogConfig;

	class AbstractServer {
	public:
		virtual ~AbstractServer() = default;

		auto initialize() -> Result;
		virtual auto shutdown() -> void;

		auto log(LogType type, const string_t &message) -> void;
		auto log(LogType type, function_t<void(out_stream_t &)> produceMessage) -> void;
		auto log(LogType type, const char *message) -> void;
		auto getServerType() const -> ServerType;
		auto getInterPassword() const -> string_t;
		auto getInterserverSaltingPolicy() const -> const SaltConfig &;
	protected:
		AbstractServer(ServerType type);
		virtual auto loadConfig() -> Result;
		virtual auto initComplete() -> void;
		virtual auto loadData() -> Result = 0;
		virtual auto makeLogIdentifier() const -> opt_string_t = 0;
		virtual auto getLogPrefix() const -> string_t = 0;

		auto getInterServerConfig() const -> const InterServerConfig &;
		auto sendAuth(AbstractServerConnection *connection) const -> void;
		auto displayLaunchTime() const -> void;
		auto buildLogIdentifier(function_t<void(out_stream_t &)> produceId) const -> opt_string_t;
		auto getConnectionManager() -> ConnectionManager & { return m_connectionManager; }
	private:
		auto loadLogConfig() -> void;
		auto createLogger(const LogConfig &conf) -> void;

		ServerType m_serverType = ServerType::None;
		time_point_t m_startTime;
		string_t m_interPassword;
		string_t m_salt;
		owned_ptr_t<Logger> m_logger;
		InterServerConfig m_interServerConfig;
		SaltConfig m_saltingPolicy;
		IpMatrix m_externalIps;
		ConnectionManager m_connectionManager;
	};
}