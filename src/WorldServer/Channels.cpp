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
#include "Channels.hpp"
#include "Channel.hpp"
#include "LoginServerConnectPacket.hpp"
#include "PacketCreator.hpp"
#include "Session.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptConnection.hpp"

auto Channels::registerChannel(WorldServerAcceptConnection *connection, channel_id_t channelId, const Ip &channelIp, const IpMatrix &extIp, port_t port) -> void {
	ref_ptr_t<Channel> chan = make_ref_ptr<Channel>();
	chan->setConnection(connection);
	chan->setId(channelId);
	chan->setExternalIpInformation(channelIp, extIp);
	chan->setPort(port);
	m_channels[channelId] = chan;
}

auto Channels::removeChannel(channel_id_t channel) -> void {
	m_channels.erase(channel);
}

auto Channels::getChannel(channel_id_t num) -> Channel * {
	auto kvp = m_channels.find(num);
	return kvp != std::end(m_channels) ? kvp->second.get() : nullptr;
}

auto Channels::sendToChannel(channel_id_t channelId, const PacketCreator &packet) -> void {
	if (Channel *channel = getChannel(channelId)) {
		channel->send(packet);	
	}
}

auto Channels::sendToList(const vector_t<channel_id_t> &channels, const PacketCreator &packet) -> void {
	for (const auto &channelId : channels) {
		sendToChannel(channelId, packet);
	}
}

auto Channels::sendToAll(const PacketCreator &packet) -> void {
	for (const auto &kvp : m_channels) {
		sendToChannel(kvp.first, packet);
	}
}

auto Channels::increasePopulation(channel_id_t channel) -> void {
	LoginServerConnectPacket::updateChannelPop(channel, getChannel(channel)->increasePlayers());
}

auto Channels::decreasePopulation(channel_id_t channel) -> void {
	LoginServerConnectPacket::updateChannelPop(channel, getChannel(channel)->decreasePlayers());
}

auto Channels::size() -> channel_id_t {
	return m_channels.size();
}

auto Channels::getAvailableChannel() -> channel_id_t {
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