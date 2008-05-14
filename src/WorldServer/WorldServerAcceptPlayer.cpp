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
#include "WorldSErverAcceptHandler.h"
#include "LoginServerConnectPlayerPacket.h"
#include "WorldServer.h"
#include "InterHeader.h"
#include "Channels.h"
#include <iostream>

void WorldServerAcceptPlayer::realHandleRequest(unsigned char *buf, int len) {
	processAuth(buf, (char *) WorldServer::Instance()->getInterPassword());
	short header = buf[0] + buf[1]*0x100;
	switch(header){
		case INTER_PLAYER_CHANGE_CHANNEL: WorldServerAcceptHandler::playerChangeChannel(this, buf+2); break;
		case INTER_TO_PLAYERS: WorldServerAcceptPlayerPacket::sendToChannels(buf, len); break;
		case INTER_REGISTER_PLAYER: WorldServerAcceptHandler::registerPlayer(this, buf+2); break;
		case INTER_REMOVE_PLAYER: WorldServerAcceptHandler::removePlayer(this, buf+2); break;
		case INTER_FIND: WorldServerAcceptHandler::findPlayer(this, buf+2); break;
	}
}

void WorldServerAcceptPlayer::authenticated(char type) {
	if (Channels::Instance()->size() < WorldServer::Instance()->getMaxChannels()) {
		channel = Channels::Instance()->size();
		short port = WorldServer::Instance()->getInterPort()+channel+1;
		Channels::Instance()->registerChannel(this, channel, ip, port);
		WorldServerAcceptPlayerPacket::connect(this, channel, port);
		LoginServerConnectPlayerPacket::registerChannel(WorldServer::Instance()->getLoginPlayer(), channel, ip, port);
		std::cout << "Assigned channel " << channel << " to channel server.";
	}
	else {
		WorldServerAcceptPlayerPacket::connect(this, -1, 0);
		std::cout << "Error: No more channel to assign.";
		disconnect();
	}
}
