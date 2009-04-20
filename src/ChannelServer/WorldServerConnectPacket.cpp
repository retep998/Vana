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
#include "WorldServerConnectPacket.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "PlayerActiveBuffs.h"
#include "WorldServerConnectPlayer.h"
#include <boost/tr1/unordered_map.hpp>
#include <list>

using std::list;
using std::tr1::unordered_map;

void WorldServerConnectPacket::groupChat(WorldServerConnectPlayer *player, int8_t type, int32_t playerid, const vector<int32_t> &receivers, const string &chat) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_GROUP_CHAT);
	packet.add<int32_t>(playerid);
	packet.add<int8_t>(type);
	packet.addString(chat);
	packet.add<int8_t>(receivers.size());
	for (size_t i = 0; i < receivers.size(); i++) {
		packet.add<int32_t>(receivers[i]);
	}

	player->getSession()->send(packet);
}

void WorldServerConnectPacket::updateLevel(WorldServerConnectPlayer *player, int32_t playerid, int32_t level) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_UPDATE_LEVEL);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(level);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::updateJob(WorldServerConnectPlayer *player, int32_t playerid, int32_t job) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_UPDATE_JOB);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(job);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::updateMap(WorldServerConnectPlayer *player, int32_t playerid, int32_t map) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_UPDATE_MAP);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(map);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::partyOperation(WorldServerConnectPlayer *player, int8_t type, int32_t playerid, int32_t target) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(type);
	packet.add<int32_t>(playerid);
	if (target != 0) {
		packet.add<int32_t>(target);
	}
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::partyInvite(WorldServerConnectPlayer *player, int32_t playerid, const string &invitee) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(playerid);
	packet.addString(invitee);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::playerChangeChannel(WorldServerConnectPlayer *player, int32_t playerid, uint16_t channel, PlayerActiveBuffs *playerbuffs) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_PLAYER_CHANGE_CHANNEL);
	packet.add<int32_t>(playerid);
	packet.add<int16_t>(channel);

	// Buff information nuisance ahoy
	packet.add<int8_t>(playerbuffs->getCombo());
	packet.add<int16_t>(playerbuffs->getEnergyChargeLevel());
	packet.add<int32_t>(playerbuffs->getCharge());
	packet.add<int32_t>(playerbuffs->getBooster());
	packet.add<int32_t>(playerbuffs->getBattleshipHp());
	packet.add<int32_t>(playerbuffs->getDebuffMask());
	MapEntryBuffs enterbuffs = playerbuffs->getMapEntryBuffs();
	packet.add<int32_t>(enterbuffs.mountid);
	packet.add<int32_t>(enterbuffs.mountskill);
	for (int8_t i = 0; i < 8; i++) {
		packet.add<uint8_t>(enterbuffs.types[i]);
		packet.add<uint8_t>((uint8_t)(enterbuffs.values[i].size()));
		for (unordered_map<uint8_t, MapEntryVals>::iterator iter = enterbuffs.values[i].begin(); iter != enterbuffs.values[i].end(); iter++) {
			packet.add<uint8_t>(iter->first);
			packet.add<int8_t>(iter->second.debuff ? 1 : 0);
			if (iter->second.debuff) {
				packet.add<int16_t>(iter->second.skill);
				packet.add<int16_t>(iter->second.val);
			}
			else {
				packet.add<int8_t>(iter->second.use ? 1 : 0);
				packet.add<int16_t>(iter->second.val);
			}
		}
	}
	list<int32_t> currentbuffs = playerbuffs->getBuffs();
	packet.add<uint8_t>((uint8_t)(currentbuffs.size()));
	for (list<int32_t>::iterator iter = currentbuffs.begin(); iter != currentbuffs.end(); iter++) {
		int32_t buffid = *iter;
		packet.add<int32_t>(buffid);
		packet.add<int32_t>(playerbuffs->buffTimeLeft(buffid) / 1000);
		packet.add<uint8_t>(playerbuffs->getActiveSkillLevel(buffid));
	}
	ActiveBuffsByType bufftypes = playerbuffs->getBuffTypes();
	unordered_map<uint8_t, int32_t> currentbyte;
	for (int8_t i = 0; i < 8; i++) {
		currentbyte = bufftypes[i];
		packet.add<uint8_t>((uint8_t)(currentbyte.size()));
		for (unordered_map<uint8_t, int32_t>::iterator iter = currentbyte.begin(); iter != currentbyte.end(); iter++) {
			packet.add<uint8_t>(iter->first);
			packet.add<int32_t>(iter->second);
		}
	}
	// Buff information end

	player->getSession()->send(packet);
}

void WorldServerConnectPacket::registerPlayer(WorldServerConnectPlayer *player, uint32_t ip, int32_t playerid, const string &name, int32_t map, int32_t job, int32_t level) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_REGISTER_PLAYER);
	packet.add<uint32_t>(ip);
	packet.add<int32_t>(playerid);
	packet.addString(name);
	packet.add<int32_t>(map);
	packet.add<int32_t>(job);
	packet.add<int32_t>(level);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::removePlayer(WorldServerConnectPlayer *player, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_REMOVE_PLAYER);
	packet.add<int32_t>(playerid);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::findPlayer(WorldServerConnectPlayer *player, int32_t playerid, const string &findee_name) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FIND);
	packet.add<int32_t>(playerid);
	packet.addString(findee_name);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::whisperPlayer(WorldServerConnectPlayer *player, int32_t playerid, const string &whisperee, const string &message) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_WHISPER);
	packet.add<int32_t>(playerid);
	packet.addString(whisperee);
	packet.addString(message);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::scrollingHeader(WorldServerConnectPlayer *player, const string &message) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_SCROLLING_HEADER);
	packet.addString(message);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::rankingCalculation(WorldServerConnectPlayer *player) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TO_LOGIN);
	packet.add<int16_t>(INTER_CALCULATE_RANKING);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::playerBuffsTransferred(WorldServerConnectPlayer *player, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TRANSFER_BUFFS);
	packet.add<int32_t>(playerid);
	player->getSession()->send(packet);
}