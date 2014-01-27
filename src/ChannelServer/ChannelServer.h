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

#include "AbstractServer.h"
#include "Configuration.h"
#include "Ip.h"
#include "Types.h"
#include <string>
#include <vector>

class PacketCreator;
class WorldServerConnection;

class ChannelServer : public AbstractServer {
	SINGLETON_CUSTOM_CONSTRUCTOR(ChannelServer);
public:
	auto loadData() -> void override;
	auto loadConfig() -> void override;
	auto loadLogConfig() -> void override;
	auto listen() -> void override;
	auto shutdown() -> void override;
	auto makeLogIdentifier() -> opt_string_t override;
	auto connectWorld() -> void;

	auto setChannelId(int16_t channel) -> void { m_channelId = channel; }
	auto setWorldId(int8_t id) -> void { m_world = id; }
	auto setWorldPort(port_t port) -> void { m_worldPort = port; }
	auto setPort(port_t port) -> void { m_port = port; }
	auto setScrollingHeader(const string_t &message) -> void;
	auto setWorldIp(const Ip &ip) -> void { m_worldIp = ip; }
	auto modifyRate(int32_t rateType, int32_t newValue) -> void;
	auto setRates(const Rates &rates) -> void;
	auto setConfig(const WorldConfig &config) -> void;

	auto isConnected() const -> bool { return m_channelId != -1; }
	auto getWorldId() const -> int8_t { return m_world; }
	auto getMaxMultiLevel() const -> uint8_t { return m_config.maxMultiLevel; }
	auto getDefaultStorageSlots() const -> uint8_t { return m_config.defaultStorageSlots; }
	auto getMaxStats() const -> int16_t { return m_config.maxStats; }
	auto getChannelId() const -> int16_t { return m_channelId; }
	auto getOnlineId() const -> int32_t { return 20000 + static_cast<int32_t>(m_world) * 100 + m_channelId; }
	auto getMobExpRate() const -> int32_t { return m_config.rates.mobExpRate; }
	auto getQuestExpRate() const -> int32_t { return m_config.rates.questExpRate; }
	auto getMobMesoRate() const -> int32_t { return m_config.rates.mobMesoRate; }
	auto getDropRate() const -> int32_t { return m_config.rates.dropRate; }
	auto getDefaultChars() const -> int32_t { return m_config.defaultChars; }
	auto getMaxChars() const -> int32_t { return m_config.maxChars; }
	auto getFameTime() const -> int32_t { return m_config.fameTime; }
	auto getFameResetTime() const -> int32_t { return m_config.fameResetTime; }
	auto getMapUnloadTime() const -> int32_t { return m_config.mapUnloadTime; }
	auto getPianusAttempts() const -> int16_t { return m_config.pianus.attempts; }
	auto getPapAttempts() const -> int16_t { return m_config.pap.attempts; }
	auto getZakumAttempts() const -> int16_t { return m_config.zakum.attempts; }
	auto getHorntailAttempts() const -> int16_t { return m_config.horntail.attempts; }
	auto getPinkBeanAttempts() const -> int16_t { return m_config.pinkbean.attempts; }
	auto getScrollingHeader() const -> string_t { return m_config.scrollingHeader; }
	auto sendPacketToWorld(PacketCreator &packet) -> void;

	// Specific bosses that can be battled on this channel
	auto isPianusChannel() const -> bool { return m_pianusChannel; }
	auto isPapChannel() const -> bool { return m_papChannel; }
	auto isZakumChannel() const -> bool { return m_zakumChannel; }
	auto isHorntailChannel() const -> bool { return m_horntailChannel; }
	auto isPinkBeanChannel() const -> bool { return m_pinkbeanChannel; }
	// Boss channel lists
	auto getPianusChannels() const -> vector_t<int8_t> { return m_config.pianus.channels; }
	auto getPapChannels() const -> vector_t<int8_t> { return m_config.pap.channels; }
	auto getZakumChannels() const -> vector_t<int8_t> { return m_config.zakum.channels; }
	auto getHorntailChannels() const -> vector_t<int8_t> { return m_config.horntail.channels; }
	auto getPinkBeanChannels() const -> vector_t<int8_t> { return m_config.pinkbean.channels; }
private:
	auto getWorldConnection() const -> WorldServerConnection * { return m_worldConnection; }

	bool m_pianusChannel = false;
	bool m_papChannel = false;
	bool m_zakumChannel = false;
	bool m_horntailChannel = false;
	bool m_pinkbeanChannel = false;
	int8_t m_world = -1;
	int16_t m_channelId = -1;
	port_t m_worldPort = 0;
	port_t m_loginPort = 0;
	port_t m_port = 0;
	Ip m_worldIp;
	Ip m_loginIp;
	WorldConfig m_config;
	WorldServerConnection *m_worldConnection = nullptr;
};