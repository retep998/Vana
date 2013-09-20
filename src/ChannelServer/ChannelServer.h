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

#include "AbstractServer.h"
#include "Configuration.h"
#include "Ip.h"
#include "noncopyable.hpp"
#include "Types.h"
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
	void loadData() override;
	void loadConfig() override;
	void loadLogConfig() override;
	void listen() override;
	void shutdown() override;
	void connectWorld();
	opt_string makeLogIdentifier() override;

	void setChannelId(int16_t channel) { m_channelId = channel; }
	void setWorldId(int8_t id) { m_world = id; }
	void setWorldPort(port_t port) { m_worldPort = port; }
	void setPort(port_t port) { m_port = port; }
	void setScrollingHeader(const string &message);
	void setWorldIp(const Ip &ip) { m_worldIp = ip; }
	void modifyRate(int32_t rateType, int32_t newValue);
	void setRates(const Rates &rates);
	void setConfig(const WorldConfig &config);

	bool isConnected() const { return m_channelId != -1; }
	int8_t getWorldId() const { return m_world; }
	uint8_t getMaxMultiLevel() const { return m_config.maxMultiLevel; }
	uint8_t getDefaultStorageSlots() const { return m_config.defaultStorageSlots; }
	int16_t getMaxStats() const { return m_config.maxStats; }
	int16_t getChannelId() const { return m_channelId; }
	int32_t getOnlineId() const { return 20000 + (int32_t) m_world * 100 + m_channelId; }
	int32_t getMobExpRate() const { return m_config.rates.mobExpRate; }
	int32_t getQuestExpRate() const { return m_config.rates.questExpRate; }
	int32_t getMobMesoRate() const { return m_config.rates.mobMesoRate; }
	int32_t getDropRate() const { return m_config.rates.dropRate; }
	int32_t getDefaultChars() const { return m_config.defaultChars; }
	int32_t getMaxChars() const { return m_config.maxChars; }
	int32_t getFameTime() const { return m_config.fameTime; }
	int32_t getFameResetTime() const { return m_config.fameResetTime; }
	int16_t getPianusAttempts() const { return m_config.pianus.attempts; }
	int16_t getPapAttempts() const { return m_config.pap.attempts; }
	int16_t getZakumAttempts() const { return m_config.zakum.attempts; }
	int16_t getHorntailAttempts() const { return m_config.horntail.attempts; }
	int16_t getPinkBeanAttempts() const { return m_config.pinkbean.attempts; }
	string getScrollingHeader() const { return m_config.scrollingHeader; }
	void sendPacketToWorld(PacketCreator &packet);

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

	WorldServerConnection * getWorldConnection() const { return m_worldConnection; }

	bool m_pianusChannel;
	bool m_papChannel;
	bool m_zakumChannel;
	bool m_horntailChannel;
	bool m_pinkbeanChannel;
	int8_t m_world;
	int16_t m_channelId;
	port_t m_worldPort;
	port_t m_loginPort;
	port_t m_port;
	Ip m_worldIp;
	Ip m_loginIp;
	WorldConfig m_config;
	WorldServerConnection *m_worldConnection;
};