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
#include "WorldServerConnectPlayer.h"
#include "PacketCreator.h"
#include "InterHeader.h"

void WorldServerConnectPlayerPacket::groupChat(WorldServerConnectPlayer *player, char type, int playerid, const vector<int> &receivers, const string &chat) {
	Packet packet;
	packet.addHeader(INTER_GROUP_CHAT);
	packet.addInt(playerid);
	packet.addByte(type);
	packet.addString(chat);
	packet.addByte(receivers.size());
	for (size_t i = 0; i < receivers.size(); i++) {
		packet.addInt(receivers[i]);
	}

	packet.send(player);
}

void WorldServerConnectPlayerPacket::updateLevel(WorldServerConnectPlayer *player, int playerid, int level) {
	Packet packet;
	packet.addHeader(INTER_UPDATE_LEVEL);
	packet.addInt(playerid);
	packet.addInt(level);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::updateJob(WorldServerConnectPlayer *player, int playerid, int job) {
	Packet packet;
	packet.addHeader(INTER_UPDATE_JOB);
	packet.addInt(playerid);
	packet.addInt(job);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::updateMap(WorldServerConnectPlayer *player, int playerid, int map) {
	Packet packet;
	packet.addHeader(INTER_UPDATE_MAP);
	packet.addInt(playerid);
	packet.addInt(map);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::partyOperation(WorldServerConnectPlayer *player, char type, int playerid, int target) {
	Packet packet;
	packet.addHeader(INTER_PARTY_OPERATION);
	packet.addByte(type);
	packet.addInt(playerid);
	if (target != 0) {
		packet.addInt(target);
	}
	packet.send(player);
}

void WorldServerConnectPlayerPacket::partyInvite(WorldServerConnectPlayer *player, int playerid, const string &invitee) {
	Packet packet;
	packet.addHeader(INTER_PARTY_OPERATION);
	packet.addByte(0x04);
	packet.addInt(playerid);
	packet.addString(invitee);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::playerChangeChannel(WorldServerConnectPlayer *player, int playerid, int channel) {
	Packet packet;
	packet.addHeader(INTER_PLAYER_CHANGE_CHANNEL);
	packet.addInt(playerid);
	packet.addInt(channel);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::registerPlayer(WorldServerConnectPlayer *player, int playerid, const string &name, int map, int job, int level) {
	Packet packet;
	packet.addHeader(INTER_REGISTER_PLAYER);
	packet.addInt(playerid);
	packet.addString(name);
	packet.addInt(map);
	packet.addInt(job);
	packet.addInt(level);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::removePlayer(WorldServerConnectPlayer *player, int playerid) {
	Packet packet;
	packet.addHeader(INTER_REMOVE_PLAYER);
	packet.addInt(playerid);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::findPlayer(WorldServerConnectPlayer *player, int playerid, const string &findee_name) {
	Packet packet;
	packet.addHeader(INTER_FIND);
	packet.addInt(playerid);
	packet.addString(findee_name);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::whisperPlayer(WorldServerConnectPlayer *player, int playerid, const string &whisperee, const string &message) {
	Packet packet;
	packet.addHeader(INTER_WHISPER);
	packet.addInt(playerid);
	packet.addString(whisperee);
	packet.addString(message);
	packet.send(player);
}

void WorldServerConnectPlayerPacket::scrollingHeader(WorldServerConnectPlayer *player, const string &message) {
	Packet packet;
	packet.addHeader(INTER_SCROLLING_HEADER);
	packet.addString(message);
	packet.send(player);
}
