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
#include "WorldServerConnectPlayer.h"
#include "WorldServerConnectPlayerPacket.h"
#include "WorldServerConnectHandler.h"
#include "PlayersPacket.h"
#include "InterHeader.h"

WorldServerConnectPlayer::WorldServerConnectPlayer() {
	type = INTER_CHANNEL_SERVER;
}

void WorldServerConnectPlayer::realHandleRequest(unsigned char *buf, int len) {
	short header = buf[0] + buf[1]*0x100;
	switch(header) {
		case INTER_LOGIN_CHANNEL_CONNECT: WorldServerConnectHandler::connectLogin(this, buf+2); break;
		case INTER_CHANNEL_CONNECT: WorldServerConnectHandler::connect(this, buf+2); break;
		case INTER_PLAYER_CHANGE_CHANNEL: WorldServerConnectHandler::playerChangeChannel(this, buf+2); break;
		case INTER_TO_PLAYERS: PlayersPacket::sendToPlayers(buf+2, len-2); break;
		case INTER_FIND: WorldServerConnectHandler::findPlayer(buf+2); break;
		case INTER_WHISPER: WorldServerConnectHandler::whisperPlayer(buf+2); break;
	}
}

void WorldServerConnectPlayer::playerChangeChannel(int playerid, int channel) {
	WorldServerConnectPlayerPacket::playerChangeChannel(this, playerid, channel);
}
