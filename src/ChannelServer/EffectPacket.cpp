/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "EffectPacket.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"

void EffectPacket::playMusic(int32_t mapId, const string &music) {
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_EFFECT);
	packet.add<int8_t>(0x06);
	packet.addString(music);
	Maps::getMap(mapId)->sendPacket(packet);
}

void EffectPacket::playMusic(Player *player, const string &music) {
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_EFFECT);
	packet.add<int8_t>(0x06);
	packet.addString(music);
	player->getSession()->send(packet);
}

void EffectPacket::sendEvent(int32_t mapId, const string &id) {
	// Look in Map.wz/Effect.img to find valid strings
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_EFFECT);
	packet.add<int8_t>(0x03);
	packet.addString(id);
	Maps::getMap(mapId)->sendPacket(packet);
}

void EffectPacket::sendEffect(int32_t mapId, const string &effect) {
	// Look in Map.wz/Obj/Effect.img/quest/ for valid strings
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_EFFECT);
	packet.add<int8_t>(0x02);
	packet.addString(effect);
	Maps::getMap(mapId)->sendPacket(packet);
}

void EffectPacket::playPortalSoundEffect(Player *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_THEATRICS);
	packet.add<int8_t>(0x07);
	player->getSession()->send(packet);
}

void EffectPacket::sendFieldSound(int32_t mapId, const string &sound) {
	// Look in Sound.wz/Field.img to find valid strings
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_EFFECT);
	packet.add<int8_t>(0x04);
	packet.addString(sound);
	Maps::getMap(mapId)->sendPacket(packet);
}

void EffectPacket::sendFieldSound(Player *player, const string &sound) {
	PacketCreator packet;
	packet.addHeader(SMSG_MAP_EFFECT);
	packet.add<int8_t>(0x04);
	packet.addString(sound);
	player->getSession()->send(packet);
}

void EffectPacket::sendMinigameSound(int32_t mapId, const string &sound) {
	// Look in Sound.wz/MiniGame.img to find valid strings
	PacketCreator packet;
	packet.addHeader(SMSG_SOUND);
	packet.addString(sound);
	Maps::getMap(mapId)->sendPacket(packet);
}

void EffectPacket::sendMinigameSound(Player *player, const string &sound) {
	PacketCreator packet;
	packet.addHeader(SMSG_SOUND);
	packet.addString(sound);
	player->getSession()->send(packet);
}

void EffectPacket::sendMobItemBuffEffect(Player *player, int32_t itemid) {
	PacketCreator packet;
	packet.addHeader(SMSG_THEATRICS);
	packet.add<int8_t>(0x0B);
	packet.add<int32_t>(itemid);
	player->getSession()->send(packet);

	packet = PacketCreator();
	packet.addHeader(SMSG_SKILL_SHOW);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(0x0B);
	packet.add<int32_t>(itemid);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}