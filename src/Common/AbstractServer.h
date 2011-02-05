/*
Copyright (C) 2008-2011 Vana Development Team

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

#include "Ip.h"
#include "Logger.h"
#include "Types.h"
#include <boost/scoped_ptr.hpp>
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
	virtual string makeLogIdentifier() = 0;
	virtual void shutdown();

	void displayLaunchTime() const;
	void initializeLoggingConstants(ConfigFile &conf) const;
	void createLogger(const LogConfig &conf);
	void setListening(bool toListen) { m_toListen = toListen; }
	void setServerType(int16_t type) { m_serverType = type; }
	void log(LogTypes::LogTypes type, const string &message);
	bool isListening() const { return m_toListen; }
	int16_t getServerType() const { return m_serverType; }
	clock_t getStartTime() const { return m_startTime; }
	IpMatrix getExternalIp() const { return m_externalIp; }
	string getInterPassword() const { return m_interPassword; }
	Logger * getLogger() const { return m_logger.get(); }
protected:
	AbstractServer();

	int16_t m_serverType;
	clock_t m_startTime;
	bool m_toListen;
	string m_interPassword;
	IpMatrix m_externalIp;
	boost::scoped_ptr<Logger> m_logger;
};
