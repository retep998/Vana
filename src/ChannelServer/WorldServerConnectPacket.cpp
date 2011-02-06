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
#include "WorldServerConnectPacket.h"
#include "InterHeader.h"
#include "SendHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "WorldServerConnection.h"

void WorldServerConnectPacket::groupChat(WorldServerConnection *player, int8_t type, int32_t playerid, const vector<int32_t> &receivers, const string &chat) {
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

void WorldServerConnectPacket::updateLevel(WorldServerConnection *player, int32_t playerid, int32_t level) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_UPDATE_LEVEL);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(level);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::updateJob(WorldServerConnection *player, int32_t playerid, int32_t job) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_UPDATE_JOB);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(job);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::updateMap(WorldServerConnection *player, int32_t playerid, int32_t map) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_UPDATE_MAP);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(map);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::partyOperation(WorldServerConnection *player, int8_t type, int32_t playerid, int32_t target) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(type);
	packet.add<int32_t>(playerid);
	if (target != 0) {
		packet.add<int32_t>(target);
	}
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::partyInvite(WorldServerConnection *player, int32_t playerid, const string &invitee) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_PARTY_OPERATION);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(playerid);
	packet.addString(invitee);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::playerChangeChannel(WorldServerConnection *player, Player *info, uint16_t channel) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_PLAYER_CHANGE_CHANNEL);
	packet.add<int32_t>(info->getId());
	packet.add<int16_t>(channel);

	packet.add<int64_t>(info->getConnectionTime());

	info->getActiveBuffs()->getBuffTransferPacket(packet);
	info->getSummons()->getSummonTransferPacket(packet);

	player->getSession()->send(packet);
}

void WorldServerConnectPacket::registerPlayer(WorldServerConnection *player, uint32_t ip, int32_t playerid, const string &name, int32_t map, int32_t job, int32_t level) {
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

void WorldServerConnectPacket::removePlayer(WorldServerConnection *player, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_REMOVE_PLAYER);
	packet.add<int32_t>(playerid);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::findPlayer(WorldServerConnection *player, int32_t playerid, const string &findee_name) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_FIND);
	packet.add<int32_t>(playerid);
	packet.addString(findee_name);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::whisperPlayer(WorldServerConnection *player, int32_t playerid, const string &whisperee, const string &message) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_WHISPER);
	packet.add<int32_t>(playerid);
	packet.addString(whisperee);
	packet.addString(message);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::scrollingHeader(WorldServerConnection *player, const string &message) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_SCROLLING_HEADER);
	packet.addString(message);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::rankingCalculation(WorldServerConnection *player) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TO_LOGIN);
	packet.add<int16_t>(INTER_CALCULATE_RANKING);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::playerBuffsTransferred(WorldServerConnection *player, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TRANSFER_PLAYER_PACKET);
	packet.add<int32_t>(playerid);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::toChannels(WorldServerConnection *player, PacketCreator &packet) {
	PacketCreator pack;
	pack.add<int16_t>(INTER_TO_CHANNELS);
	pack.addBuffer(packet);
	player->getSession()->send(pack);
}

void WorldServerConnectPacket::toWorlds(WorldServerConnection *player, PacketCreator &packet) {
	PacketCreator pack;
	pack.add<int16_t>(INTER_TO_WORLDS);
	pack.addBuffer(packet);
	player->getSession()->send(pack);
}

void WorldServerConnectPacket::worldMessage(WorldServerConnection *player, const string &message, int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TO_CHANNELS);
	packet.add<int16_t>(INTER_TO_PLAYERS);
	packet.add<int16_t>(SMSG_MESSAGE); 
	packet.add<int8_t>(type);
	packet.addString(message);
	if (type == 6)
		packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::globalMessage(WorldServerConnection *player, const string &message, int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_TO_LOGIN);
	packet.add<int16_t>(INTER_TO_WORLDS);
	packet.add<int16_t>(INTER_TO_CHANNELS);
	packet.add<int16_t>(INTER_TO_PLAYERS);
	packet.add<int16_t>(SMSG_MESSAGE); 
	packet.add<int8_t>(type);
	packet.addString(message);
	if (type == 6)
		packet.add<int32_t>(0);
	
	player->getSession()->send(packet);
}

void WorldServerConnectPacket::reloadMcdb(WorldServerConnection *player, const string &type) {
	PacketCreator packet;
	packet.add<int16_t>(INTER_REFRESH_DATA);
	packet.addString(type);
	toChannels(player, packet);
}