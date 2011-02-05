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
#include "GmPacket.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void GmPacket::beginHide(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_GM);
	packet.add<int8_t>(0x10);
	packet.addBool(true);

	player->getSession()->send(packet);
}

void GmPacket::endHide(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_GM);
	packet.add<int8_t>(0x10);
	packet.addBool(false);

	player->getSession()->send(packet);
}

void GmPacket::warning(Player *player, bool succeed) {
	PacketCreator packet;
	packet.addHeader(SMSG_GM);
	packet.add<int8_t>(0x1d);
	packet.addBool(succeed);

	player->getSession()->send(packet);
}

void GmPacket::block(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_GM);
	packet.add<int8_t>(0x04);
	packet.add<int8_t>(0); // Might be something like succeed but it isn't displayed

	player->getSession()->send(packet);
}

void GmPacket::invalidCharacterName(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_GM);
	packet.add<int8_t>(0x06);
	packet.add<int8_t>(1);

	player->getSession()->send(packet);
}

void GmPacket::hiredMerchantPlace(Player *player, int8_t channel) {
	PacketCreator packet;
	packet.addHeader(SMSG_GM);
	packet.add<int8_t>(0x13);
	packet.add<int8_t>(0x01); // Mode, 00 = map, 01 = channel
	packet.add<int8_t>(channel); // 0xFE / -2 for 'Not found'

	player->getSession()->send(packet);
}

void GmPacket::hiredMerchantPlace(Player *player, int32_t mapid) {
	PacketCreator packet;
	packet.addHeader(SMSG_GM);
	packet.add<int8_t>(0x13);
	packet.add<int8_t>(0x00); // Mode, 00 = map, 01 = channel
	packet.add<int32_t>(mapid);

	player->getSession()->send(packet);
}

void GmPacket::setGetVarResult(Player *player, const string &name, const string &variable, const string &value) {
	PacketCreator packet;
	packet.addHeader(SMSG_GM);
	packet.add<int8_t>(0x09);
	packet.addString(name);
	packet.addString(variable);
	packet.addString(value);

	player->getSession()->send(packet);
}