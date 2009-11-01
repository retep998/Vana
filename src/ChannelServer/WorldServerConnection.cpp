/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "WorldServerConnection.h"
#include "ChannelServer.h"
#include "InterHeader.h"
#include "PacketReader.h"
#include "PartyHandler.h"
#include "PlayerActiveBuffs.h"
#include "PlayerDataProvider.h"
#include "PlayersPacket.h"
#include "SyncHandler.h"
#include "WorldServerConnectHandler.h"
#include "WorldServerConnectPacket.h"
#include <iostream>

WorldServerConnection::WorldServerConnection() {
	setType(InterChannelServer);
}

WorldServerConnection::~WorldServerConnection() {
	if (ChannelServer::Instance()->isConnected()) {
		std::cout << "Disconnected from the worldserver. Shutting down..." << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnection::realHandleRequest(PacketReader &packet) {
	switch (packet.get<int16_t>()) {
		case IMSG_LOGIN_CHANNEL_CONNECT: WorldServerConnectHandler::connectLogin(this, packet); break;
		case IMSG_CHANNEL_CONNECT: WorldServerConnectHandler::connect(this, packet); break;
		case IMSG_TO_PLAYERS: PlayersPacket::sendToPlayers(packet.getBuffer(), packet.getBufferLength()); break;
		case IMSG_FIND: WorldServerConnectHandler::findPlayer(packet); break;
		case IMSG_WHISPER: WorldServerConnectHandler::whisperPlayer(packet); break;
		case IMSG_SCROLLING_HEADER: WorldServerConnectHandler::scrollingHeader(packet); break;
		case IMSG_FORWARD_TO: WorldServerConnectHandler::forwardPacket(packet); break;
		case IMSG_SET_RATES: WorldServerConnectHandler::setRates(packet); break;
		case IMSG_REFRESH_DATA: WorldServerConnectHandler::reloadMcdb(packet); break;

		case IMSG_SYNC: SyncHandler::handleDataSync(packet); break;
		case IMSG_SYNC_OPERATION: PartyHandler::handleResponse(packet); break;
		case IMSG_TRANSFER_PLAYER_PACKET: PlayerDataProvider::Instance()->parseIncomingPacket(packet); break;
		case IMSG_TRANSFER_PLAYER_PACKET_DISCONNECT: PlayerDataProvider::Instance()->removePacket(packet.get<int32_t>()); break;
		case IMSG_GUILD_OPERATION: SyncHandler::guildPacketHandlerWorld(packet); break;
		case IMSG_ALLIANCE: SyncHandler::alliancePacketHandlerWorld(packet); break;
		case IMSG_NEW_CONNECTABLE: SyncHandler::newConnectable(packet); break;
		case IMSG_PLAYER_CHANGE_CHANNEL: SyncHandler::playerChangeChannel(this, packet); break;
	}
}