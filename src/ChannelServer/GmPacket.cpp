/*
Copyright (C) 2008-2010 Vana Development Team

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
#include "GmPacket.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void GmPacket::beginHide(Player *player) {
	PacketCreator packet = PacketCreator();
	packet.add<int16_t>(SMSG_GM);
	packet.add<int8_t>(0x10);
	packet.addBool(true);
	
	player->getSession()->send(packet);
}

void GmPacket::endHide(Player *player) {
	PacketCreator packet = PacketCreator();
	packet.add<int16_t>(SMSG_GM);
	packet.add<int8_t>(0x10);
	packet.addBool(false);
	
	player->getSession()->send(packet);
}

void GmPacket::warning(Player *player, bool succeed) {
	PacketCreator packet = PacketCreator();
	packet.add<int16_t>(SMSG_GM);
	packet.add<int8_t>(0x1d);
	packet.addBool(succeed);
	
	player->getSession()->send(packet);
}

void GmPacket::block(Player *player) {
	PacketCreator packet = PacketCreator();
	packet.add<int16_t>(SMSG_GM);
	packet.add<int8_t>(0x04);
	packet.add<int8_t>(0); // Might be something like succeed but it isn't displayed
	
	player->getSession()->send(packet);
}

void GmPacket::invalidCharacterName(Player *player) {
	PacketCreator packet = PacketCreator();
	packet.add<int16_t>(SMSG_GM);
	packet.add<int8_t>(0x06);
	packet.add<int8_t>(1);
	
	player->getSession()->send(packet);
}