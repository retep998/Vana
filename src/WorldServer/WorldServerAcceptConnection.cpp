/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "WorldServerAcceptConnection.h"
#include "Channels.h"
#include "InterHeader.h"
#include "LoginServerConnectPacket.h"
#include "MapleSession.h"
#include "MiscUtilities.h"
#include "Rates.h"
#include "PacketReader.h"
#include "PlayerDataProvider.h"
#include "WorldServer.h"
#include "WorldServerAcceptHandler.h"
#include "WorldServerAcceptPacket.h"
#include <iostream>

WorldServerAcceptConnection::~WorldServerAcceptConnection() {
	if (isAuthenticated()) {
		if (getType() == InterChannelServer) {
			if (WorldServer::Instance()->isConnected()) {
				LoginServerConnectPacket::removeChannel(channel);
			}
			PlayerDataProvider::Instance()->removeChannelPlayers(channel);
			Channels::Instance()->removeChannel(channel);
			std::cout << "Channel " << channel << " disconnected." << std::endl;
		}
	}
}

void WorldServerAcceptConnection::realHandleRequest(PacketReader &packet) {
	if (!processAuth(packet, WorldServer::Instance()->getInterPassword())) return;
	switch (packet.get<int16_t>()) {
		case INTER_PLAYER_CHANGE_CHANNEL: WorldServerAcceptHandler::playerChangeChannel(this, packet); break;
		case INTER_TRANSFER_PLAYER_PACKET: WorldServerAcceptHandler::handleChangeChannel(this, packet); break;
		case INTER_REGISTER_PLAYER: WorldServerAcceptHandler::playerConnect(this, packet); break;
		case INTER_REMOVE_PLAYER: WorldServerAcceptHandler::playerDisconnect(this, packet); break;
		case INTER_PARTY_OPERATION: WorldServerAcceptHandler::partyOperation(this, packet); break;
		case INTER_UPDATE_LEVEL: WorldServerAcceptHandler::updateLevel(this, packet); break;
		case INTER_UPDATE_JOB: WorldServerAcceptHandler::updateJob(this, packet); break;
		case INTER_UPDATE_MAP: WorldServerAcceptHandler::updateMap(this, packet); break;

		case INTER_FIND: WorldServerAcceptHandler::findPlayer(this, packet); break;
		case INTER_WHISPER: WorldServerAcceptHandler::whisperPlayer(this, packet); break;
		case INTER_SCROLLING_HEADER: WorldServerAcceptHandler::scrollingHeader(this, packet); break;
		case INTER_GROUP_CHAT: WorldServerAcceptHandler::groupChat(this, packet); break;
		case INTER_TO_LOGIN: WorldServerAcceptHandler::sendToLogin(packet); break;
		case INTER_TO_CHANNELS: WorldServerAcceptHandler::sendToChannels(packet); break;
		case INTER_TO_PLAYERS: packet.reset(); WorldServerAcceptHandler::sendToChannels(packet); break;
	}
}

void WorldServerAcceptConnection::authenticated(int8_t type) {
	channel = Channels::Instance()->getAvailableChannel();
	if (type == InterChannelServer) {
		if (channel != -1) {
			uint16_t port = WorldServer::Instance()->getInterPort() + channel + 1;
			Channels::Instance()->registerChannel(this, channel, getIp(), getExternalIp(), port);
			WorldServerAcceptPacket::connect(this, channel, port);
			WorldServerAcceptPacket::sendRates(this, Rates::SetBits::all);
			WorldServerAcceptPacket::scrollingHeader(WorldServer::Instance()->getScrollingHeader());
			WorldServerAcceptPacket::sendParties(this);
			LoginServerConnectPacket::registerChannel(channel, getIp(), getExternalIp(), port);
			std::cout << "Assigned channel " << channel << " to channel server." << std::endl;
		}
		else {
			WorldServerAcceptPacket::connect(this, -1, 0);
			std::cout << "Error: No more channel to assign." << std::endl;
			getSession()->disconnect();
		}
	}
}
