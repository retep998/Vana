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

auto Channels::registerChannel(WorldServerAcceptConnection *connection, uint16_t channel, const Ip &channelIp, const IpMatrix &extIp, port_t port) -> void {
	ref_ptr_t<Channel> chan = make_ref_ptr<Channel>();
	chan->setConnection(connection);
	chan->setId(channel);
	chan->setExternalIpInformation(channelIp, extIp);
	chan->setPort(port);
	m_channels[channel] = chan;
}

auto Channels::removeChannel(uint16_t channel) -> void {
	m_channels.erase(channel);
}

auto Channels::getChannel(uint16_t num) -> Channel * {
	auto kvp = m_channels.find(num);
	return kvp != std::end(m_channels) ? kvp->second.get() : nullptr;
}

auto Channels::sendToAll(const PacketCreator &packet) -> void {
	for (const auto &kvp : m_channels) {
		sendToChannel(kvp.first, packet);
	}
}

auto Channels::sendToChannel(uint16_t channel, const PacketCreator &packet) -> void {
	getChannel(channel)->send(packet);
}

auto Channels::increasePopulation(uint16_t channel) -> void {
	LoginServerConnectPacket::updateChannelPop(channel, getChannel(channel)->increasePlayers());
}

auto Channels::decreasePopulation(uint16_t channel) -> void {
	LoginServerConnectPacket::updateChannelPop(channel, getChannel(channel)->decreasePlayers());
}

auto Channels::size() -> uint16_t {
	return m_channels.size();
}

auto Channels::getAvailableChannel() -> uint16_t {
	uint16_t channel = -1;
	uint16_t max = static_cast<uint16_t>(WorldServer::getInstance().getConfig().maxChannels);
	for (uint16_t i = 0; i < max; ++i) {
		if (m_channels.find(i) == std::end(m_channels)) {
			channel = i;
			break;
		}
	}
	return channel;
}