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

#include "ExternalIp.h"
#include "Ip.h"
#include "noncopyable.hpp"
#include "Types.h"
#include <memory>
#include <string>
#include <unordered_map>

using std::shared_ptr;
using std::string;
using std::unordered_map;

class Channel;
class PacketCreator;
class WorldServerAcceptConnection;

class Channels {
public:
	static Channels * Instance() {
		if (singleton == nullptr)
			singleton = new Channels;
		return singleton;
	}
	void registerChannel(WorldServerAcceptConnection *connection, uint16_t channel, const Ip &channelIp, const IpMatrix &extIp, port_t port);
	void removeChannel(uint16_t channel);
	Channel * getChannel(uint16_t num);
	void increasePopulation(uint16_t channel);
	void decreasePopulation(uint16_t channel);
	void sendToChannel(uint16_t channel, const PacketCreator &packet);
	void sendToAll(const PacketCreator &packet);
	uint16_t size();
	uint16_t getAvailableChannel();
private:
	Channels();
	static Channels *singleton;

	unordered_map<uint16_t, shared_ptr<Channel>> m_channels;
};