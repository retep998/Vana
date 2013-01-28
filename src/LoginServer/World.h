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

#include "Channel.h"
#include "Configuration.h"
#include "Types.h"
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::function;
using std::vector;

class Channel;
class LoginServerAcceptConnection;
class PacketCreator;

class World {
public:
	World() : m_connected(false), m_playerLoad(0), m_connection(nullptr) { }

	void setConnected(bool connected) { m_connected = connected; }
	void setId(int8_t id) { m_id = id; }
	void setPort(port_t port) { m_port = port; }
	void setPlayerLoad(int32_t load) { m_playerLoad = load; }
	void setConnection(LoginServerAcceptConnection *connection) { m_connection = connection; }
	void setConfiguration(const WorldConfig &config) { m_config = config; }
	void setEventMessage(const string &message) { m_config.eventMsg = message; }
	void runChannelFunction(function<void (Channel *)> func);
	void clearChannels() { m_channels.clear(); }
	void removeChannel(int32_t id) { m_channels.erase(id); }
	void addChannel(int32_t id, Channel *channel) { m_channels[id].reset(channel); }
	void send(const PacketCreator &packet);

	bool isConnected() const { return m_connected; }
	int8_t getId() const { return m_id; }
	int8_t getRibbon() const { return m_config.ribbon; }
	port_t getPort() const { return m_port; }
	uint16_t getRandomChannel() const;
	size_t getMaxChannels() const { return m_config.maxChannels; }
	int32_t getPlayerLoad() const { return m_playerLoad; }
	int32_t getMaxPlayerLoad() const { return m_config.maxPlayerLoad; }
	Ip matchSubnet(const Ip &test);
	size_t getChannelCount() const { return m_channels.size(); }
	string getName() const { return m_config.name; }
	string getEventMessage() const { return m_config.eventMsg; }
	Channel * getChannel(int32_t id) { return (m_channels.find(id) != m_channels.end() ? m_channels[id].get() : nullptr); }
	const WorldConfig & getConfig() const { return m_config; }
private:
	typedef std::unordered_map<int32_t, std::shared_ptr<Channel>> ChannelMap;

	bool m_connected;
	int8_t m_id;
	port_t m_port;
	int32_t m_playerLoad;
	WorldConfig m_config;
	ChannelMap m_channels;
	LoginServerAcceptConnection *m_connection;
};