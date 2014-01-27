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
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "SyncHandler.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptConnection.hpp"
#include "WorldServerAcceptPacket.hpp"

auto WorldServerAcceptHandler::groupChat(WorldServerAcceptConnection *connection, PacketReader &packet) -> void {
	int32_t playerId = packet.get<int32_t>();
	int8_t type = packet.get<int8_t>(); // Buddy = 0, party = 1, guild = 2, alliance = 3
	const string_t &message = packet.getString();
	const vector_t<int32_t> &receivers = packet.getVector<int32_t>();
	const string_t &sender = PlayerDataProvider::getInstance().getPlayer(playerId)->getName();
	for (size_t i = 0; i < receivers.size(); i++) {
		int32_t receiver = receivers[i];
		if (Player *p = PlayerDataProvider::getInstance().getPlayer(receiver)) {
			uint16_t channel = p->getChannel();
			WorldServerAcceptPacket::groupChat(channel, receiver, type, message, sender);
		}
	}
}

auto WorldServerAcceptHandler::findPlayer(WorldServerAcceptConnection *connection, PacketReader &packet) -> void {
	int32_t finder = packet.get<int32_t>();
	const string_t &findeeName = packet.getString();

	Player *findee = PlayerDataProvider::getInstance().getPlayer(findeeName);

	WorldServerAcceptPacket::findPlayer(connection, finder, findee->getChannel(), (findee->isOnline() ? findee->getName() : findeeName));
}

auto WorldServerAcceptHandler::whisperPlayer(WorldServerAcceptConnection *connection, PacketReader &packet) -> void {
	int32_t whisperer = packet.get<int32_t>();
	const string_t &whispereeName = packet.getString();
	const string_t &message = packet.getString();

	Player *whisperee = PlayerDataProvider::getInstance().getPlayer(whispereeName);
	if (whisperee->isOnline()) {
		WorldServerAcceptPacket::findPlayer(connection, whisperer, -1, whisperee->getName(), 1);
		WorldServerAcceptPacket::whisperPlayer(whisperee->getChannel(), whisperee->getId(), PlayerDataProvider::getInstance().getPlayer(whisperer)->getName(), connection->getChannel(),  message);
	}
	else {
		WorldServerAcceptPacket::findPlayer(connection, whisperer, whisperee->getChannel(), whispereeName);
	}
}

auto WorldServerAcceptHandler::sendPacketToChannels(PacketReader &packet) -> void {
	PacketCreator pack;
	pack.addBuffer(packet);
	Channels::getInstance().sendToAll(pack);
}

auto WorldServerAcceptHandler::sendPacketToLogin(PacketReader &packet) -> void {
	PacketCreator pack;
	pack.addBuffer(packet);
	WorldServer::getInstance().sendPacketToLogin(pack);
}