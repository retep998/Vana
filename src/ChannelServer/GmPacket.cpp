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
#include "GmPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Session.h"
#include "SmsgHeader.h"

auto GmPacket::beginHide(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_GM);
	packet.add<int8_t>(0x10);
	packet.add<bool>(true);

	player->getSession()->send(packet);
}

auto GmPacket::endHide(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_GM);
	packet.add<int8_t>(0x10);
	packet.add<bool>(false);

	player->getSession()->send(packet);
}

auto GmPacket::warning(Player *player, bool succeed) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_GM);
	packet.add<int8_t>(0x1d);
	packet.add<bool>(succeed);

	player->getSession()->send(packet);
}

auto GmPacket::block(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_GM);
	packet.add<int8_t>(0x04);
	packet.add<int8_t>(0); // Might be something like succeed but it isn't displayed

	player->getSession()->send(packet);
}

auto GmPacket::invalidCharacterName(Player *player) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_GM);
	packet.add<int8_t>(0x06);
	packet.add<int8_t>(1);

	player->getSession()->send(packet);
}

auto GmPacket::hiredMerchantPlace(Player *player, int8_t channel) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_GM);
	packet.add<int8_t>(0x13);
	packet.add<int8_t>(0x01); // Mode, 00 = map, 01 = channel
	packet.add<int8_t>(channel); // 0xFE / -2 for 'Not found'

	player->getSession()->send(packet);
}

auto GmPacket::hiredMerchantPlace(Player *player, int32_t mapId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_GM);
	packet.add<int8_t>(0x13);
	packet.add<int8_t>(0x00); // Mode, 00 = map, 01 = channel
	packet.add<int32_t>(mapId);

	player->getSession()->send(packet);
}

auto GmPacket::setGetVarResult(Player *player, const string_t &name, const string_t &variable, const string_t &value) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_GM);
	packet.add<int8_t>(0x09);
	packet.addString(name);
	packet.addString(variable);
	packet.addString(value);

	player->getSession()->send(packet);
}