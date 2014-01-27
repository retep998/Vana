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
#include "SummonsPacket.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "Maps.hpp"
#include "PacketCreator.hpp"
#include "Player.hpp"
#include "SmsgHeader.hpp"
#include "Summon.hpp"

auto SummonsPacket::showSummon(Player *player, Summon *summon, bool animated, Player *toPlayer) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_SUMMON_SPAWN);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(summon->getId());
	packet.add<int32_t>(summon->getSummonId());
	packet.add<int8_t>(summon->getLevel());
	packet.addClass<Pos>(summon->getPos());
	packet.add<int8_t>(4); // ?
	packet.add<int8_t>(0x53); // ?
	packet.add<int8_t>(1); // ?
	packet.add<int8_t>(summon->getType()); // Movement type
	packet.add<int8_t>(!GameLogicUtilities::isPuppet(summon->getSummonId())); // Attack or not
	packet.add<bool>(!animated);
	if (toPlayer != nullptr) {
		toPlayer->getSession()->send(packet);
	}
	else {
		player->getMap()->sendPacket(packet);
	}
}

auto SummonsPacket::moveSummon(Player *player, Summon *summon, const Pos &startPos, unsigned char *buf, int32_t bufLen) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_SUMMON_MOVEMENT);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(summon->getId());
	packet.addClass<Pos>(startPos);
	packet.addBuffer(buf, bufLen);
	player->getMap()->sendPacket(packet, player);
}

auto SummonsPacket::removeSummon(Player *player, Summon *summon, int8_t message) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_SUMMON_DESPAWN);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(summon->getId());
	packet.add<int8_t>(message);
	player->getMap()->sendPacket(packet);
}

auto SummonsPacket::damageSummon(Player *player, int32_t summonId, int8_t unk, int32_t damage, int32_t mobId) -> void {
	PacketCreator packet;
	packet.add<header_t>(SMSG_SUMMON_DAMAGE);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(summonId);
	packet.add<int8_t>(unk);
	packet.add<int32_t>(damage);
	packet.add<int32_t>(mobId);
	packet.add<int8_t>(0);
	player->getMap()->sendPacket(packet, player);
}