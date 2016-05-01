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
#include "AbstractServer.hpp"
#include "Common/AuthenticationPacket.hpp"
#include "Common/ComboLoggers.hpp"
#include "Common/ConfigFile.hpp"
#include "Common/ConnectionManager.hpp"
#include "Common/ConsoleLogger.hpp"
#include "Common/ExitCodes.hpp"
#include "Common/FileLogger.hpp"
#include "Common/HashUtilities.hpp"
#include "Common/LogConfig.hpp"
#include "Common/Logger.hpp"
#include "Common/MiscUtilities.hpp"
#include "Common/SaltingConfig.hpp"
#include "Common/Session.hpp"
#include "Common/SqlLogger.hpp"
#include "Common/ThreadPool.hpp"
#include "Common/TimerThread.hpp"
#include "Common/TimeUtilities.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace Vana {

AbstractServer::AbstractServer(ServerType type) :
	m_serverType{type},
	m_connectionManager{this}
{
}

auto AbstractServer::initialize() -> Result {
	m_startTime = TimeUtilities::getNow();

	loadLogConfig();

	auto config = ConfigFile::getConnectionPropertiesConfig();
	config->run();

	m_interPassword = config->get<string_t>("inter_password");
	m_salt = config->get<string_t>("inter_salt");

	if (m_interPassword == "changeme") {
		log(LogType::CriticalError, "inter_password is not changed.");
		ExitCodes::exit(ExitCodes::ConfigError);
		return Result::Failure;
	}
	if (m_salt == "changeme") {
		log(LogType::CriticalError, "inter_salt is not changed.");
		ExitCodes::exit(ExitCodes::ConfigError);
		return Result::Failure;
	}

	auto rawIpMap = config->get<vector_t<hash_map_t<string_t, string_t>>>("external_ip");
	for (const auto &pair : rawIpMap) {
		auto ipValue = pair.find("ip");
		auto maskValue = pair.find("mask");
		if (ipValue == std::end(pair) || maskValue == std::end(pair)) {
			log(LogType::CriticalError, "External IP configuration is malformed!");
			ExitCodes::exit(ExitCodes::ConfigError);
			return Result::Failure;
		}

		auto ip = Ip::stringToIpv4(ipValue->second);
		auto mask = Ip::stringToIpv4(maskValue->second);
		m_externalIps.push_back(ExternalIp{ip, mask});
	}

	m_interServerConfig = config->get<InterServerConfig>("");

	auto salting = ConfigFile::getSaltingConfig();
	salting->run();

	auto saltingConf = salting->get<SaltingConfig>("");
	m_saltingPolicy = saltingConf.interserver;

	if (loadConfig() == Result::Failure) {
		return Result::Failure;
	}

	if (loadData() == Result::Failure) {
		return Result::Failure;
	}
	initComplete();

	m_connectionManager.run();

	return Result::Successful;
}

auto AbstractServer::loadLogConfig() -> void {
	auto conf = ConfigFile::getLoggerConfig();
	conf->run();

	string_t prefix = getLogPrefix();
	LogConfig log;
	log = conf->get<LogConfig>(prefix);
	if (log.log) {
		createLogger(log);
	}
}

auto AbstractServer::shutdown() -> void {
	m_connectionManager.stop();
	ThreadPool::wait();
}

auto AbstractServer::getServerType() const -> ServerType {
	return m_serverType;
}

auto AbstractServer::getInterPassword() const -> string_t {
	return HashUtilities::hashPassword(m_interPassword, m_salt, m_saltingPolicy);
}

auto AbstractServer::getInterserverSaltingPolicy() const -> const SaltConfig & {
	return m_saltingPolicy;
}

auto AbstractServer::getInterServerConfig() const -> const InterServerConfig & {
	return m_interServerConfig;
}

auto AbstractServer::loadConfig() -> Result {
	// Intentionally left blank
	return Result::Successful;
}

auto AbstractServer::initComplete() -> void {
	// Intentionally left blank
}

auto AbstractServer::sendAuth(ref_ptr_t<Session> session) const -> void {
	session->send(
		Packets::sendPassword(
			MiscUtilities::getServerType(session->getType()),
			getInterPassword(),
			m_externalIps));
}

auto AbstractServer::createLogger(const LogConfig &conf) -> void {
	const string_t &timeFormat = conf.timeFormat;
	const string_t &format = conf.format;
	const string_t &file = conf.file;
	ServerType serverType = getServerType();
	size_t bufferSize = conf.bufferSize;

	switch (conf.destination) {
		case LogDestinations::Console: m_logger = make_owned_ptr<ConsoleLogger>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::File: m_logger = make_owned_ptr<FileLogger>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::Sql: m_logger = make_owned_ptr<SqlLogger>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::FileSql: m_logger = make_owned_ptr<DuoLogger<FileLogger, SqlLogger>>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::FileConsole: m_logger = make_owned_ptr<DuoLogger<FileLogger, ConsoleLogger>>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::SqlConsole: m_logger = make_owned_ptr<DuoLogger<SqlLogger, ConsoleLogger>>(file, format, timeFormat, serverType, bufferSize); break;
		case LogDestinations::FileSqlConsole: m_logger = make_owned_ptr<TriLogger<FileLogger, SqlLogger, ConsoleLogger>>(file, format, timeFormat, serverType, bufferSize); break;
	}
}

auto AbstractServer::log(LogType type, const string_t &message) -> void {
	if (Logger *logger = m_logger.get()) {
		logger->log(type, makeLogIdentifier(), message);
	}
}

auto AbstractServer::log(LogType type, function_t<void(out_stream_t &)> produceMessage) -> void {
	out_stream_t message;
	produceMessage(message);
	log(type, message.str());
}

auto AbstractServer::log(LogType type, const char *message) -> void {
	log(type, string_t{message});
}

auto AbstractServer::buildLogIdentifier(function_t<void(out_stream_t &)> produceId) const -> opt_string_t {
	out_stream_t message;
	produceId(message);
	return message.str();
}

auto AbstractServer::displayLaunchTime() const -> void {
	auto loadingTime = TimeUtilities::getDistance<milliseconds_t>(TimeUtilities::getNow(), m_startTime);
	std::cout << "Started in " << std::setprecision(3) << loadingTime / 1000.f << " seconds!" << std::endl << std::endl;
}

}