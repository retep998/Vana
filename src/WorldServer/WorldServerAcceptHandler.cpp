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
#include "WorldServerAcceptHandler.hpp"
#include "Channel.hpp"
#include "Channels.hpp"
#include "InterHelper.hpp"
#include "PacketReader.hpp"
#include "PlayerDataProvider.hpp"
#include "SyncHandler.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptConnection.hpp"
#include "WorldServerAcceptPacket.hpp"

auto WorldServerAcceptHandler::sendPacketToLogin(PacketReader &packet) -> void {
	PacketCreator pack;
	pack.addBuffer(packet);
	WorldServer::getInstance().sendPacketToLogin(pack);
}

auto WorldServerAcceptHandler::sendPacketToChannel(PacketReader &packet) -> void {
	channel_id_t channelId = packet.get<channel_id_t>();
	PacketCreator pack;
	pack.addBuffer(packet);
	Channels::getInstance().sendToChannel(channelId, pack);
}

auto WorldServerAcceptHandler::sendPacketToChannelList(PacketReader &packet) -> void {
	vector_t<channel_id_t> channels = packet.getVector<channel_id_t>();
	PacketCreator pack;
	pack.addBuffer(packet);
	Channels::getInstance().sendToList(channels, pack);
}

auto WorldServerAcceptHandler::sendPacketToAllChannels(PacketReader &packet) -> void {
	PacketCreator pack;
	pack.addBuffer(packet);
	Channels::getInstance().sendToAll(pack);
}