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
#include "Channels.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/Session.hpp"
#include "WorldServer/Channel.hpp"
#include "WorldServer/LoginServerConnectPacket.hpp"
#include "WorldServer/WorldServer.hpp"
#include "WorldServer/WorldServerAcceptConnection.hpp"

namespace Vana {
namespace WorldServer {

auto Channels::registerChannel(WorldServerAcceptConnection *connection, channel_id_t channelId, const Ip &channelIp, const IpMatrix &extIp, port_t port) -> void {
	ref_ptr_t<Channel> chan = make_ref_ptr<Channel>(connection, channelId, port);
	chan->setExternalIpInformation(channelIp, extIp);
	m_channels[channelId] = chan;
}

auto Channels::removeChannel(channel_id_t channel) -> void {
	m_channels.erase(channel);
}

auto Channels::getChannel(channel_id_t num) -> Channel * {
	auto kvp = m_channels.find(num);
	return kvp != std::end(m_channels) ? kvp->second.get() : nullptr;
}

auto Channels::send(channel_id_t channelId, const PacketBuilder &builder) -> void {
	if (Channel *channel = getChannel(channelId)) {
		channel->send(builder);
	}
}

auto Channels::send(const vector_t<channel_id_t> &channels, const PacketBuilder &builder) -> void {
	for (const auto &channelId : channels) {
		send(channelId, builder);
	}
}

auto Channels::send(const PacketBuilder &builder) -> void {
	for (const auto &kvp : m_channels) {
		send(kvp.first, builder);
	}
}

auto Channels::increasePopulation(channel_id_t channel) -> void {
	WorldServer::getInstance().sendLogin(Packets::updateChannelPop(channel, getChannel(channel)->increasePlayers()));
}

auto Channels::decreasePopulation(channel_id_t channel) -> void {
	WorldServer::getInstance().sendLogin(Packets::updateChannelPop(channel, getChannel(channel)->decreasePlayers()));
}

auto Channels::getFirstAvailableChannelId() -> channel_id_t {
	channel_id_t channelId = -1;
	channel_id_t max = WorldServer::getInstance().getConfig().maxChannels;
	for (channel_id_t i = 0; i < max; ++i) {
		if (m_channels.find(i) == std::end(m_channels)) {
			channelId = i;
			break;
		}
	}
	return channelId;
}

auto Channels::disconnect() -> void {
	for (const auto &kvp : m_channels) {
		if (auto ref = kvp.second.get()) {
			ref->disconnect();
		}
	}
}

}
}