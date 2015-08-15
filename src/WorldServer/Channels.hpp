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

#include "ExternalIp.hpp"
#include "Ip.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Channel;
class PacketBuilder;
class WorldServerAcceptConnection;

class Channels {
public:
	auto registerChannel(WorldServerAcceptConnection *connection, channel_id_t channelId, const Ip &channelIp, const IpMatrix &extIp, port_t port) -> void;
	auto removeChannel(channel_id_t channelId) -> void;
	auto getChannel(channel_id_t num) -> Channel *;
	auto increasePopulation(channel_id_t channelId) -> void;
	auto decreasePopulation(channel_id_t channelId) -> void;
	auto getFirstAvailableChannelId() -> channel_id_t;
	auto disconnect() -> void;
	auto send(channel_id_t channelId, const PacketBuilder &builder) -> void;
	auto send(const vector_t<channel_id_t> &channels, const PacketBuilder &builder) -> void;
	auto send(const PacketBuilder &builder) -> void;
private:
	hash_map_t<channel_id_t, ref_ptr_t<Channel>> m_channels;
};