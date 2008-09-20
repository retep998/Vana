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
#include "WorldServerConnectPlayerPacket.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "WorldServerConnectPlayer.h"
#include <string>
#include <vector>

using std::vector;
using std::string;

void WorldServerConnectPlayerPacket::groupChat(WorldServerConnectPlayer *player, int8_t type, int32_t playerid, const vector<int32_t> &receivers, const string &chat) {
	PacketCreator packet;
	packet.addShort(INTER_GROUP_CHAT);
	packet.addInt(playerid);
	packet.addByte(type);
	packet.addString(chat);
	packet.addByte(receivers.size());
	for (size_t i = 0; i < receivers.size(); i++) {
		packet.addInt(receivers[i]);
	}

	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::updateLevel(WorldServerConnectPlayer *player, int32_t playerid, int32_t level) {
	PacketCreator packet;
	packet.addShort(INTER_UPDATE_LEVEL);
	packet.addInt(playerid);
	packet.addInt(level);
	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::updateJob(WorldServerConnectPlayer *player, int32_t playerid, int32_t job) {
	PacketCreator packet;
	packet.addShort(INTER_UPDATE_JOB);
	packet.addInt(playerid);
	packet.addInt(job);
	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::updateMap(WorldServerConnectPlayer *player, int32_t playerid, int32_t map) {
	PacketCreator packet;
	packet.addShort(INTER_UPDATE_MAP);
	packet.addInt(playerid);
	packet.addInt(map);
	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::partyOperation(WorldServerConnectPlayer *player, int8_t type, int32_t playerid, int32_t target) {
	PacketCreator packet;
	packet.addShort(INTER_PARTY_OPERATION);
	packet.addByte(type);
	packet.addInt(playerid);
	if (target != 0) {
		packet.addInt(target);
	}
	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::partyInvite(WorldServerConnectPlayer *player, int32_t playerid, const string &invitee) {
	PacketCreator packet;
	packet.addShort(INTER_PARTY_OPERATION);
	packet.addByte(0x04);
	packet.addInt(playerid);
	packet.addString(invitee);
	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::playerChangeChannel(WorldServerConnectPlayer *player, int32_t playerid, uint16_t channel) {
	PacketCreator packet;
	packet.addShort(INTER_PLAYER_CHANGE_CHANNEL);
	packet.addInt(playerid);
	packet.addShort(channel);
	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::registerPlayer(WorldServerConnectPlayer *player, int32_t playerid, const string &name, int32_t map, int32_t job, int32_t level) {
	PacketCreator packet;
	packet.addShort(INTER_REGISTER_PLAYER);
	packet.addInt(playerid);
	packet.addString(name);
	packet.addInt(map);
	packet.addInt(job);
	packet.addInt(level);
	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::removePlayer(WorldServerConnectPlayer *player, int32_t playerid) {
	PacketCreator packet;
	packet.addShort(INTER_REMOVE_PLAYER);
	packet.addInt(playerid);
	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::findPlayer(WorldServerConnectPlayer *player, int32_t playerid, const string &findee_name) {
	PacketCreator packet;
	packet.addShort(INTER_FIND);
	packet.addInt(playerid);
	packet.addString(findee_name);
	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::whisperPlayer(WorldServerConnectPlayer *player, int32_t playerid, const string &whisperee, const string &message) {
	PacketCreator packet;
	packet.addShort(INTER_WHISPER);
	packet.addInt(playerid);
	packet.addString(whisperee);
	packet.addString(message);
	player->getSession()->send(packet);
}

void WorldServerConnectPlayerPacket::scrollingHeader(WorldServerConnectPlayer *player, const string &message) {
	PacketCreator packet;
	packet.addShort(INTER_SCROLLING_HEADER);
	packet.addString(message);
	player->getSession()->send(packet);
}
