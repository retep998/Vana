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

#include "ExternalIp.hpp"
#include "Ip.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Channel;
class PacketCreator;
class WorldServerAcceptConnection;

class Channels {
	SINGLETON(Channels);
public:
	auto registerChannel(WorldServerAcceptConnection *connection, uint16_t channel, const Ip &channelIp, const IpMatrix &extIp, port_t port) -> void;
	auto removeChannel(uint16_t channel) -> void;
	auto getChannel(uint16_t num) -> Channel *;
	auto increasePopulation(uint16_t channel) -> void;
	auto decreasePopulation(uint16_t channel) -> void;
	auto sendToChannel(uint16_t channel, const PacketCreator &packet) -> void;
	auto sendToAll(const PacketCreator &packet) -> void;
	auto size() -> uint16_t;
	auto getAvailableChannel() -> uint16_t;
private:
	hash_map_t<uint16_t, ref_ptr_t<Channel>> m_channels;
};