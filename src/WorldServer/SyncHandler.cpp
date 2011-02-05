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
#include "SyncHandler.h"
#include "Alliance.h"
#include "AlliancePacket.h"
#include "Channel.h"
#include "Channels.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameObjects.h"
#include "Guild.h"
#include "GuildPacket.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "IpUtilities.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"
#include "SyncPacket.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"
#include "WorldServerAcceptPacket.h"

void SyncHandler::handle(WorldServerAcceptConnection *connection, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::SyncTypes::GuildBbs: handleBbsPacket(packet); break;
		case Sync::SyncTypes::Player: handlePlayerPacket(connection, packet); break;
		case Sync::SyncTypes::Guild: handleGuildPacket(packet); break;
		case Sync::SyncTypes::Party: partyOperation(packet); break;
		case Sync::SyncTypes::Alliance: handleAlliancePacket(packet); break;
		case Sync::SyncTypes::Buddy: handleBuddyPacket(packet); break;
	}
}

void SyncHandler::handleAlliancePacket(PacketReader &packet) {
	int8_t option = packet.get<int8_t>();
	int32_t allianceid = packet.get<int32_t>();
	switch (option) {
		case Sync::Alliance::InviteDenied: sendDenyPacket(packet); break;
		case Sync::Alliance::GetInfo: sendAllianceInfo(packet.get<int32_t>(), allianceid); break;
		case Sync::Alliance::ChangeTitles: sendTitleUpdate(allianceid, packet); break;
		case Sync::Alliance::ChangeNotice: sendNoticeUpdate(allianceid, packet); break;
		case Sync::Alliance::Invite: sendInvite(allianceid, packet); break;
		case Sync::Alliance::ChangeGuild: sendChangeGuild(allianceid, packet); break;
		case Sync::Alliance::ChangeLeader: sendChangeLeader(allianceid, packet); break;
		case Sync::Alliance::ChangeRank: sendPlayerChangeRank(allianceid, packet); break;
		case Sync::Alliance::Create: handleAllianceCreation(packet); break;
		case Sync::Alliance::ChangeCapacity: sendIncreaseCapacity(allianceid, packet.get<int32_t>()); break;
		case Sync::Alliance::Disband: sendAllianceDisband(allianceid, packet.get<int32_t>()); break;
	}
}

void SyncHandler::handleAllianceCreation(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	string alliancename = packet.getString();
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == nullptr|| player->getParty() == nullptr || player->getGuild() == nullptr || player->getAlliance() != nullptr || player->getGuildRank() != 1)
		return;

	Party *party = player->getParty();
	if (party->members.size() != 2) {
		return;
	}

	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->getAlliance() != nullptr || iter->second->getGuild() == nullptr) {
			return;
		}
	}

	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO alliances (id, leader, worldid, name) VALUES (NULL, "
		<< party->getLeader() << ", "
		<< static_cast<int16_t>(WorldServer::Instance()->getWorldId()) << ", "
		<< mysqlpp::quote << alliancename << ");";

	if (!query.exec()) {
		std::stringstream x;
		x << player->getName() << " has created " << alliancename << " but it failed! Error: " << query.error();
		WorldServer::Instance()->log(LogTypes::Error, x.str());
		return;
	}

	int32_t allianceid = static_cast<int32_t>(query.insert_id());

	loadAlliance(allianceid);
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);

	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		Guild *playerGuild = iter->second->getGuild();
		if (playerGuild != nullptr) {
			for (unordered_map<int32_t, Player *>::iterator iter2 = playerGuild->m_players.begin(); iter2 != playerGuild->m_players.end(); iter2++) {
				if (iter2->second->getAlliance() != nullptr || iter2->second->getGuild() == nullptr)
					continue;

				iter2->second->setAlliance(alliance);

				if (party->getLeader() == iter2->second->getId()) // The party/alliance leader
					iter2->second->setAllianceRank(1);
				else if (iter->second->getId() == iter2->second->getId()) // The other member in the party
					iter2->second->setAllianceRank(2);
				else // The other members
					iter2->second->setAllianceRank(3);
				query << "UPDATE characters SET allianceid = " << allianceid << ", "
					<< "alliancerank = " <<	static_cast<int16_t>(iter2->second->getAllianceRank()) << " WHERE id = " << iter2->second->getId() << " LIMIT 1";
				query.exec();
			}

			playerGuild->setAlliance(alliance);
			query << "UPDATE guilds SET allianceid = " << allianceid << " WHERE id = " << iter->second->getGuild()->getId() << " LIMIT 1";
			query.exec();
			alliance->addGuild(playerGuild);
		}
	}
	AlliancePacket::sendNewAlliance(alliance);
	SyncPacket::AlliancePacket::changeAlliance(alliance, 1);
}

void SyncHandler::loadAlliance(int32_t allianceid) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM alliances WHERE id = " << allianceid;
	mysqlpp::StoreQueryResult res = query.store();

	if ((int32_t) res.num_rows() == 0) {
		std::stringstream x;
		x << "Can't load alliance! Alliance ID " << allianceid;
		WorldServer::Instance()->log(LogTypes::Error, x.str());
		return;
	}

	GuildRanks ranks;

	for (int32_t i = 0; i < (int32_t) res.num_rows(); i++) {
		ranks[0] = static_cast<string>(res[i]["rank1title"]);
		ranks[1] = static_cast<string>(res[i]["rank2title"]);
		ranks[2] = static_cast<string>(res[i]["rank3title"]);
		ranks[3] = static_cast<string>(res[i]["rank4title"]);
		ranks[4] = static_cast<string>(res[i]["rank5title"]);

		PlayerDataProvider::Instance()->addAlliance(res[i]["id"],
			static_cast<string>(res[i]["name"]),
			static_cast<string>(res[i]["notice"]),
			ranks,
			res[i]["capacity"],
			res[i]["leader"]);
	}
}

void SyncHandler::sendAllianceInfo(int32_t playerid, int32_t allianceid) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	if (alliance == nullptr)
		return;
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == nullptr)
		return;
	AlliancePacket::sendAllianceInfo(alliance, player);
}

void SyncHandler::sendChangeGuild(int32_t allianceid, PacketReader &packet) {
	int32_t guildid = packet.get<int32_t>();
	int32_t playerid = packet.get<int32_t>();
	uint8_t option = packet.get<uint8_t>();
	Alliance *alliance = nullptr;

	if (option != 0) {
		alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	}
	else {
		Guild *otherGuild = PlayerDataProvider::Instance()->getGuild(allianceid);

		alliance = otherGuild->getAlliance();
		allianceid = alliance->getId();
	}

	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);

	if (alliance == nullptr || (guild == nullptr && option == 0) || player == nullptr || (option != 0 && player->getAlliance() != alliance))
		return;

	if (option == 0) {
		if (guild->getInvited() && guild->getInvitedId() == alliance->getId()) {
			alliance->addGuild(guild); // add the guild before sending the packet

			guild->removeInvite();
			guild->setAlliance(alliance);

			int16_t lowestAllianceRank = alliance->getLowestRank();

			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE characters SET allianceid = " << allianceid << ", alliancerank = " << lowestAllianceRank << " WHERE guildid = " << guild->getId();
			query.exec();

			query << "UPDATE characters SET alliancerank = 2 WHERE id = " << guild->getLeader(); // Make the leader jr master
			query.exec();

			query << "UPDATE guilds SET allianceid = " << allianceid << " WHERE id = " << guild->getId(); // Update the guild in the database
			query.exec();

			for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
				if (iter->second->getGuildRank() == 1)
					iter->second->setAllianceRank(2);
				else
					iter->second->setAllianceRank(static_cast<uint8_t>(lowestAllianceRank));

				iter->second->setAlliance(alliance);
				AlliancePacket::sendAllianceInfo(alliance, iter->second);
			}
			SyncPacket::AlliancePacket::changeGuild(alliance, guild);

			AlliancePacket::sendInviteAccepted(alliance, guild);
		}
	}
	else {
		guild->setAlliance(nullptr);

		for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
			iter->second->setAllianceRank(5);
			iter->second->setAlliance(nullptr);
		}

		SyncPacket::AlliancePacket::changeGuild(nullptr, guild);

		mysqlpp::Query query = Database::getCharDB().query();
		query << "UPDATE characters SET allianceid = 0, alliancerank = 5 WHERE guildid = " << guild->getId();
		query.exec();

		query << "UPDATE guilds SET allianceid = 0 WHERE id = " << guild->getId(); // Update the guild in the database
		query.exec();

		alliance->removeGuild(guild);
		AlliancePacket::sendGuildLeft(alliance, guild, option == 1 ? true : false);
	}
}

void SyncHandler::sendNoticeUpdate(int32_t allianceid, PacketReader &packet) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	if (alliance == nullptr)
		return;
	Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
	if (player == nullptr || player->getAllianceRank() > 2 || player->getGuild() == nullptr)
		return;

	alliance->setNotice(packet.getString());
	alliance->save();

	AlliancePacket::sendUpdateNotice(alliance);
}

void SyncHandler::sendPlayerUpdate(int32_t allianceid, int32_t playerid) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	if (alliance == nullptr)
		return;
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == nullptr || player->getAlliance() == nullptr || player->getGuild() == nullptr)
		return;

	AlliancePacket::sendUpdatePlayer(alliance, player, 1);
}

void SyncHandler::sendTitleUpdate(int32_t allianceid, PacketReader &packet) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	if (alliance == nullptr)
		return;
	Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
	if (player == nullptr || player->getAllianceRank() > 1 || player->getGuild() == nullptr)
		return;

	for (uint8_t i = 1; i <= GuildsAndAlliances::RankQuantity; i++)
		alliance->setTitle(i, packet.getString());

	alliance->save();

	AlliancePacket::sendUpdateTitles(alliance);
}

void SyncHandler::sendPlayerChangeRank(int32_t allianceid, PacketReader &packet) {
	int32_t changerid = packet.get<int32_t>();
	int32_t victimid = packet.get<int32_t>();

	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	Player *changer = PlayerDataProvider::Instance()->getPlayer(changerid);
	Player *victim = PlayerDataProvider::Instance()->getPlayer(victimid, true);

	if (alliance == nullptr || changer == nullptr || victim == nullptr)
		return;

	if (changer->getAllianceRank() > 2 || changer->getGuild() == nullptr || changer->getAlliance() == nullptr || 
		changer->getAlliance() != alliance)
		return;
	if (victim->getAllianceRank() == 1 || victim->getGuild() == nullptr || victim->getAlliance() == nullptr || 
		victim->getAlliance() != alliance)
		return;

	bool DecreaseRank = packet.getBool();

	if (DecreaseRank && victim->getAllianceRank() > 2)
		victim->setAllianceRank(victim->getAllianceRank() - 1);
	else if (!DecreaseRank && victim->getAllianceRank() > 2 && victim->getAllianceRank() < 5)
		victim->setAllianceRank(victim->getAllianceRank() + 1);
	else
		return;

	AlliancePacket::sendUpdatePlayer(alliance, victim, 0);
	SyncPacket::AlliancePacket::changePlayerRank(alliance, victim);
}

void SyncHandler::sendInvite(int32_t allianceid, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	string guildname = packet.getString();

	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildname);
	Player *inviter = PlayerDataProvider::Instance()->getPlayer(playerid);

	if (alliance == nullptr || guild == nullptr || guild->getLeader() == inviter->getId())
		return;

	Player *invitee = PlayerDataProvider::Instance()->getPlayer(guild->getLeader());
	if (invitee == nullptr || inviter == nullptr || inviter->getAllianceRank() > 1 || 
		inviter->getGuild() == nullptr || inviter->getAlliance() == nullptr || invitee->getGuild() == nullptr)
		return;

	if (!guild->getInvited()) {
		guild->setInvite(allianceid);
		AlliancePacket::sendInvite(alliance, inviter, invitee);
	}
}

void SyncHandler::sendAllianceDisband(int32_t allianceid, int32_t playerid) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);

	if (alliance == nullptr || player == nullptr || player->getAlliance() == nullptr || 
		player->getGuild() == nullptr || player->getAllianceRank() != 1 || 
		player->getAlliance() != alliance || alliance->getLeaderId() != playerid)
		return;

	unordered_map<int32_t, Guild *> guilds = alliance->getGuilds();
	unordered_map<int32_t, Guild *>::iterator iter;
	unordered_map<int32_t, Player *>::iterator iter2;

	for (iter = guilds.begin(); iter != guilds.end(); iter++) {
		for (iter2 = iter->second->m_players.begin(); iter2 != iter->second->m_players.end(); iter2++) {
			iter2->second->setAlliance(nullptr);
			iter2->second->setAllianceRank(5);
		}
		iter->second->setAlliance(nullptr);
	}

	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET allianceid = 0, alliancerank = 5 WHERE allianceid = " << allianceid;
	query.exec();

	query << "UPDATE guilds SET allianceid = 0 WHERE allianceid = " << allianceid;
	query.exec();

	query << "DELETE FROM alliances WHERE id = " << allianceid;
	query.exec();

	AlliancePacket::sendDeleteAlliance(alliance);
	SyncPacket::AlliancePacket::changeAlliance(alliance, 0);

	delete alliance;
}

void SyncHandler::sendChangeLeader(int32_t allianceid, PacketReader &packet) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
	Player *victim = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());

	if (alliance == nullptr || player == nullptr || victim == nullptr || 
		player->getAlliance() != alliance || victim->getAlliance() != alliance || 
		player->getAllianceRank() != 1 || victim->getAllianceRank() != 2)
		return;

	player->setAllianceRank(2);
	victim->setAllianceRank(1);

	alliance->setLeaderId(victim->getId());
	alliance->save();

	AlliancePacket::sendUpdateLeader(alliance, player);
	SyncPacket::AlliancePacket::changeLeader(alliance, player);
}

void SyncHandler::sendIncreaseCapacity(int32_t allianceid, int32_t playerid) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);

	if (alliance == nullptr || player == nullptr || player->getAlliance() != alliance || 
		player->getAllianceRank() != 1)
		return;

	alliance->setCapacity(alliance->getCapacity() + 1);
	alliance->save();

	SyncPacket::GuildPacket::updatePlayerMesos(player, -GuildsAndAlliances::AllianceCapacityIncreasementCost);

	AlliancePacket::sendUpdateCapacity(alliance);
	SyncPacket::AlliancePacket::changeCapacity(alliance);
}

void SyncHandler::sendDenyPacket(PacketReader &packet) {
	Player *denier = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
	if (denier == nullptr || denier->getGuild() == nullptr || denier->getGuildRank() != 1)
		return;

	packet.get<int8_t>(); // Type, to be handled....
	/* Types:
		0x14: Doesn't accept alliance invites. (in the game options window)
		0x16: Denied alliance invite.
	*/
	string allianceLeaderName = packet.getString();
	string deniedGuildName = packet.getString();

	Guild *dguild = denier->getGuild();
	if (!dguild->getInvited() || dguild->getName() != deniedGuildName)
		return;

	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(dguild->getInvitedId());
	if (alliance == nullptr)
		return;

	Player *allianceLeader = PlayerDataProvider::Instance()->getPlayer(alliance->getLeaderId());
	if (allianceLeader == nullptr || allianceLeader->getName() != allianceLeaderName)
		return;

	dguild->removeInvite();
	AlliancePacket::sendInviteDenied(alliance, dguild);
}

void SyncHandler::removeGuild(Guild *guild) {
	Alliance *alliance = guild->getAlliance();
	guild->setAlliance(nullptr);

	SyncPacket::AlliancePacket::changeGuild(nullptr, guild);

	for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
		iter->second->setAlliance(nullptr);
		iter->second->setAllianceRank(5);
	}

	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET allianceid = 0, alliancerank = 5 WHERE guildid = " << guild->getId();
	query.exec();

	query << "UPDATE guilds SET allianceid = 0 WHERE id = " << guild->getId();
	query.exec();

	alliance->removeGuild(guild);
	AlliancePacket::sendGuildLeft(alliance, guild, false);
}

void SyncHandler::handleBbsPacket(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case 0x01: handleNewThread(packet); break;
		case 0x02: handleDeleteThread(packet); break;
		case 0x03: handleNewReply(packet); break;
		case 0x04: handleDeleteReply(packet); break;
		case 0x05: handleShowThreadList(packet); break;
		case 0x06: handleShowThread(packet); break;
	}
}

void SyncHandler::handleNewThread(PacketReader &packet) {
	int32_t guildid = packet.get<int32_t>();
	int32_t playerid = packet.get<int32_t>();
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (guild == nullptr || player == nullptr || player->getGuild() == nullptr)
		return;

	GuildBbs *bbs = guild->getBbs();

	if (guildid != player->getGuild()->getId()) {
		std::stringstream x;
		x << "Player tries to create a thread in a guild BBS without being IN the guild. PlayerID: " << playerid << ", GuildID: " << guildid << std::endl;
		WorldServer::Instance()->log(LogTypes::Warning, x.str());
		return;
	}

	int32_t threadid = -1;
	bool isEdit = packet.getBool();
	if (isEdit)
		threadid = packet.get<int32_t>();
	bool isNotice = packet.getBool();
	if (isNotice && !isEdit)
		threadid = 0;
	else if (!isEdit)
		threadid = bbs->getLastThreadId() + 1;

	if (isNotice && bbs->getNotice() != nullptr)
		return; // We dont want 2 notices...

	string title = packet.getString();
	string text = packet.getString();
	int16_t icon = (int16_t) packet.get<int32_t>();

	if (!isEdit) {
		mysqlpp::Query query = Database::getCharDB().query();
		query << "INSERT INTO guild_bbs_threads (user, guild, time, icon, title, content, listid) VALUES ("
			<< playerid << ", "
			<< guildid << ", "
			<< "NOW()" << ", "
			<< icon << ", "
			<< mysqlpp::quote << title << ", "
			<< mysqlpp::quote << text << ", "
			<< threadid << ");";
		query.exec();

		bbs->load(); // Load all the threads again...

		handleShowThread(playerid, threadid);
		if (!isNotice)
			bbs->setLastThreadId(threadid);
	}
	else {
		BbsThread *thread = bbs->getThread(threadid);
		if (thread == nullptr)
			return;

		thread->setIcon(icon);
		thread->setTitle(title);
		thread->setContent(text);

		bbs->save();
		bbs->load();

		handleShowThread(playerid, threadid);
	}
}

void SyncHandler::handleShowThread(int32_t playerid, int32_t threadid) {
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = player->getGuild();
	if (player == nullptr || guild == nullptr)
		return;

	BbsThread *thread = guild->getBbs()->getThread(threadid);
	if (thread == nullptr)
		return;

	SyncPacket::BbsPacket::sendThreadData(player->getChannel(), thread, playerid);
}

void SyncHandler::handleShowThread(PacketReader &pack) {
	int32_t playerid = pack.get<int32_t>();
	int32_t guildid = pack.get<int32_t>();
	int32_t threadid = pack.get<int32_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (player == nullptr || player->getGuild() == nullptr || guild == nullptr)
		return;

	BbsThread *thread = guild->getBbs()->getThread(threadid);
	if (thread == nullptr)
		return;

	SyncPacket::BbsPacket::sendThreadData(player->getChannel(), thread, playerid);
}

void SyncHandler::handleDeleteThread(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int32_t guildid = packet.get<int32_t>();
	int32_t threadid = packet.get<int32_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = player->getGuild();
	if (player == nullptr || guild == nullptr || player->getGuild()->getId() != guildid)
		return;

	GuildBbs *bbs = guild->getBbs();

	BbsThread *thread = bbs->getThread(threadid);
	if (thread == nullptr || (thread->getUserId() != playerid && player->getGuildRank() < 2))
		return;

	bbs->removeThread(threadid);
	bbs->save();
}

void SyncHandler::handleShowThreadList(PacketReader &pack) {
	int32_t playerid = pack.get<int32_t>();
	int32_t guildid = pack.get<int32_t>();
	int16_t page = pack.get<int16_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (player == nullptr || player->getGuild() == nullptr || guild == nullptr)
		return;

	SyncPacket::BbsPacket::sendThreadList(player->getChannel(), guild, playerid, page);
}

void SyncHandler::handleNewReply(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int32_t guildid = packet.get<int32_t>();
	int32_t threadid = packet.get<int32_t>();
	string replytext = packet.getString();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (player == nullptr || player->getGuild() == nullptr || guild == nullptr)
		return;

	BbsThread *thread = guild->getBbs()->getThread(threadid);
	if (thread == nullptr)
		return;

	thread->saveReplies();
	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO guild_bbs_replies (user, thread, content, time) VALUES (" << playerid
		<< ", " << thread->getId()
		<< ", " << mysqlpp::quote << replytext
		<< ", " << "NOW()"
		<< ")";
	query.exec();

	thread->loadReplies();

	handleShowThread(playerid, thread->getListId());
}

void SyncHandler::handleDeleteReply(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int32_t guildid = packet.get<int32_t>();
	int32_t threadid = packet.get<int32_t>();
	int32_t replyid = packet.get<int32_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (player == nullptr || player->getGuild() == nullptr || guild == nullptr)
		return;

	BbsThread *thread = guild->getBbs()->getThread(threadid);
	if (thread == nullptr)
		return;

	BbsReply *reply = thread->getReply(replyid);
	if (reply == nullptr || (reply->getUserId() != playerid && player->getGuildRank() < 2))
		return;

	thread->removeReply(replyid);
	thread->saveReplies();
	thread->loadReplies();

	handleShowThread(playerid, thread->getListId());
}

void SyncHandler::handleGuildPacket(PacketReader &packet) {
	int32_t option = packet.get<int8_t>();
	int32_t guildid = packet.get<int32_t>();
	switch (option) {
		case Sync::Guild::Invite: sendGuildInvite(guildid, packet); break;
		case Sync::Guild::ExpelOrLeave: {
			Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>(), true);
			if (player == nullptr)
				return;
			string name = packet.getString();
			bool expelled = packet.getBool();
			sendDeletePlayer(guildid, player->getId(), name, expelled);
			break;
		}
		case Sync::Guild::AcceptInvite: {
			int32_t playerid = packet.get<int32_t>();
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
			if (player == nullptr)
				return;
			int32_t inviteGuildId = player->getGuildInviteId();
			if (!player->isInvitedToGuild() || difftime(player->getGuildInviteTime(), time(0)) >= GuildsAndAlliances::InvitationAliveTime) {
				if (difftime(player->getGuildInviteTime(), time(0)) >= GuildsAndAlliances::InvitationAliveTime) {
					player->setGuildInviteTime(0);
					player->setGuildInviteId(0);
				}
				return;
			}
			Guild *guild = PlayerDataProvider::Instance()->getGuild(inviteGuildId);
			if (guild == nullptr)
				return;
			player->setGuildInviteId(0);
			player->setGuildInviteTime(0);
			player->setGuild(guild);
			player->setGuildRank(guild->getLowestRank());
			player->setAlliance(guild->getAlliance() != nullptr ? guild->getAlliance() : nullptr);
			player->setAllianceRank(guild->getAlliance() != nullptr ? guild->getAlliance()->getLowestRank() : 0);
			sendNewPlayer(inviteGuildId, playerid, false);
			break;
		}
		case Sync::Guild::ChangeTitles: sendUpdateOfTitles(guildid, packet); break;
		case Sync::Guild::ChangeNotice: sendGuildNotice(guildid, packet); break;
		case Sync::Guild::ChangeRanks: {
			int32_t playerid = packet.get<int32_t>();
			int32_t victimid = packet.get<int32_t>();
			uint8_t rank = packet.get<uint8_t>();
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == nullptr)
				return;
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid); // Don't allow offline players..
			Player *victim = PlayerDataProvider::Instance()->getPlayer(victimid, true);

			if (player == nullptr || victim == nullptr)
				return;

			if (player->getGuild()->getId() != guildid || victim->getGuild()->getId() != guildid)
				return;
			if (victim->getGuildRank() == 1)
				return;
			if (rank <= 2 && player->getGuildRank() >= 2)
				return;
			if (rank <= 1 || rank > guild->getLowestRank())
				return;

			victim->setGuildRank(rank);

			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE characters SET guildrank = " <<  static_cast<int16_t>(rank) << " WHERE id = " << victimid << " LIMIT 1";
			query.exec();

			SyncPacket::GuildPacket::updatePlayerRank(victim);
			GuildPacket::sendRankUpdate(guild, victim);
			break;
		}
		case Sync::Guild::ChangeCapacity: {
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == nullptr)
				return;
			int32_t playerid = packet.get<int32_t>();
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
			if (player == nullptr || guild->getCapacity() == GuildsAndAlliances::GuildMaxCapacacity)
				return;

			int32_t cost = GuildsAndAlliances::GuildCapacityIncreasementCost;

			if (guild->getCapacity() == 15)
				cost *= GuildsAndAlliances::GuildIncreaseCapacityCostMultiplier::FifteenSlots;
			else if (guild->getCapacity() == 20)
				cost *= GuildsAndAlliances::GuildIncreaseCapacityCostMultiplier::TwentySlots;
			else if (guild->getCapacity() >= 25)
				cost *= GuildsAndAlliances::GuildIncreaseCapacityCostMultiplier::TwentyFiveOrMoreSlots;

			guild->setCapacity(guild->getCapacity() + 5);
			guild->save();

			SyncPacket::GuildPacket::updatePlayerMesos(player, -cost);
			GuildPacket::sendCapacityUpdate(guild);
			SyncPacket::GuildPacket::updateCapacity(guild);
			break;
		}
		case Sync::Guild::Disband: {
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == nullptr)
				return;
			Player *player = PlayerDataProvider::Instance()->getPlayer(guild->getLeader());
			if (player == nullptr)
				return;
			SyncPacket::GuildPacket::updatePlayerMesos(player, -GuildsAndAlliances::GuildDisbandCost);

			if (guild->getAlliance() != nullptr)
				SyncHandler::removeGuild(guild);

			for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++)
				SyncPacket::GuildPacket::removePlayer(iter->second);

			GuildPacket::sendGuildDisband(guild);
			SyncPacket::GuildPacket::updatePlayers(guild, true);
			SyncPacket::GuildPacket::unloadGuild(guild->getId());

			PlayerDataProvider::Instance()->removeGuild(guild);

			mysqlpp::Query deleteQuery = Database::getCharDB().query();
			deleteQuery << "DELETE FROM guilds WHERE id = " << guildid << " LIMIT 1";
			deleteQuery.exec();

			deleteQuery << "DELETE reply, thread FROM guild_bbs_replies reply JOIN guild_bbs_threads thread ON reply.thread = thread.id WHERE thread.guild = " << guildid;
			deleteQuery.exec();

			deleteQuery << "DELETE FROM guild_bbs_threads WHERE guild = " << guildid;
			deleteQuery.exec();

			deleteQuery << "UPDATE characters SET guildid = 0, guildrank = 5, allianceid = 0, alliancerank = 5 WHERE guildid = " << guildid;
			deleteQuery.exec();
			break;
		}
		case Sync::Guild::DenyInvite: {
			uint8_t result = packet.get<uint8_t>();
			Player *inviter = PlayerDataProvider::Instance()->getPlayer(packet.getString());
			Player *invitee = PlayerDataProvider::Instance()->getPlayer(packet.getString());
			if (invitee == nullptr || inviter == nullptr || !invitee->isInvitedToGuild())
				return;

			invitee->setGuildInviteId(0);
			invitee->setGuildInviteTime(0);

			GuildPacket::sendGuildDenyResult(inviter, invitee, result);
			break;
		}
		case Sync::Guild::ChangePoints: {
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == nullptr)
				return;
			guild->setGuildPoints(guild->getGuildPoints() + packet.get<int32_t>());
			guild->save();

			GuildPacket::sendGuildPointsUpdate(guild);
			break;
		}
		case Sync::Guild::ChangeEmblem: {
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == nullptr)
				return;
			int32_t playerid = packet.get<int32_t>();
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
			if (player == nullptr || player->getGuildRank() != 1 || player->getGuild() == nullptr)
				return;

			GuildLogo logo;
			logo.background = packet.get<int16_t>();
			logo.backgroundColor = packet.get<uint8_t>();
			logo.logo = packet.get<int16_t>();
			logo.color = packet.get<uint8_t>();

			guild->setLogo(logo);
			guild->save();

			SyncPacket::GuildPacket::updatePlayerMesos(player, -GuildsAndAlliances::GuildChangeEmblemCost);
			GuildPacket::sendEmblemUpdate(guild);
			SyncPacket::GuildPacket::updateEmblem(guild);
			break;
		}
		case Sync::Guild::GetRankBoard: {
			int32_t playerid = packet.get<int32_t>();
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
			if (player == nullptr)
				return;
			GuildPacket::sendGuildRankBoard(player, packet.get<int32_t>());
			break;
		}
		case Sync::Guild::Create: handleGuildCreation(packet); break;
		case Sync::Guild::RemoveEmblem: { // intentional fallthrough?
			int32_t playerid = packet.get<int32_t>();
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
			if (player == nullptr || player->getGuildRank() != 1)
				return;
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == nullptr || guild != player->getGuild())
				return;

			guild->setLogo(GuildLogo());
			guild->save();

			SyncPacket::GuildPacket::updatePlayerMesos(player, -GuildsAndAlliances::GuildRemoveEmblemCost);
			GuildPacket::sendEmblemUpdate(guild);
			SyncPacket::GuildPacket::updateEmblem(guild);
			break;
		}
	}
}

void SyncHandler::handleLoginServerPacket(LoginServerConnection *player, PacketReader &packet) {
	int32_t charid = packet.get<int32_t>();
	Player *character = PlayerDataProvider::Instance()->getPlayer(charid, true);
	if (character == nullptr || character->isOnline())
		return;
	Guild *guild = character->getGuild();
	if (guild == nullptr)
		return;

	guild->removePlayer(character);

	GuildPacket::sendPlayerUpdate(guild, character, 1, false);
	PlayerDataProvider::Instance()->removePlayer(charid);
}

void SyncHandler::loadGuild(int32_t id) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM guilds WHERE id = " << id << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();

	if ((int32_t) res.num_rows() == 0) {
		std::stringstream x;
		x << "Unable to load guild! Guildid: " << id;
		WorldServer::Instance()->log(LogTypes::Error, x.str());
		return;
	}

	GuildLogo logo;
	GuildRanks ranks;
	logo.logo = static_cast<int16_t>(res[0]["logo"]);
	logo.color = static_cast<uint8_t>(res[0]["logocolor"]);
	logo.background = static_cast<int16_t>(res[0]["logobg"]);
	logo.backgroundColor = static_cast<uint8_t>(res[0]["logobgcolor"]);
	ranks[0] = static_cast<string>(res[0]["rank1title"]);
	ranks[1] = static_cast<string>(res[0]["rank2title"]);
	ranks[2] = static_cast<string>(res[0]["rank3title"]);
	ranks[3] = static_cast<string>(res[0]["rank4title"]);
	ranks[4] = static_cast<string>(res[0]["rank5title"]);

	PlayerDataProvider::Instance()->createGuild((string) res[0]["name"],
		(string) res[0]["notice"],
		res[0]["id"],
		res[0]["leaderid"],
		res[0]["capacity"],
		res[0]["points"],
		logo,
		ranks,
		PlayerDataProvider::Instance()->getAlliance(res[0]["allianceid"]));
}

void SyncHandler::handleGuildCreation(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int8_t option = packet.get<int8_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == nullptr || player->getParty() == nullptr || player->getGuild() != nullptr)
		return;
	Party *party = player->getParty();
	if (party->members.size() < 2)
		return;

	if (option == 1) {
		if (!party->isLeader(playerid))
			return;
		string guildname = packet.getString();

		if (player->getLevel() <= 10) {
			GuildPacket::sendPlayerGuildMessage(player, 0x23);
		}
		else if (PlayerDataProvider::Instance()->getGuild(guildname) != nullptr) {
			GuildPacket::sendPlayerGuildMessage(player, 0x1c);
		}
		else {
			party->setGuildName(guildname);
			party->setGuildContract(0);
			party->setVoters(0);
			for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++)
				GuildPacket::sendGuildContract(iter->second, party->isLeader(iter->second->getId()), iter->second->getParty()->getId(), player->getName(), guildname);
		}
	}
	else if (option == 2) {
		int32_t partyid = packet.get<int32_t>();
		if (player->getParty()->getId() != partyid)
			return;
		int8_t result = packet.get<int8_t>();
		int8_t guildcontract = party->getGuildContract() + result;
		int8_t voters = party->getVoters() + 1;
		party->setGuildContract(guildcontract);
		party->setVoters(voters);
		if (voters == 5) {
			if (guildcontract == 5) {
				Player *leader = PlayerDataProvider::Instance()->getPlayer(party->getLeader());
				mysqlpp::Query query = Database::getCharDB().query();
				query << "INSERT INTO guilds (`worldid`, `name`, `leaderid`) VALUES ("
					<< static_cast<int16_t>(WorldServer::Instance()->getWorldId()) << ", "
					<< mysqlpp::quote << party->getGuildName() << ", "
					<< party->getLeader() << ")";

				if (!query.exec()) {
					std::stringstream x;
					x << "The server can't create a guild! MySQL error: " << query.error();
					WorldServer::Instance()->log(LogTypes::Error, x.str());

					GuildPacket::sendPlayerMessage(leader, 1, "Sorry, but something went wrong on the server. You didn't lose money and there was no guild created.");
					party->clearGuild();
					return;
				}

				int32_t gid = static_cast<int32_t>(query.insert_id());

				if (gid == 0) {
					std::stringstream x;
					x << "Guild creation failed! MySQL error: " << query.error();
					WorldServer::Instance()->log(LogTypes::Error, x.str());

					GuildPacket::sendPlayerMessage(leader, 1, "Sorry, but something went wrong on the server. You didn't lose money and there was no guild created.");
					party->clearGuild();
					return;
				}

				SyncPacket::GuildPacket::loadGuild(gid);
				loadGuild(gid);
				Guild *guild = PlayerDataProvider::Instance()->getGuild(gid);

				if (guild == nullptr) {
					std::stringstream x;
					x << "SyncHandler::handleGuildCreation: Unable to load the guild. Please check if the guild was created. Guild ID: " << gid;
					WorldServer::Instance()->log(LogTypes::Error, x.str());
					return;
				}

				for (map<int32_t, Player*>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
					iter->second->setGuild(guild);
					iter->second->setGuildRank(party->isLeader(iter->second->getId()) ? 1 : guild->getLowestRank());
					query << "UPDATE characters SET guildid = " << gid << ", guildrank = " << static_cast<int16_t>(iter->second->getGuildRank()) << " WHERE id = " << iter->second->getId() << " LIMIT 1;";
					query.exec();
					guild->addPlayer(iter->second);
				}

				for (map<int32_t, Player*>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
					GuildPacket::sendGuildInfo(guild, iter->second, true);
				}

				SyncPacket::GuildPacket::updatePlayers(guild, false);
				SyncPacket::GuildPacket::updatePlayerMesos(leader, -GuildsAndAlliances::GuildCreationCost);
			}
			else {
				// Sorry guys, I need this packet...
				Player *leader = PlayerDataProvider::Instance()->getPlayer(party->getLeader());
				GuildPacket::sendPlayerMessage(leader, 1, "I'm sorry but not all the partymembers have accepted the contract. Try again.");
			}
			party->clearGuild();
		}
	}
}

void SyncHandler::sendNewPlayer(int32_t guildid, int32_t pid, bool newGuild) {
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (guild == nullptr)
		return;
	Player *player = PlayerDataProvider::Instance()->getPlayer(pid, true);

	guild->addPlayer(player);

	mysqlpp::Query update = Database::getCharDB().query();
	update << "UPDATE characters SET " 
		<< "guildid = " << guild->getId() << ", "
		<< "guildrank = " << static_cast<int16_t>(player->getGuildRank()) << ", "
		<< "allianceid = " << (guild->getAlliance() == nullptr ? 0 : guild->getAlliance()->getId()) << ", "
		<< "alliancerank = " << static_cast<int16_t>(player->getAllianceRank())
		<< " WHERE ID = " << pid;
	update.exec();

	SyncPacket::GuildPacket::addPlayer(player);
	GuildPacket::sendPlayerUpdate(guild, player, 0);
	GuildPacket::sendGuildInfo(guild, player, newGuild);
	SyncPacket::GuildPacket::updatePlayer(guild, player);
}

void SyncHandler::sendGuildNotice(int32_t guildid, PacketReader &packet) {
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (guild == nullptr)
		return;

	guild->setNotice(packet.getString());
	guild->save();
	GuildPacket::sendNoticeUpdate(guild);
}

void SyncHandler::sendDeletePlayer(int32_t guildid, int32_t pid, const string &name, bool expelled) {
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (guild == nullptr)
		return;

	Player *player = PlayerDataProvider::Instance()->getPlayer(pid, true);
	if (player == nullptr)
		return;

	player->setGuild(nullptr);
	player->setGuildRank(0);
	player->setAlliance(nullptr);
	player->setAllianceRank(0);

	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET guildid = 0, guildrank = 5, allianceid = 0, alliancerank = 5 WHERE id = " << pid << " LIMIT 1";
	query.exec();

	GuildPacket::sendPlayerUpdate(guild, player, (expelled ? 2 : 1));
	guild->removePlayer(player);
	SyncPacket::GuildPacket::removePlayer(player);
	SyncPacket::GuildPacket::updatePlayer(0, player);
}

void SyncHandler::sendUpdateOfTitles(int32_t guildid, PacketReader &packet) {
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (guild == nullptr)
		return;

	for (uint8_t i = 1; i <= 5; i++)
		guild->setTitle(i, packet.getString());

	guild->save();
	GuildPacket::sendTitlesUpdate(guild);
}

void SyncHandler::sendGuildInvite(int32_t guildid, PacketReader &packet) {
	int32_t inviterid = packet.get<int32_t>();
	string name = packet.getString();
	Player *inviter = PlayerDataProvider::Instance()->getPlayer(inviterid);
	Player *invitee = PlayerDataProvider::Instance()->getPlayer(name);
	if (inviter == nullptr || invitee == nullptr || inviter->getGuild()->getId() != guildid)
		return;

	if (difftime(invitee->getGuildInviteTime(), time(0)) >= GuildsAndAlliances::InvitationAliveTime) {
		invitee->setGuildInviteTime(0);
		invitee->setGuildInviteId(0);
	}

	if (invitee->getChannel() != inviter->getChannel()) {
		GuildPacket::sendPlayerGuildMessage(inviter, 0x2a);
		return;
	}
	if (invitee->getGuild() != nullptr) {
		GuildPacket::sendPlayerGuildMessage(inviter, 0x28);
		return;
	}

	if (!invitee->isInvitedToGuild()) {
		invitee->setGuildInviteTime(time(0));
		invitee->setGuildInviteId(guildid);
	}

	GuildPacket::sendInvite(inviter, invitee);
}

void SyncHandler::silentUpdate(int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	Party *party = pplayer->getParty();
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::SilentUpdate, iter->second->getId());
		}
	}
}

void SyncHandler::logInLogOut(int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid, true);
	Party *party = pplayer->getParty();
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::LogInOrOut, iter->second->getId());
		}
	}
}

void SyncHandler::createParty(int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() != nullptr) {
		// Hacking
		return;
	}

	Party *party = new Party(PlayerDataProvider::Instance()->getPartyId());
	party->addMember(pplayer);
	party->setLeader(pplayer->getId());
	PlayerDataProvider::Instance()->addParty(party);
	pplayer->setParty(party);

	SyncPacket::PlayerPacket::sendCreateParty(pplayer->getId(), pplayer->getParty()->getId());
	SyncPacket::PartyPacket::createParty(pplayer->getChannel(), playerid);
}

void SyncHandler::giveLeader(int32_t playerid, int32_t target, bool is) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == nullptr || !pplayer->getParty()->isLeader(playerid)) {
		// Hacking
		return;
	}

	Party *party = pplayer->getParty();
	party->setLeader(target);

	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			SyncPacket::PartyPacket::giveLeader(iter->second->getChannel(), iter->second->getId(), target, is);
		}
	}

	SyncPacket::PlayerPacket::sendSwitchPartyLeader(target, pplayer->getParty()->getId());
}

void SyncHandler::expelPlayer(int32_t playerid, int32_t target) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	Player *tplayer = PlayerDataProvider::Instance()->getPlayer(target);
	if (pplayer->getParty() == nullptr || !pplayer->getParty()->isLeader(playerid)) {
		// Hacking
		return;
	}

	Party *party = pplayer->getParty();
	party->deleteMember(target);
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::Expel, iter->first, target);
		}
	}
	if (tplayer != nullptr) {
		SyncPacket::PartyPacket::updateParty(tplayer->getChannel(), PartyActions::Expel, target, target);
	}

	SyncPacket::PlayerPacket::sendRemovePartyPlayer(target, pplayer->getParty()->getId());
	PlayerDataProvider::Instance()->getPlayer(target, true)->setParty(nullptr);
}

void SyncHandler::leaveParty(int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == nullptr) {
		// Hacking
		return;
	}

	Party *party = pplayer->getParty();
	if (party->isLeader(playerid)) {
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->isOnline()) {
				SyncPacket::PartyPacket::disbandParty(iter->second->getChannel(), iter->second->getId());
				iter->second->setParty(nullptr);
			}
		}
		SyncPacket::PlayerPacket::sendDisbandParty(party->getId());
		PlayerDataProvider::Instance()->removeParty(party->getId());
	}
	else {
		SyncPacket::PlayerPacket::sendRemovePartyPlayer(pplayer->getId(), pplayer->getParty()->getId());
		party->deleteMember(pplayer->getId());
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->isOnline()) {
				SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::Leave, iter->second->getId(), playerid);
			}
		}
		SyncPacket::PartyPacket::updateParty(pplayer->getChannel(), PartyActions::Leave, playerid, playerid);
		pplayer->setParty(nullptr);
	}
}

void SyncHandler::joinParty(int32_t playerid, int32_t partyid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() != nullptr) {
		// Hacking
		return;
	}

	Party *party = PlayerDataProvider::Instance()->getParty(partyid);
	if (party->members.size() == 6) {
		SyncPacket::PartyPacket::partyError(pplayer->getChannel(), playerid, 0x11);
	}
	else {
		pplayer->setParty(party);
		party->addMember(pplayer);
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->isOnline()) {
				SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::Join, iter->second->getId(), playerid);
			}
		}
		SyncPacket::PlayerPacket::sendAddPartyPlayer(playerid, partyid);
	}
}

void SyncHandler::invitePlayer(int32_t playerid, const string &invitee) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == nullptr || !pplayer->getParty()->isLeader(playerid)) {
		// Hacking
		return;
	}

	Player *invited = PlayerDataProvider::Instance()->getPlayer(invitee);
	if (invited->isOnline() && invited->getChannel() == pplayer->getChannel()) {
		if (invited->getParty() != nullptr) {
			SyncPacket::PartyPacket::partyError(pplayer->getChannel(), playerid, 0x10);
		}
		else {
			SyncPacket::PartyPacket::invitePlayer(pplayer->getChannel(), invited->getId(), pplayer->getName());
		}
	}
	else {
		SyncPacket::PartyPacket::partyError(pplayer->getChannel(), playerid, 0x12);
	}
}

void SyncHandler::handlePlayerPacket(WorldServerAcceptConnection *connection, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Player::ChangeChannelRequest: playerChangeChannel(connection, packet); break;
		case Sync::Player::ChangeChannelGo: handleChangeChannel(connection, packet); break;
		case Sync::Player::ChangeServerRequest: playerChangeServer(connection, packet); break;
		case Sync::Player::ChangeServerGo: handleChangeServer(connection, packet); break;
		case Sync::Player::Connect: playerConnect(connection->getChannel(), packet); break;
		case Sync::Player::Disconnect: playerDisconnect(connection->getChannel(), packet); break;
		case Sync::Player::UpdateLevel: updateLevel(packet); break;
		case Sync::Player::UpdateJob: updateJob(packet); break;
		case Sync::Player::UpdateMap: updateMap(packet); break;
	}
}

void SyncHandler::playerConnect(uint16_t channel, PacketReader &packet) {
	uint32_t ip = packet.get<uint32_t>();
	int32_t id = packet.get<int32_t>();
	string name = packet.getString();
	int32_t map = packet.get<int32_t>();
	int16_t job = packet.get<int16_t>();
	uint8_t level = packet.get<uint8_t>();
	int32_t guildid = packet.get<int32_t>();
	uint8_t guildrank = packet.get<uint8_t>();
	int32_t allianceid = packet.get<int32_t>();
	uint8_t alliancerank = packet.get<uint8_t>();

	Player *p = PlayerDataProvider::Instance()->getPlayer(id, true);
	if (p == nullptr) {
		p = new Player(id);
	}

	int16_t oldJob = p->getJob();

	p->setIp(ip);
	p->setName(name);
	p->setMap(map);
	p->setJob(job);
	p->setLevel(level);
	p->setGuild(PlayerDataProvider::Instance()->getGuild(guildid));
	p->setGuildRank(guildrank);
	p->setAlliance(PlayerDataProvider::Instance()->getAlliance(allianceid));
	p->setAllianceRank(alliancerank);
	p->setOnline(true);

	if (!p->isInCashShop()) {
		p->setChannel(channel);
		if (guildid != 0) {
			GuildPacket::sendGuildInfo(p->getGuild(), p);
			if (oldJob == -1) {
				// Didn't come online till now...
				// Fun fact: GMS updates this when the PONG packet is received.
				GuildPacket::sendPlayerStatUpdate(p->getGuild(), p, false, true);
			}
			if (allianceid != 0 && oldJob == -1) {
				AlliancePacket::sendUpdatePlayer(p->getAlliance(), p, 1);
			}
		}
	}
	PlayerDataProvider::Instance()->registerPlayer(p);
}

void SyncHandler::playerDisconnect(uint16_t channel, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	PlayerDataProvider::Instance()->removePlayer(id, channel);
	int16_t chan = PlayerDataProvider::Instance()->removePendingPlayerEarly(id);
	if (chan != -1) {
		SyncPacket::PlayerPacket::sendHeldPacketRemoval(chan, id);
	}
}

void SyncHandler::partyOperation(PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	int32_t playerid = packet.get<int32_t>();
	switch (type) {
		case PartyActions::Create: createParty(playerid); break;
		case PartyActions::Leave: leaveParty(playerid); break;
		case PartyActions::Join: joinParty(playerid, packet.get<int32_t>()); break;
		case PartyActions::Invite: invitePlayer(playerid, packet.getString()); break;
		case PartyActions::Expel: expelPlayer(playerid, packet.get<int32_t>()); break;
		case PartyActions::SetLeader: giveLeader(playerid, packet.get<int32_t>(), 0); break;
	}
}

void SyncHandler::playerChangeChannel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Channel *chan = Channels::Instance()->getChannel(packet.get<int16_t>());
	if (chan) {
		SyncPacket::PlayerPacket::sendPacketToChannelForHolding(chan->getId(), playerid, packet, false);
		PlayerDataProvider::Instance()->addPendingPlayer(playerid, chan->getId());
	}
	else {
		// Channel doesn't exist (offline)
		Player *gamePlayer = PlayerDataProvider::Instance()->getPlayer(playerid);
		SyncPacket::PlayerPacket::sendCannotChangeServerToPlayer(gamePlayer->getChannel(), playerid, Sync::Player::BlockMessages::CannotGo);
	}
}

void SyncHandler::playerChangeServer(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Player *gamePlayer = PlayerDataProvider::Instance()->getPlayer(playerid, true);
	bool connect = packet.getBool();
	Channel *chan = Channels::Instance()->getChannel(gamePlayer->getChannel());
	if (connect) {
		bool cashShop = packet.getBool();
		if (cashShop) {
			if (WorldServer::Instance()->isCashServerConnected()) {
				SyncPacket::PlayerPacket::sendPacketToCashServerForHolding(playerid, packet);
			}
			else {
				SyncPacket::PlayerPacket::sendCannotChangeServerToPlayer(gamePlayer->getChannel(), playerid, Sync::Player::BlockMessages::NoCashShop);
			}
		}
		else {
			// TODO: MTS handling
			SyncPacket::PlayerPacket::sendCannotChangeServerToPlayer(gamePlayer->getChannel(), playerid, Sync::Player::BlockMessages::MtsUnavailable);
		}
	}
	else {
		if (chan) {
			SyncPacket::PlayerPacket::sendPacketToChannelForHolding(chan->getId(), playerid, packet, true);
			PlayerDataProvider::Instance()->addPendingPlayer(playerid, chan->getId());
		}
		else {
			SyncPacket::PlayerPacket::sendPlayerDisconnectServer(WorldServer::Instance()->getCashConnection(), playerid);
			std::stringstream str;
			str << "Player couldn't get back into its channel. Disconnected. (Channel: " << gamePlayer->getChannel() << ")";
			WorldServer::Instance()->log(LogTypes::Error, str.str());
		}
	}
}

void SyncHandler::handleChangeServer(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	bool connect = packet.getBool(); // If true: it connects to the server, otherwise it disconnects/goes back to the channel.
	Player *gamePlayer = PlayerDataProvider::Instance()->getPlayer(playerid, true);
	if (gamePlayer) {
		Channel *curchan = Channels::Instance()->getChannel(gamePlayer->getChannel());
		WorldServerAcceptConnection *cash = WorldServer::Instance()->getCashConnection();
		if (connect) {
			bool cashShop = packet.getBool();
			if (cashShop) {
				if (WorldServer::Instance()->isCashServerConnected()) {
					gamePlayer->setCashShop(true);
					SyncPacket::PlayerPacket::newConnectableCashServer(playerid, gamePlayer->getIp());
					uint32_t chanIp = IpUtilities::matchIpSubnet(gamePlayer->getIp(), cash->getExternalIp(), cash->getIp());
					SyncPacket::PlayerPacket::playerChangeChannel(curchan->getConnection(), playerid, chanIp, WorldServer::Instance()->getCashPort());
				}
				else {
					SyncPacket::PlayerPacket::sendCannotChangeServerToPlayer(gamePlayer->getChannel(), playerid, Sync::Player::BlockMessages::NoCashShop);
				}
			}
			else {
				SyncPacket::PlayerPacket::sendCannotChangeServerToPlayer(gamePlayer->getChannel(), playerid, Sync::Player::BlockMessages::MtsUnavailable);
			}
		}
		else {
			if (curchan) {
				gamePlayer->setCashShop(false);
				SyncPacket::PlayerPacket::newConnectable(curchan->getId(), playerid, gamePlayer->getIp());
				uint32_t chanIp = IpUtilities::matchIpSubnet(gamePlayer->getIp(), curchan->getExternalIps(), curchan->getIp());
				SyncPacket::PlayerPacket::playerChangeChannel(cash, playerid, chanIp, curchan->getPort());
			}
			else {
				//SyncPacket::PlayerPacket::sendCannotChangeServerToPlayer(cash, playerid, Sync::Player::BlockMessages::CannotGo);
			}
		}
		PlayerDataProvider::Instance()->removePendingPlayer(playerid);
	}
}

void SyncHandler::handleChangeChannel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Player *gamePlayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (gamePlayer) {
		uint16_t chanid = PlayerDataProvider::Instance()->getPendingPlayerChannel(playerid);
		Channel *chan = Channels::Instance()->getChannel(chanid);
		Channel *curchan = Channels::Instance()->getChannel(gamePlayer->getChannel());
		if (chan) {
			SyncPacket::PlayerPacket::newConnectable(chan->getId(), playerid, gamePlayer->getIp());
			uint32_t chanIp = IpUtilities::matchIpSubnet(gamePlayer->getIp(), chan->getExternalIps(), chan->getIp());
			SyncPacket::PlayerPacket::playerChangeChannel(curchan->getConnection(), playerid, chanIp, chan->getPort());
		}
		else {
			SyncPacket::PlayerPacket::sendCannotChangeServerToPlayer(gamePlayer->getChannel(), playerid, Sync::Player::BlockMessages::CannotGo);
		}
		PlayerDataProvider::Instance()->removePendingPlayer(playerid);
	}
}

void SyncHandler::updateJob(PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int16_t job = packet.get<int16_t>();
	Player *player = PlayerDataProvider::Instance()->getPlayer(id);
	player->setJob(job);
	if (player->getParty() != nullptr) {
		SyncHandler::silentUpdate(id);
	}
	if (player->getGuild() != nullptr) {
		GuildPacket::sendPlayerStatUpdate(player->getGuild(), player, false);
	}
	if (player->getAlliance() != nullptr) {
		AlliancePacket::sendUpdatePlayer(player->getAlliance(), player, 1);
	}
}

void SyncHandler::updateLevel(PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	uint8_t level = packet.get<uint8_t>();
	Player *player = PlayerDataProvider::Instance()->getPlayer(id, true);
	player->setLevel(level);
	if (player->getParty() != nullptr) {
		SyncHandler::silentUpdate(id);
	}
	if (player->getGuild() != nullptr) {
		GuildPacket::sendPlayerStatUpdate(player->getGuild(), player, true);
	}
	if (player->getAlliance() != nullptr) {
		AlliancePacket::sendUpdatePlayer(player->getAlliance(), player, 1);
	}
}

void SyncHandler::updateMap(PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int32_t map = packet.get<int32_t>();
	if (Player *p = PlayerDataProvider::Instance()->getPlayer(id)) {
		p->setMap(map);
		if (p->getParty() != nullptr) {
			SyncHandler::silentUpdate(id);
		}
	}
}

void SyncHandler::handleBuddyPacket(PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Buddies::Invite: buddyInvite(packet); break;
		case Sync::Buddies::OnlineOffline: buddyOnline(packet); break;
	}
}

void SyncHandler::buddyInvite(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Player *inviter = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (inviter == nullptr) {
		// No idea how this would happen... Lets take no risk and just return.
		return;
	}

	int32_t inviteeId = packet.get<int32_t>();
	if (Player *invitee = PlayerDataProvider::Instance()->getPlayer(inviteeId)) {
		SyncPacket::BuddyPacket::sendBuddyInvite(Channels::Instance()->getChannel(invitee->getChannel())->getConnection(), inviteeId, playerid, inviter->getName());
	}
	else {
		// Make new pending buddy in the database
		mysqlpp::Query query = Database::getCharDB().query();
		query << "INSERT INTO buddylist_pending VALUES ("
			<< inviteeId << ", "
			<< mysqlpp::quote << inviter->getName() << ", "
			<< playerid << ")";
		query.exec();
	}
}

void SyncHandler::buddyOnline(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == nullptr) {
		// No idea how this would happen... Lets take no risk and just return.
		return;
	}

	bool online = packet.getBool();

	vector<int32_t> tempIds = packet.getVector<int32_t>();
	unordered_map<int16_t, vector<int32_t> > ids; // <channel, <ids>> , for sending less packets for a buddylist of 100 people

	int32_t id = 0;
	for (size_t i = 0; i < tempIds.size(); i++) {
		id = tempIds[i];
		if (Player *player = PlayerDataProvider::Instance()->getPlayer(id)) {
			if (player->isOnline() && !player->isInCashShop()) {
				ids[player->getChannel()].push_back(id);
			}
		}
	}

	for (unordered_map<int16_t, vector<int32_t> >::iterator iter = ids.begin(); iter != ids.end(); iter++) {
		if (Channel *channel = Channels::Instance()->getChannel(iter->first)) {
			SyncPacket::BuddyPacket::sendBuddyOnlineOffline(channel->getConnection(), iter->second, playerid, (online ? player->getChannel() : -1));
		}
	}
}
