/*
Copyright (C) 2008 Vana Development Team

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
#include "WorldServerAcceptPlayer.h"
#include "WorldServerAcceptPlayerPacket.h"
#include "WorldServerAcceptHandler.h"
#include "LoginServerConnectPlayerPacket.h"
#include "WorldServer.h"
#include "InterHeader.h"
#include "Channels.h"
#include "ReadPacket.h"
#include "Rates.h"
#include <iostream>

void WorldServerAcceptPlayer::realHandleRequest(ReadPacket *packet) {
	if (!processAuth(packet, WorldServer::Instance()->getInterPassword())) return;
	switch(packet->getShort()) {
		case INTER_PLAYER_CHANGE_CHANNEL: WorldServerAcceptHandler::playerChangeChannel(this, packet); break;
		case INTER_TO_PLAYERS: packet->reset(); WorldServerAcceptPlayerPacket::sendToChannels(packet->getBuffer(), packet->getBufferLength()); break;
		case INTER_REGISTER_PLAYER: WorldServerAcceptHandler::registerPlayer(this, packet); break;
		case INTER_REMOVE_PLAYER: WorldServerAcceptHandler::removePlayer(this, packet); break;
		case INTER_FIND: WorldServerAcceptHandler::findPlayer(this, packet); break;
		case INTER_WHISPER: WorldServerAcceptHandler::whisperPlayer(this, packet); break;
		case INTER_SCROLLING_HEADER: WorldServerAcceptHandler::scrollingHeader(this, packet); break;
		case INTER_PARTY_OPERATION: WorldServerAcceptHandler::partyOperation(this, packet); break;
		case INTER_UPDATE_LEVEL: WorldServerAcceptHandler::updateLevel(this, packet); break;
		case INTER_UPDATE_JOB: WorldServerAcceptHandler::updateJob(this, packet); break;
		case INTER_UPDATE_MAP: WorldServerAcceptHandler::updateMap(this, packet); break;
		case INTER_GROUP_CHAT: WorldServerAcceptHandler::groupChat(this, packet); break;
	}
}

void WorldServerAcceptPlayer::authenticated(char type) {
	if (Channels::Instance()->size() < WorldServer::Instance()->getMaxChannels()) {
		channel = Channels::Instance()->size();
		short port = WorldServer::Instance()->getInterPort()+channel+1;
		Channels::Instance()->registerChannel(this, channel, ip, port);
		WorldServerAcceptPlayerPacket::connect(this, channel, port);
		WorldServerAcceptPlayerPacket::sendRates(this, Rates::SetBits::all);
		WorldServerAcceptPlayerPacket::scrollingHeader(WorldServer::Instance()->getScrollingHeader());
		LoginServerConnectPlayerPacket::registerChannel(WorldServer::Instance()->getLoginPlayer(), channel, ip, port);
		std::cout << "Assigned channel " << channel << " to channel server.";
	}
	else {
		WorldServerAcceptPlayerPacket::connect(this, -1, 0);
		std::cout << "Error: No more channel to assign.";
		disconnect();
	}
}
