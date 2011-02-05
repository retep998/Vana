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
#include "PartyHandler.h"
#include "ChannelServer.h"
#include "InterHelper.h"
#include "PacketReader.h"
#include "Player.h"
#include "SyncPacket.h"

void PartyHandler::handleRequest(Player *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case PartyActions::Create: // Create party
		case PartyActions::Leave: // Leave party
			SyncPacket::partyOperation(ChannelServer::Instance()->getWorldConnection(), type, player->getId());
			break;
		case PartyActions::Join: // Join party
		case PartyActions::Expel: // Expel Player
		case PartyActions::SetLeader: // Give leader rights
			SyncPacket::partyOperation(ChannelServer::Instance()->getWorldConnection(), type, player->getId(), packet.get<int32_t>());
			break;
		case PartyActions::Invite: // Invite
			SyncPacket::partyInvite(ChannelServer::Instance()->getWorldConnection(), player->getId(), packet.getString());
			break;
	}
}