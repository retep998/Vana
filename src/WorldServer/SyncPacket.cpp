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
#include "SyncPacket.h"
#include "Alliance.h"
#include "Channel.h"
#include "Channels.h"
#include "Guild.h"
#include "GuildPacket.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "MapConstants.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"
#include "TimeUtilities.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"

void SyncPacket::AlliancePacket::changeAlliance(Alliance *alliance, int8_t type) {
	unordered_map<int32_t, Guild *> guilds = alliance->getGuilds();

	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeAlliance);
	packet.add<uint8_t>(type);
	packet.add<int32_t>(type == 1 ? alliance->getId() : 0);
	packet.add<uint8_t>(alliance->getSize());
	for (unordered_map<int32_t, Guild *>::iterator iter = guilds.begin(); iter != guilds.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
		packet.add<int32_t>(iter->second->m_players.size());
		for (unordered_map<int32_t, Player *>::iterator iter2 = iter->second->m_players.begin(); iter2 != iter->second->m_players.end(); iter2++) {
			packet.add<int32_t>(iter2->second->getId());
			packet.add<uint8_t>(type == 1 ? iter2->second->getAllianceRank() : 0);
		}
	}

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::AlliancePacket::changeGuild(Alliance *alliance, Guild *guild) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeGuild);
	packet.add<int32_t>(alliance == nullptr ? 0 : alliance->getId());
	packet.add<int32_t>(guild->getId());
	packet.add<int32_t>(guild->m_players.size());

	unordered_map<int32_t, Player *>::iterator iter;
	for (iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
		packet.add<uint8_t>(alliance == nullptr ? 5 : iter->second->getAllianceRank());
	}

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::AlliancePacket::changeLeader(Alliance *alliance, Player *oldLeader) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeLeader);

	packet.add<int32_t>(alliance->getId());
	packet.add<int32_t>(oldLeader->getId());
	packet.add<int32_t>(alliance->getLeaderId());
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::AlliancePacket::changePlayerRank(Alliance *alliance, Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeRank);

	packet.add<int32_t>(alliance->getId());
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getAllianceRank());
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::AlliancePacket::changeCapacity(Alliance *alliance) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChangeCapacity);

	packet.add<int32_t>(alliance->getId());
	packet.add<int32_t>(alliance->getCapacity());
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::GuildPacket::updatePlayers(Guild *guild, bool remove) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangePlayerGuildName);

	if (remove) {
		packet.addString("");
		packet.add<int32_t>(0);
	}
	else {
		packet.addString(guild->getName());
		packet.add<int32_t>(guild->getId());
	}
	packet.add<int32_t>(guild->m_players.size());

	for (unordered_map<int32_t, Player*>::iterator iter = guild->m_players.begin(); iter !=  guild->m_players.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
		packet.add<uint8_t>(remove ? 5 : iter->second->getGuildRank());
	}

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::GuildPacket::updatePlayer(Guild *guild, Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangePlayerGuildName);

	if (guild == nullptr) { // Expel/leave
		packet.add<int32_t>(-1);
		packet.add<int32_t>(1);
		packet.add<int32_t>(player->getId());
		packet.add<uint8_t>(5);
	}
	else {
		packet.add<int32_t>(guild->getId());
		packet.add<int32_t>(1);
		packet.add<int32_t>(player->getId());
		packet.add<uint8_t>(guild->getLowestRank());
	}

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::GuildPacket::updatePlayerRank(Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangePlayer);

	packet.add<int32_t>(player->getId());
	packet.add<uint8_t>(player->getGuildRank());
	packet.add<uint8_t>(player->getAllianceRank());

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::GuildPacket::updatePlayerMesos(Player *player, int32_t amount) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangePlayerMoney);

	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(amount);

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::GuildPacket::updateEmblem(Guild *guild) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangeEmblem);

	GuildLogo logo = guild->getLogo();
	packet.add<int32_t>(guild->getId());
	packet.add<int16_t>(logo.logo);
	packet.add<uint8_t>(logo.color);
	packet.add<int16_t>(logo.background);
	packet.add<uint8_t>(logo.backgroundColor);
	packet.add<int32_t>(guild->m_players.size());

	for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++)
		packet.add<int32_t>(iter->second->getId());

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::GuildPacket::updateCapacity(Guild *guild) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChangeCapacity);

	packet.add<int32_t>(guild->getId());
	packet.add<int32_t>(guild->getCapacity());

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::GuildPacket::loadGuild(int32_t guildid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::Load);

	packet.add<int32_t>(guildid);

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::GuildPacket::unloadGuild(int32_t guildid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::Unload);

	packet.add<int32_t>(guildid);

	Channels::Instance()->sendToAll(packet);
}


void SyncPacket::GuildPacket::removePlayer(Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::RemovePlayer);

	packet.add<int32_t>(player->getId());

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::GuildPacket::addPlayer(Player *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::AddPlayer);

	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(player->getGuild()->getId());
	packet.add<uint8_t>(player->getGuildRank());
	packet.add<int32_t>(player->getAlliance() == nullptr ? 0 : player->getAlliance()->getId());
	packet.add<uint8_t>(player->getAllianceRank());

	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::BbsPacket::sendThreadList(uint16_t channel, Guild *guild, int32_t playerid, int16_t page) {
	GuildBbs *bbs = guild->getBbs();
	int32_t threads = bbs->m_threads.size();
	int16_t counter1 = 0;
	int16_t counter2 = 0;
	int32_t startNumber = page * 10; // 10 threads per page
	int32_t threadsOnPage = std::min<int32_t>(10, (threads - startNumber));

	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);

	packet.addHeader(SMSG_BBS);
	packet.add<int8_t>(0x06);

	BbsThread *notice = bbs->getNotice();
	packet.addBool(notice != nullptr);
	if (notice != nullptr) {
		packet.add<int32_t>(notice->getListId());
		packet.add<int32_t>(notice->getUserId());
		packet.addString(notice->getTitle());
		packet.add<int64_t>(TimeUtilities::timeToTick(notice->getTime()));
		packet.add<int32_t>(notice->getIcon());
		packet.add<int32_t>(notice->getSize());
	}

	packet.add<int32_t>(threads);
	packet.add<int32_t>(threadsOnPage);

	for (map<int32_t, BbsThread *>::reverse_iterator iter = bbs->m_threads.rbegin(); iter != bbs->m_threads.rend(); iter++) {
		counter1++;
		if (counter1 > startNumber && counter2 <= threadsOnPage) {
			counter2++;
			packet.add<int32_t>(iter->second->getListId());
			packet.add<int32_t>(iter->second->getUserId());
			packet.addString(iter->second->getTitle());
			packet.add<int64_t>(TimeUtilities::timeToTick(iter->second->getTime()));
			packet.add<int32_t>(iter->second->getIcon());
			packet.add<int32_t>(iter->second->m_replies.size());
		}
	}

	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::BbsPacket::sendThreadData(uint16_t channel, BbsThread *thread, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);

	packet.addHeader(SMSG_BBS);
	packet.add<int8_t>(0x07);

	packet.add<int32_t>(thread->getListId());
	packet.add<int32_t>(thread->getUserId());
	packet.add<int64_t>(TimeUtilities::timeToTick(thread->getTime()));
	packet.addString(thread->getTitle());
	packet.addString(thread->getContent());
	packet.add<int32_t>(thread->getIcon());

	packet.add<int32_t>(thread->m_replies.size());

	for (map<int32_t, BbsReply *>::iterator iter = thread->m_replies.begin(); iter != thread->m_replies.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
		packet.add<int32_t>(iter->second->getUserId());
		packet.add<int64_t>(TimeUtilities::timeToTick(iter->second->getTime()));
		packet.addString(iter->second->getContent());
	}

	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::giveLeader(uint16_t channel, int32_t playerid, int32_t target, bool is) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	packet.add<int8_t>(0x1b);
	packet.add<int32_t>(target);
	packet.addBool(is);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::invitePlayer(uint16_t channel, int32_t playerid, const string &inviter) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	packet.add<int8_t>(0x04);
	packet.add<int32_t>(PlayerDataProvider::Instance()->getPlayer(inviter)->getParty()->getId());
	packet.addString(inviter);
	packet.add<int8_t>(0);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::createParty(uint16_t channel, int32_t playerid) {
	Player *partyplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	packet.add<int8_t>(0x08);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(Maps::NoMap);
	packet.add<int32_t>(0);
	Channels::Instance()->sendToChannel(channel, packet);

	packet = PacketCreator();
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(PartyActions::Join);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::disbandParty(uint16_t channel, int32_t playerid) {
	Player *partyplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	packet.add<int8_t>(0x0C);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	packet.add<int32_t>(partyplayer->getParty()->getLeader());
	packet.add<int8_t>(0);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);

	packet = PacketCreator();
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(PartyActions::Leave);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(partyplayer->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::updateParty(uint16_t channel, int8_t type, int32_t playerid, int32_t target) {
	Player *partyplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	switch (type) {
		case PartyActions::Join: 
			packet.add<int8_t>(0x0F);
			packet.add<int32_t>(partyplayer->getParty()->getId());
			packet.addString(PlayerDataProvider::Instance()->getPlayer(target)->getName());
			break;
		case PartyActions::Leave:
		case PartyActions::Expel:
			packet.add<int8_t>(0x0C);
			packet.add<int32_t>(partyplayer->getParty()->getId());
			packet.add<int32_t>(target);
			packet.add<int8_t>(0x01);
			packet.add<int8_t>(type == PartyActions::Leave ? 0x00 : 0x01);
			packet.addString(PlayerDataProvider::Instance()->getPlayer(target, true)->getName());
			break;
		case PartyActions::SilentUpdate:
		case PartyActions::LogInOrOut:
			packet.add<int8_t>(0x07);
			packet.add<int32_t>(partyplayer->getParty()->getId());
	}
	addParty(packet, partyplayer->getParty(), PlayerDataProvider::Instance()->getPlayer(playerid)->getChannel());
	Channels::Instance()->sendToChannel(channel, packet);

	if (type == PartyActions::SilentUpdate)
		return;

	target = target == 0 ? playerid : target;

	packet = PacketCreator();
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(type);
	packet.add<int32_t>(target);
	packet.add<int32_t>(PlayerDataProvider::Instance()->getPlayer(target, true)->getParty()->getId());
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::partyError(uint16_t channel, int32_t playerid, int8_t error) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_FORWARD_TO);
	packet.add<int32_t>(playerid);
	packet.addHeader(SMSG_PARTY);
	packet.add<int8_t>(error);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PartyPacket::addParty(PacketCreator &packet, Party *party, int32_t tochan) {
	size_t offset = 6 - party->members.size();
	size_t i = 0;
	map<int32_t, Player *>::iterator iter;

	// Add party member's ids to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->getId());
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member's names to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.addString(iter->second->getName(), 13);
	}
	for (i = 0; i < offset; i++) {
		packet.addString("", 13);
	}

	// Add party member's jobs to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->getJob());
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member's levels to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(iter->second->getLevel());
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member's channels to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			packet.add<int32_t>(iter->second->getChannel());
		}
		else {
			packet.add<int32_t>(-2);
		}
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(-2);
	}

	packet.add<int32_t>(party->getLeader());

	// Add party member's maps to packet
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->getChannel() == tochan) {
			packet.add<int32_t>(iter->second->getMap());
		}
		else {
			packet.add<int32_t>(-2);
		}
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(-2);
	}

	// Add some portal shit
	for (iter = party->members.begin(); iter != party->members.end(); iter++) {
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(-1);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(-1);
	}
}

void SyncPacket::PlayerPacket::newConnectable(uint16_t channel, int32_t playerid, uint32_t playerip) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::NewConnectable);
	packet.add<int32_t>(playerid);
	packet.add<uint32_t>(playerip);

	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PlayerPacket::newConnectableCashServer(int32_t playerid, uint32_t playerip) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::NewConnectable);
	packet.add<int32_t>(playerid);
	packet.add<uint32_t>(playerip);

	WorldServer::Instance()->getCashConnection()->getSession()->send(packet);
}

void SyncPacket::PlayerPacket::sendCannotChangeServerToPlayer(uint16_t channel, int32_t playerid, int8_t reason) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::CannotChangeServer);
	packet.add<int32_t>(playerid);
	packet.add<int8_t>(reason);

	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PlayerPacket::sendPlayerDisconnectServer(WorldServerAcceptConnection *player, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::Disconnect);
	packet.add<int32_t>(playerid);

	player->getSession()->send(packet);
}

void SyncPacket::PlayerPacket::sendPacketToChannelForHolding(uint16_t channel, int32_t playerid, PacketReader &buffer, bool fromCashOrMts) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::PacketTransfer);
	packet.add<int32_t>(playerid);
	packet.addBool(fromCashOrMts);
	packet.addBuffer(buffer);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PlayerPacket::sendPacketToCashServerForHolding(int32_t playerid, PacketReader &buffer) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::PacketTransfer);
	packet.add<int32_t>(playerid);
	packet.addBuffer(buffer);
	WorldServer::Instance()->getCashConnection()->getSession()->send(packet);
}

void SyncPacket::PlayerPacket::sendHeldPacketRemoval(uint16_t channel, int32_t playerid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::RemovePacketTransfer);
	packet.add<int32_t>(playerid);
	Channels::Instance()->sendToChannel(channel, packet);
}

void SyncPacket::PlayerPacket::playerChangeChannel(WorldServerAcceptConnection *player, int32_t playerid, uint32_t ip, uint16_t port) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Player);
	packet.add<int8_t>(Sync::Player::ChangeChannelGo);
	packet.add<int32_t>(playerid);
	packet.add<uint32_t>(ip);
	packet.add<uint16_t>(port);
	player->getSession()->send(packet);
}

void SyncPacket::PlayerPacket::sendParties(WorldServerAcceptConnection *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::ChannelStart);

	unordered_map<int32_t, Party *> parties = PlayerDataProvider::Instance()->getParties();
	map<int32_t, Player *> players;

	packet.add<int32_t>((int32_t)(parties.size()));
	for (unordered_map<int32_t, Party *>::iterator iter = parties.begin(); iter != parties.end(); iter++) {
		packet.add<int32_t>(iter->first);
		players = iter->second->members;
		packet.add<int8_t>((int8_t)(players.size()));
		for (map<int32_t, Player *>::iterator playeriter = players.begin(); playeriter != players.end(); playeriter++) {
			packet.add<int32_t>(playeriter->first);
		}
		packet.add<int32_t>(iter->second->getLeader());
	}

	player->getSession()->send(packet);
}

void SyncPacket::PlayerPacket::sendRemovePartyPlayer(int32_t playerid, int32_t partyid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Data);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::RemoveMember);
	packet.add<int32_t>(partyid);
	packet.add<int32_t>(playerid);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::sendAddPartyPlayer(int32_t playerid, int32_t partyid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Data);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::AddMember);
	packet.add<int32_t>(partyid);
	packet.add<int32_t>(playerid);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::sendSwitchPartyLeader(int32_t playerid, int32_t partyid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Data);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::SwitchLeader);
	packet.add<int32_t>(partyid);
	packet.add<int32_t>(playerid);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::sendCreateParty(int32_t playerid, int32_t partyid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Data);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::Create);
	packet.add<int32_t>(partyid);
	packet.add<int32_t>(playerid);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::sendDisbandParty(int32_t partyid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Data);
	packet.add<int8_t>(Sync::SyncTypes::Party);
	packet.add<int8_t>(Sync::Party::Disband);
	packet.add<int32_t>(partyid);
	Channels::Instance()->sendToAll(packet);
}

void SyncPacket::PlayerPacket::sendGuilds(WorldServerAcceptConnection *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Guild);
	packet.add<int8_t>(Sync::Guild::ChannelConnect);
	PlayerDataProvider::Instance()->getChannelConnectPacketGuild(packet);
	player->getSession()->send(packet);
}

void SyncPacket::PlayerPacket::sendAlliances(WorldServerAcceptConnection *player) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Alliance);
	packet.add<int8_t>(Sync::Alliance::ChannelConnect);
	PlayerDataProvider::Instance()->getChannelConnectPacketAlliance(packet);
	player->getSession()->send(packet);
}

void SyncPacket::BuddyPacket::sendBuddyInvite(WorldServerAcceptConnection *channel, int32_t inviteeid, int32_t inviterid, const string &name) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddies::Invite);
	packet.add<int32_t>(inviteeid);
	packet.add<int32_t>(inviterid);
	packet.addString(name);
	channel->getSession()->send(packet);
}

void SyncPacket::BuddyPacket::sendBuddyOnlineOffline(WorldServerAcceptConnection *channel, vector<int32_t> players, int32_t playerid, int32_t channelid) {
	PacketCreator packet;
	packet.add<int16_t>(IMSG_SYNC);
	packet.add<int8_t>(Sync::SyncTypes::Buddy);
	packet.add<int8_t>(Sync::Buddies::OnlineOffline);
	packet.add<int32_t>(playerid);
	packet.add<int32_t>(channelid); // I need to get FF FF FF FF, not FF FF 00 00
	packet.addVector(players);
	channel->getSession()->send(packet);
}
