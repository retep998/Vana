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

#include "Party.h"
#include "Player.h"
#include "WorldServerConnectPlayerPacket.h"
#include "WorldServerConnectPlayer.h"
#include "ChannelServer.h"
#include "ReadPacket.h"

void Party::handleRequest(Player *player, ReadPacket *packet) {
	int8_t type = packet->getByte();
	switch(type){
		case 0x01: //Create party
		case 0x02: //Leaver party
			WorldServerConnectPlayerPacket::partyOperation(ChannelServer::Instance()->getWorldPlayer(), type, player->getId());
			break;
		case 0x03: //Join party
		case 0x05: //Expel Player
		case 0x06: //Give leader rights
			WorldServerConnectPlayerPacket::partyOperation(ChannelServer::Instance()->getWorldPlayer(), type, player->getId(), packet->getInt());
			break;
		case 0x04: //Invite
			WorldServerConnectPlayerPacket::partyInvite(ChannelServer::Instance()->getWorldPlayer(), player->getId(), packet->getString());
			break;
	}
}
