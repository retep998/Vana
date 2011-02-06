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

#include "AbstractServer.h"
#include "Configuration.h"
#include "Types.h"
#include <boost/utility.hpp>
#include <string>

using std::string;

class WorldServerConnection;
class PacketCreator;

// ChannelServer main application class, implemented as singleton
class ChannelServer : public AbstractServer, boost::noncopyable {
public:
	static ChannelServer * Instance() {
		if (singleton == nullptr)
			singleton = new ChannelServer;
		return singleton;
	}
	void loadData();
	void loadConfig();
	void loadLogConfig();
	void listen();
	void shutdown();
	void connectWorld();
	string makeLogIdentifier();

	void setPianusChannel(bool isChannel) { m_pianusChannel = isChannel; }
	void setPapChannel(bool isChannel) { m_papChannel = isChannel; }
	void setZakumChannel(bool isChannel) { m_zakumChannel = isChannel; }
	void setHorntailChannel(bool isChannel) {m_horntailChannel = isChannel; }
	void setPinkBeanChannel(bool isChannel) { m_pinkbeanChannel = isChannel; }
	void setChannel(int16_t channel) { m_channel = channel; }
	void setWorld(int8_t id) { m_world = id; }
	void setWorldPort(uint16_t port) { m_worldPort = port; }
	void setPort(uint16_t port) { m_port = port; }
	void setScrollingHeader(const string &message);
	void setWorldIp(uint32_t ip) { m_worldIp = ip; }
	void setExpRate(int32_t rate) { m_config.expRate = rate; }
	void setQuestExpRate(int32_t rate) { m_config.questExpRate = rate; }
	void setDropRate(int32_t rate) { m_config.dropRate = rate; }
	void setMesoRate(int32_t rate) { m_config.mesoRate = rate; }
	void setConfig(const Configuration &config) { m_config = config; }

	bool isConnected() const { return m_channel != -1; }
	int8_t getWorld() const { return m_world; }
	uint8_t getMaxMultiLevel() const { return m_config.maxMultiLevel; }
	int16_t getMaxStats() const { return m_config.maxStats; }
	int16_t getChannel() const { return m_channel; }
	int32_t getOnlineId() const { return 20000 + (int32_t) m_world * 100 + m_channel; }
	int32_t getExpRate() const { return m_config.expRate; }
	int32_t getQuestExpRate() const { return m_config.questExpRate; }
	int32_t getMesoRate() const { return m_config.mesoRate; }
	int32_t getDropRate() const { return m_config.dropRate; }
	int32_t getMaxChars() const { return m_config.maxChars; }
	int16_t getPianusAttempts() const { return m_config.pianus.attempts; }
	int16_t getPapAttempts() const { return m_config.pap.attempts; }
	int16_t getZakumAttempts() const { return m_config.zakum.attempts; }
	int16_t getHorntailAttempts() const { return m_config.horntail.attempts; }
	int16_t getPinkBeanAttempts() const { return m_config.pinkbean.attempts; }
	string getScrollingHeader() const { return m_config.scrollingHeader; }
	WorldServerConnection * getWorldConnection() const { return m_worldConnection; }
	void sendToWorld(PacketCreator &packet);

	// Specific bosses that can be battled on this channel
	bool isPianusChannel() const { return m_pianusChannel; }
	bool isPapChannel() const { return m_papChannel; }
	bool isZakumChannel() const { return m_zakumChannel; }
	bool isHorntailChannel() const { return m_horntailChannel; }
	bool isPinkBeanChannel() const { return m_pinkbeanChannel; }
	// Boss channel lists
	vector<int8_t> getPianusChannels() const { return m_config.pianus.channels; }
	vector<int8_t> getPapChannels() const { return m_config.pap.channels; }
	vector<int8_t> getZakumChannels() const { return m_config.zakum.channels; }
	vector<int8_t> getHorntailChannels() const { return m_config.horntail.channels; }
	vector<int8_t> getPinkBeanChannels() const { return m_config.pinkbean.channels; }
private:
	ChannelServer();
	static ChannelServer *singleton;

	WorldServerConnection *m_worldConnection;

	bool m_pianusChannel;
	bool m_papChannel;
	bool m_zakumChannel;
	bool m_horntailChannel;
	bool m_pinkbeanChannel;
	int8_t m_world;
	int16_t m_channel;
	uint16_t m_worldPort;
	uint16_t m_loginPort;
	uint16_t m_port;
	uint32_t m_worldIp;
	uint32_t m_loginIp;
	Configuration m_config;
};
