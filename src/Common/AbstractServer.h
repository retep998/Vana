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
#pragma once

#include "Configuration.h"
#include "ExternalIp.h"
#include "Ip.h"
#include "Logger.h"
#include "Types.h"
#include <memory>
#include <string>
#include <vector>

using std::string;
using std::vector;

class ConfigFile;
struct LogConfig;

class AbstractServer {
public:
	virtual ~AbstractServer() { }

	void initialize();
	virtual void listen() = 0;
	virtual void loadConfig() = 0;
	virtual void loadLogConfig() = 0;
	virtual void loadData() = 0;
	virtual opt_string makeLogIdentifier() = 0;
	virtual void shutdown();

	void displayLaunchTime() const;
	void initializeLoggingConstants(ConfigFile &conf) const;
	void createLogger(const LogConfig &conf);
	void setListening(bool toListen) { m_toListen = toListen; }
	void setServerType(int16_t type) { m_serverType = type; }
	void log(LogTypes::LogTypes type, const string &message);
	bool isListening() const { return m_toListen; }
	int16_t getServerType() const { return m_serverType; }
	const time_point_t & getStartTime() const { return m_startTime; }
	IpMatrix getExternalIps() const { return m_externalIps; }
	string getInterPassword() const { return m_interPassword; }
	string getSalt() const { return m_salt; }
	Logger * getLogger() const { return m_logger.get(); }
protected:
	AbstractServer();

	int16_t m_serverType;
	bool m_toListen;
	LoginConfig m_loginConfig;
	string m_interPassword;
	string m_salt;
	IpMatrix m_externalIps;
	time_point_t m_startTime;
	std::unique_ptr<Logger> m_logger;
private:
	typedef std::unordered_map<string, int32_t> ConstantMap;
	void loggerOptions(const ConstantMap &constants, ConfigFile &conf, const string &base, int32_t val, uint32_t depth) const;
};