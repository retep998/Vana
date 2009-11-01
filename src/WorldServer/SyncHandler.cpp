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
#include "SyncHandler.h"
#include "Alliance.h"
#include "AlliancePacket.h"
#include "Channel.h"
#include "Channels.h"
#include "Database.h"
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

void SyncHandler::handleAlliancePacket(WorldServerAcceptConnection *player, PacketReader &packet) {
	int8_t option = packet.get<int8_t>();
	int32_t allianceid = packet.get<int32_t>();
	switch (option) {
		case 0x02: sendAllianceInfo(packet.get<int32_t>(), allianceid); break;
		case 0x03: sendTitleUpdate(allianceid, packet); break;
		case 0x04: sendNoticeUpdate(allianceid, packet); break;
		case 0x05: sendInvite(allianceid, packet); break;
		case 0x06: sendDenyPacket(packet); break;
		case 0x07: sendChangeGuild(allianceid, packet); break;
		case 0x08: sendChangeLeader(allianceid, packet); break;
		case 0x09: sendPlayerChangeRank(allianceid, packet); break;
		case 0x0a: handleAllianceCreation(packet); break;
		case 0x0b: sendIncreaseCapacity(allianceid, packet.get<int32_t>()); break;
		case 0x0c: sendAllianceDisband(allianceid, packet.get<int32_t>()); break;
	}
}

void SyncHandler::handleAllianceCreation(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	string alliancename = packet.getString();
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == 0 || player->getParty() == 0 || player->getGuild() == 0 || 
		player->getAlliance() != 0 || player->getGuildRank() != 1) 
		return;

	Party *party = player->getParty();
	if (party->members.size() != 2) 
		return;

	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++)
		if (iter->second->getAlliance() != 0 || iter->second->getGuild() == 0)
			return;
	
	// There we go, create an alliance...

	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO alliances (id, leader, worldid, name, notice, rank4title, rank5title) VALUES (NULL, "
		<< party->getLeader() << ", "
		<< static_cast<int16_t>(WorldServer::Instance()->getWorldId()) << ", "
		<< mysqlpp::quote << alliancename << ", "
		<< mysqlpp::quote << "" << ", "
		<< mysqlpp::quote << "" << ", "
		<< mysqlpp::quote << "" << ");";

	if (!query.exec()) {
		std::cout << "\a Warning! " << player->getName() << " has created " << alliancename << " but it failed! " << query.error() << std::endl;
		return;
	}

	int32_t allianceid = static_cast<int32_t>(query.insert_id());
	
	loadAlliance(allianceid);
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);

	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		Guild *playerGuild = iter->second->getGuild();
		if (playerGuild != 0) {
			for (unordered_map<int32_t, Player *>::iterator iter2 = playerGuild->m_players.begin(); iter2 != playerGuild->m_players.end(); iter2++) {
				if (iter2->second->getAlliance() != 0 || iter2->second->getGuild() == 0) 
					continue;

				iter2->second->setAlliance(alliance);

				if (party->getLeader() == iter2->second->getId()) // The party/alliance leader
					iter2->second->setAllianceRank(1);
				else if (iter->second->getId() == iter2->second->getId()) // The other member in the party
					iter2->second->setAllianceRank(2);
				else // The other members
					iter2->second->setAllianceRank(3);
				query << "UPDATE characters SET alliance = " << allianceid << ", "
					<< "alliancerank = " <<	static_cast<int16_t>(iter2->second->getAllianceRank()) << " WHERE id = " << iter2->second->getId() << " LIMIT 1";
				query.exec();
			}

			playerGuild->setAlliance(alliance);
			query << "UPDATE guilds SET alliance = " << allianceid << " WHERE id = " << iter->second->getGuild()->getId() << " LIMIT 1";
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
		std::cout << "Can't load alliance! Alliance ID " << allianceid << std::endl;
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
	if (alliance == 0)
		return;
	Player * player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == 0) 
		return;
	AlliancePacket::sendAllianceInfo(alliance, player);
}

void SyncHandler::sendChangeGuild(int32_t allianceid, PacketReader &packet) {
	int32_t guildid = packet.get<int32_t>();
	int32_t playerid = packet.get<int32_t>();
	uint8_t option = packet.get<uint8_t>();
	Alliance *alliance = 0;

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

	if (alliance == 0 || (guild == 0 && option == 0) || player == 0 || (option != 0 && player->getAlliance() != alliance)) 
		return;

	if (option == 0) {
		if (guild->getInvited() && guild->getInvitedId() == alliance->getId()) {
			alliance->addGuild(guild); // add the guild before sending the packet
			
			guild->removeInvite();
			guild->setAlliance(alliance);

			int16_t lowestAllianceRank = alliance->getLowestRank();

			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE characters SET alliance = " << allianceid << ", alliancerank = " << lowestAllianceRank << " WHERE guild = " << guild->getId();
			query.exec();

			query << "UPDATE characters SET alliancerank = 2 WHERE id = " << guild->getLeader(); // Make the leader jr master
			query.exec();

			query << "UPDATE guilds SET alliance = " << allianceid << " WHERE id = " << guild->getId(); // Update the guild in the database
			query.exec();

			SyncPacket::AlliancePacket::changeGuild(alliance, guild);

			for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
				if (iter->second->getGuildRank() == 1)
					iter->second->setAllianceRank(2);
				else
					iter->second->setAllianceRank(static_cast<uint8_t>(lowestAllianceRank));

				iter->second->setAlliance(alliance);
			}

			AlliancePacket::sendInviteAccepted(alliance, guild);
		}
		else
			return;
	}
	else {
		guild->setAlliance(0);

		SyncPacket::AlliancePacket::changeGuild(0, guild);

		for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
			iter->second->setAllianceRank(5);
			iter->second->setAlliance(0);
		}

		mysqlpp::Query query = Database::getCharDB().query();
		query << "UPDATE characters SET alliance = 0, alliancerank = 5 WHERE guild = " << guild->getId();
		query.exec();

		query << "UPDATE guilds SET alliance = 0 WHERE id = " << guild->getId(); // Update the guild in the database
		query.exec();

		alliance->removeGuild(guild);
		AlliancePacket::sendGuildLeft(alliance, guild, option == 1 ? true : false);
	}
}

void SyncHandler::sendNoticeUpdate(int32_t allianceid, PacketReader &packet) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	if (alliance == 0) 
		return;
	Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
	if (player == 0 || player->getAllianceRank() > 2 || player->getGuild() == 0) 
		return;

	alliance->setNotice(packet.getString());
	alliance->save();

	AlliancePacket::sendUpdateNotice(alliance);
}

void SyncHandler::sendPlayerUpdate(int32_t allianceid, int32_t playerid) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	if (alliance == 0) 
		return;
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == 0 || player->getAlliance() == 0 || player->getGuild() == 0) 
		return;
	
	AlliancePacket::sendUpdatePlayer(alliance, player, 1);
}

void SyncHandler::sendTitleUpdate(int32_t allianceid, PacketReader &packet) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	if (alliance == 0) 
		return;
	Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
	if (player == 0 || player->getAllianceRank() > 1 || player->getGuild() == 0) 
		return;

	alliance->setTitle(0, packet.getString());
	alliance->setTitle(1, packet.getString());
	alliance->setTitle(2, packet.getString());
	alliance->setTitle(3, packet.getString());
	alliance->setTitle(4, packet.getString());

	alliance->save();

	AlliancePacket::sendUpdateTitles(alliance);
}

void SyncHandler::sendPlayerChangeRank(int32_t allianceid, PacketReader &packet) {
	int32_t changerid = packet.get<int32_t>();
	int32_t victimid = packet.get<int32_t>();

	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	Player *changer = PlayerDataProvider::Instance()->getPlayer(changerid);
	Player *victim = PlayerDataProvider::Instance()->getPlayer(victimid, true);

	if (alliance == 0 || changer == 0 || victim == 0) 
		return;

	if (changer->getAllianceRank() > 2 || changer->getGuild() == 0 || changer->getAlliance() == 0 || 
		changer->getAlliance() != alliance) 
		return;
	if (victim->getAllianceRank() == 1 || victim->getGuild() == 0 || victim->getAlliance() == 0 || 
		victim->getAlliance() != alliance) 
		return;

	// Client sends a zero for increasing the rank, and an one for decreasing the rank....
	uint8_t option = packet.get<uint8_t>();

	if (option == 1 && victim->getAllianceRank() > 2)
		victim->setAllianceRank(victim->getAllianceRank() - 1);
	else if (option == 0 && victim->getAllianceRank() > 2 && victim->getAllianceRank() < 5)
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

	if (alliance == 0 || guild == 0 || guild->getLeader() == inviter->getId()) 
		return;

	Player *invitee = PlayerDataProvider::Instance()->getPlayer(guild->getLeader());
	if (invitee == 0 || inviter == 0 || inviter->getAllianceRank() > 1 || 
		inviter->getGuild() == 0 || inviter->getAlliance() == 0 || invitee->getGuild() == 0) 
		return;

	if (!guild->getInvited()) {
		guild->setInvite(allianceid);
		AlliancePacket::sendInvite(alliance, inviter, invitee);
	}
}

void SyncHandler::sendAllianceDisband(int32_t allianceid, int32_t playerid) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);

	if (alliance == 0 || player == 0 || player->getAlliance() == 0 || 
		player->getGuild() == 0 || player->getAllianceRank() != 1 || 
		player->getAlliance() != alliance || alliance->getLeaderId() != playerid) 
		return;

	// Todo: update the characters and guilds so the alliance gets deleted!

	unordered_map<int32_t, Guild *> guilds = alliance->getGuilds();
	unordered_map<int32_t, Guild *>::iterator iter;
	unordered_map<int32_t, Player *>::iterator iter2;

	for (iter = guilds.begin(); iter != guilds.end(); iter++) {
		for (iter2 = iter->second->m_players.begin(); iter2 != iter->second->m_players.end(); iter2++) {
			iter2->second->setAlliance(0);
			iter2->second->setAllianceRank(5);
		}
		iter->second->setAlliance(0);
	}

	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET alliance = 0, alliancerank = 5 WHERE alliance = " << allianceid;
	query.exec();

	query << "UPDATE guilds SET alliance = 0 WHERE alliance = " << allianceid; // Update the guild in the database
	query.exec();

	query << "DELETE FROM alliances WHERE id = " << allianceid; // Update the guild in the database
	query.exec();
	
	AlliancePacket::sendDeleteAlliance(alliance);
	SyncPacket::AlliancePacket::changeAlliance(alliance, 0);

	delete alliance;
}

void SyncHandler::sendChangeLeader(int32_t allianceid, PacketReader &packet) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
	Player *victim = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());

	if (alliance == 0 || player == 0 || victim == 0 || 
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

	if (alliance == 0 || player == 0 || player->getAlliance() != alliance || 
		player->getAllianceRank() != 1) 
		return;

	alliance->setCapacity(alliance->getCapacity() + 1);
	alliance->save();

	SyncPacket::GuildPacket::updatePlayerMesos(player, -1000000);

	AlliancePacket::sendUpdateCapacity(alliance);
	SyncPacket::AlliancePacket::changeCapacity(alliance);
}

void SyncHandler::sendDenyPacket(PacketReader &packet) {
	Player *denier = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>());
	if (denier == 0 || denier->getGuild() == 0 || denier->getGuildRank() != 1) 
		return;

	Guild *dguild = denier->getGuild();
	if (!dguild->getInvited()) 
		return;

	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(dguild->getInvitedId());
	if (alliance == 0)
		return;

	AlliancePacket::sendInviteDenied(alliance, dguild);
}

void SyncHandler::removeGuild(Guild *guild) {
	Alliance *alliance = guild->getAlliance();
	guild->setAlliance(0);

	SyncPacket::AlliancePacket::changeGuild(0, guild);

	for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
		iter->second->setAlliance(0);
		iter->second->setAllianceRank(5);
	}

	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET alliance = 0, alliancerank = 5 WHERE guild = " << guild->getId();
	query.exec();

	query << "UPDATE guilds SET alliance = 0 WHERE id = " << guild->getId(); // Update the guild in the database
	query.exec();

	alliance->removeGuild(guild);
	AlliancePacket::sendGuildLeft(alliance, guild, false);
}

void SyncHandler::handleBbsPacket(WorldServerAcceptConnection *player, PacketReader &packet) {
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
	if (guild == 0 || player == 0)
		return;

	GuildBbs *bbs = guild->getBbs();

	if (guildid != player->getGuild()->getId()) {
		std::cout << player->getName() << " wants to post a new thread without being in the guild!" << std::endl;
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

	if (isNotice && bbs->getNotice() != 0)
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
		if (thread == 0)
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
	if (player == 0 || guild == 0) 
		return;

	BbsThread *thread = guild->getBbs()->getThread(threadid);
	if (thread == 0)
		return;

	SyncPacket::BbsPacket::sendThreadData(player->getChannel(), thread, playerid);
}

void SyncHandler::handleShowThread(PacketReader &pack) {
	int32_t playerid = pack.get<int32_t>();
	int32_t guildid = pack.get<int32_t>();
	int32_t threadid = pack.get<int32_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (player == 0 || player->getGuild() == 0 || guild == 0) 
		return;

	BbsThread *thread = guild->getBbs()->getThread(threadid);
	if (thread == 0)
		return;

	SyncPacket::BbsPacket::sendThreadData(player->getChannel(), thread, playerid);
}

void SyncHandler::handleDeleteThread(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int32_t guildid = packet.get<int32_t>();
	int32_t threadid = packet.get<int32_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild *guild = player->getGuild();
	if (player == 0 || guild == 0 || player->getGuild()->getId() != guildid) 
		return;

	GuildBbs *bbs = guild->getBbs();

	BbsThread * thread = bbs->getThread(threadid);
	if (thread == 0 || (thread->getUserId() != playerid && player->getGuildRank() < 2)) 
		return;

	bbs->removeThread(threadid);
	bbs->save();
}

void SyncHandler::handleShowThreadList(PacketReader &pack) {
	int32_t playerid = pack.get<int32_t>();
	int32_t guildid = pack.get<int32_t>();
	int16_t page = pack.get<int16_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	Guild * guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (player == 0 || player->getGuild() == 0 || guild == 0) 
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
	if (player == 0 || player->getGuild() == 0 || guild == 0) 
		return;

	BbsThread * thread = guild->getBbs()->getThread(threadid);
	if (thread == 0) 
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
	if (player == 0 || player->getGuild() == 0 || guild == 0) 
		return;

	BbsThread *thread = guild->getBbs()->getThread(threadid);
	if (thread == 0) 
		return;

	BbsReply *reply = thread->getReply(replyid);
	if (reply == 0 || (reply->getUserId() != playerid && player->getGuildRank() < 2))
		return;

	thread->removeReply(replyid);
	thread->saveReplies();
	thread->loadReplies();

	handleShowThread(playerid, thread->getListId());
}

void SyncHandler::handleGuildPacket(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t option = packet.get<int8_t>();
	int32_t guildid = packet.get<int32_t>();
	switch(option) {
		case 0x01: sendGuildInvite(guildid, packet); break; // Invite a player
		case 0x02: { // Expel a player or leave
			Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>(), true);
			if (player == 0)
				return;
			string name = packet.getString();
			sendDeletePlayer(guildid, player->getId(), name, (packet.get<int8_t>() == 1 ? true : false));
			break;
		}
		case 0x03: { // Accept invite
			int32_t playerid = packet.get<int32_t>();
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
			if (player == 0)
				return;
			int32_t inviteGuildId = player->getGuildInviteId();
			if (!player->isInvitedToGuild() || difftime(player->getGuildInviteTime(), time(0)) >= 300) {
				if (difftime(player->getGuildInviteTime(), time(0)) >= 300) {
					player->setGuildInviteTime(0);
					player->setGuildInviteId(0);
				}
				return;
			}
			Guild *guild = PlayerDataProvider::Instance()->getGuild(inviteGuildId);
			if (guild == 0) 
				return;
			player->setGuildInviteId(0);
			player->setGuildInviteTime(0);
			player->setGuild(guild);
			player->setGuildRank(guild->getLowestRank());
			player->setAlliance(guild->getAlliance() != 0 ? guild->getAlliance() : 0);
			player->setAllianceRank(guild->getAlliance() != 0 ? guild->getAlliance()->getLowestRank() : 0);
			sendNewPlayer(inviteGuildId, playerid, false);
			break;
		}
		case 0x04: sendUpdateOfTitles(guildid, packet); break; // Change the titles
		case 0x05: sendGuildNotice(guildid, packet); break; // Change the notice
		case 0x06: { // Change someones rank
			int32_t playerid = packet.get<int32_t>();
			int32_t victimid = packet.get<int32_t>();
			uint8_t rank = packet.get<uint8_t>();
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == 0) 
				return;
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid); // Don't allow offline players..
			Player *victim = PlayerDataProvider::Instance()->getPlayer(victimid, true);

			if (player == 0 || victim == 0)
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
		case 0x07: { // Add guild capacity
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == 0) 
				return;
			int32_t playerid = packet.get<int32_t>();
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
			if (player == 0 || guild->getCapacity() == 100)
				return;

			int32_t cost = 500000;

			if (guild->getCapacity() == 15)
				cost *= 3;
			else if (guild->getCapacity() == 20)
				cost *= 5;
			else if (guild->getCapacity() >= 25)
				cost *= 7;

			guild->setCapacity(guild->getCapacity() + 5);
			guild->save();

			SyncPacket::GuildPacket::updatePlayerMesos(player, -cost);
			GuildPacket::sendCapacityUpdate(guild);
			SyncPacket::GuildPacket::updateCapacity(guild);
			break;
		}
		case 0x08: { // Disband a guild
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == 0)
				return;
			Player *player = PlayerDataProvider::Instance()->getPlayer(guild->getLeader());
			if (player == 0)
				return;
			SyncPacket::GuildPacket::updatePlayerMesos(player, -200000);

			if (guild->getAlliance() != 0)
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

			deleteQuery << "UPDATE characters SET guild = 0, guildrank = 5, alliance = 0, alliancerank = 5 WHERE guild = " << guildid;
			deleteQuery.exec();
			}
		break;
		case 0x09: { // Invite Denied
			uint8_t result = packet.get<uint8_t>();
			Player *invitee = PlayerDataProvider::Instance()->getPlayer(packet.getString());
			Player *inviter = PlayerDataProvider::Instance()->getPlayer(packet.getString());
			if (invitee == 0 || inviter == 0 || !invitee->isInvitedToGuild()) 
				return;

			invitee->setGuildInviteId(0);
			invitee->setGuildInviteTime(0);

			GuildPacket::sendGuildDenyResult(inviter, invitee, result);
			break;
		}
		case 0x0a: { // Add or remove guild points
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == 0) 
				return;
			guild->setGuildPoints(guild->getGuildPoints() + packet.get<int32_t>());
			guild->save();

			GuildPacket::sendGuildPointsUpdate(guild);
			break;
		}
		case 0x0b: { // Change emblem
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == 0) 
				return;
			int32_t playerid = packet.get<int32_t>();
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
			if (player == 0 || player->getGuildRank() != 1 || player->getGuild() == 0) 
				return;

			GuildLogo logo;
			logo.background = packet.get<int16_t>();
			logo.backgroundColor = packet.get<uint8_t>();
			logo.logo = packet.get<int16_t>();
			logo.color = packet.get<uint8_t>();

			guild->setLogo(logo);
			guild->save();

			SyncPacket::GuildPacket::updatePlayerMesos(player, -5000000);
			GuildPacket::sendEmblemUpdate(guild);
			SyncPacket::GuildPacket::updateEmblem(guild);
			break;
		}
		case 0x0c: { // Send guild rank board
			int32_t playerid = packet.get<int32_t>();
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
			if (player == 0)
				return;
			GuildPacket::sendGuildRankBoard(player, packet.get<int32_t>());
			break;
		}
		case 0x0d: handleGuildCreation(packet); break; // Guild contract check/creation
		case 0x0e: { // Remove a character from the guild (Loginserver -> Worldserver packet)
			Player *player = PlayerDataProvider::Instance()->getPlayer(packet.get<int32_t>(), true);
			if (player == 0)
				return;

			Guild *guild = player->getGuild();
			if (guild == 0)
				return;

			SyncPacket::GuildPacket::updatePlayer(0, player);
			GuildPacket::sendPlayerUpdate(guild, player, 1);
		}
		case 0x0f: { // Remove emblem, intentional fallthrough?
			int32_t playerid = packet.get<int32_t>();
			Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
			if (player == 0 || player->getGuildRank() != 1) 
				return;
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == 0 || guild != player->getGuild()) 
				return;

			guild->setLogo(GuildLogo());
			guild->save();
			
			SyncPacket::GuildPacket::updatePlayerMesos(player, -1000000);
			GuildPacket::sendEmblemUpdate(guild);
			SyncPacket::GuildPacket::updateEmblem(guild);
			break;
		}
	}
}

void SyncHandler::handleLoginServerPacket(LoginServerConnection *player, PacketReader &packet) {
	int32_t charid = packet.get<int32_t>();
	Player *character = PlayerDataProvider::Instance()->getPlayer(charid);
	if (character == 0)
		return;
	Guild *guild = character->getGuild();
	if (guild == 0)
		return;
	
	guild->removePlayer(character);

	GuildPacket::sendPlayerUpdate(guild, character, 1);
}

void SyncHandler::loadGuild(int32_t id) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM guilds WHERE id = " << id << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();

	if ((int32_t) res.num_rows() == 0) {
		std::cout << "\aAlert! Can't load a guild! Guild ID: " << id << " >_>" << std::endl;
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
		res[0]["gp"],
		logo,
		ranks,
		PlayerDataProvider::Instance()->getAlliance(res[0]["alliance"]));
}

void SyncHandler::handleGuildCreation(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int8_t option = packet.get<int8_t>();

	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == 0 || player->getParty() == 0 || player->getGuild() != 0) 
		return;
	Party *party = player->getParty();
	if (party->members.size() < 2) 
		return;

	if (option == 1) { // Check the guildname and send the contract around
		if (!party->isLeader(playerid)) 
			return;
		string guildname = packet.getString();
		if (player->getLevel() <= 10) {
			GuildPacket::sendPlayerGuildMessage(player, 0x23);
			return;
		}
		if (PlayerDataProvider::Instance()->getGuild(guildname) != 0) {
			GuildPacket::sendPlayerGuildMessage(player, 0x1c);
		}
		else {
			party->setGuildName(guildname);
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
		if (voters == 1) {
			if (guildcontract == 1) {
				Player *leader = PlayerDataProvider::Instance()->getPlayer(party->getLeader());
				mysqlpp::Query query = Database::getCharDB().query();
				query << "INSERT INTO guilds (`world`, `name`, `leaderid`, `notice`, `rank4title`, `rank5title`, `capacity`) VALUES ("
					<< static_cast<int16_t>(WorldServer::Instance()->getWorldId()) << ", "
					<< mysqlpp::quote << party->getGuildName() << ", "
					<< party->getLeader() << ", "
					<< mysqlpp::quote << "" << ", "
					<< mysqlpp::quote << "" << ", "
					<< mysqlpp::quote << "" << ", "
					<< "10)";

				if (!query.exec()) {
					std::cout << "/a[ALERT] The server cant create a guild! MySQL error: " << query.error() << std::endl;
					Player *leader = PlayerDataProvider::Instance()->getPlayer(party->getLeader());
					GuildPacket::sendPlayerMessage(leader, 1, "Sorry, but something went wrong on the server. You didn't lose money and there was no guild created.");
					party->clearGuild();
					return;
				}

				int32_t gid = static_cast<int32_t>(query.insert_id());

				if (gid == 0) {
					std::cout << "/a[ALERT] The server can't load a guild! MySQL error: " << query.error() << std::endl;
					GuildPacket::sendPlayerMessage(leader, 1, "Sorry, but something went wrong on the server. You didn't lose money and there was no guild created.");
					party->clearGuild();
					return;
				}

				SyncPacket::GuildPacket::loadGuild(gid);
				loadGuild(gid);
				Guild *guild = PlayerDataProvider::Instance()->getGuild(gid);

				if (guild == 0) {
					std::cout << "\aSyncHandler::handleGuildCreation(): The code cannot load the guild. Please check if the guild was inserted into the database. Guildid: " << gid << std::endl;
					return;
				}

				for (map<int32_t, Player*>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
					iter->second->setGuild(guild);
					iter->second->setGuildRank(party->isLeader(iter->second->getId()) ? 1 : guild->getLowestRank());
					query << "UPDATE characters SET guild = " << gid << ", guildrank = " << static_cast<int16_t>(iter->second->getGuildRank()) << " WHERE id = " << iter->second->getId() << " LIMIT 1;";
					query.exec();
					guild->addPlayer(iter->second);
				}

				for (map<int32_t, Player*>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
					GuildPacket::sendGuildInfo(guild, iter->second, true);
				}

				SyncPacket::GuildPacket::updatePlayers(guild, false);
				SyncPacket::GuildPacket::updatePlayerMesos(leader, -1500000);
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
	if (guild == 0) 
		return;
	Player *player = PlayerDataProvider::Instance()->getPlayer(pid, true);
	
	guild->addPlayer(player);

	mysqlpp::Query update = Database::getCharDB().query();
	update << "UPDATE characters SET " 
		<< "guild = " << guild->getId() << ", "
		<< "guildrank = " <<  static_cast<int16_t>(player->getGuildRank()) << ", "
		<< "alliance = " << guild->getAlliance()->getId() << ", "
		<< "alliancerank = " <<  static_cast<int16_t>(player->getAllianceRank())
		<< " WHERE ID = " << pid;
	update.exec();
	
	SyncPacket::GuildPacket::addPlayer(player);
	GuildPacket::sendPlayerUpdate(guild, player, 0);
	GuildPacket::sendGuildInfo(guild, player, newGuild);
	SyncPacket::GuildPacket::updatePlayer(guild, player);
}

void SyncHandler::sendGuildNotice(int32_t guildid, PacketReader &packet) {
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (guild == 0) 
		return;

	guild->setNotice(packet.getString());
	guild->save();
	GuildPacket::sendNoticeUpdate(guild);
}

void SyncHandler::sendDeletePlayer(int32_t guildid, int32_t pid, const string &name, bool expelled) {
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (guild == 0) 
		return;

	Player *player = PlayerDataProvider::Instance()->getPlayer(pid, true);
	if (player == 0)
		return;

	player->setGuild(0);
	player->setGuildRank(0);
	player->setAlliance(0);
	player->setAllianceRank(0);

	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET guild = 0, guildrank = 5, alliance = 0, alliancerank = 5 WHERE id = " << pid << " LIMIT 1";
	query.exec();

	GuildPacket::sendPlayerUpdate(guild, player, (expelled ? 2 : 1));
	guild->removePlayer(player);
	SyncPacket::GuildPacket::removePlayer(player);
	SyncPacket::GuildPacket::updatePlayer(0, player);
}

void SyncHandler::sendUpdateOfTitles(int32_t guildid, PacketReader &packet) {
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (guild == 0) 
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
	if (inviter == 0 || invitee == 0 || inviter->getGuild()->getId() != guildid) 
		return;

	if (difftime(invitee->getGuildInviteTime(), time(0)) >= 300) {
		invitee->setGuildInviteTime(0);
		invitee->setGuildInviteId(0);
	}

	if (invitee->getChannel() != inviter->getChannel()) {
		GuildPacket::sendPlayerGuildMessage(inviter, 0x2a);
		return;
	}
	if (invitee->getGuild() != 0) {
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

void SyncHandler::createParty(WorldServerAcceptConnection *player, int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() != 0) {
		// Hacking
		return;
	}
	Party *party = new Party(PlayerDataProvider::Instance()->getPartyId());
	party->addMember(pplayer);
	party->setLeader(pplayer->getId());
	PlayerDataProvider::Instance()->addParty(party);
	pplayer->setParty(party);

	SyncPacket::PartyPacket::createParty(pplayer->getChannel(), playerid);

	WorldServerAcceptPacket::sendCreateParty(pplayer->getId(), pplayer->getParty()->getId());
}

void SyncHandler::giveLeader(WorldServerAcceptConnection *player, int32_t playerid, int32_t target, bool is) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == 0 || !pplayer->getParty()->isLeader(playerid)) {
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
	WorldServerAcceptPacket::sendSwitchPartyLeader(target, pplayer->getParty()->getId());
}

void SyncHandler::expelPlayer(WorldServerAcceptConnection *player, int32_t playerid, int32_t target) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	Player *tplayer = PlayerDataProvider::Instance()->getPlayer(target);
	if (pplayer->getParty() == 0 || !pplayer->getParty()->isLeader(playerid)) {
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
	if (tplayer != 0) {
		SyncPacket::PartyPacket::updateParty(tplayer->getChannel(), PartyActions::Expel, target, target);
	}
	WorldServerAcceptPacket::sendRemovePartyPlayer(target, pplayer->getParty()->getId());
	PlayerDataProvider::Instance()->getPlayer(target, true)->setParty(0);
}

void SyncHandler::leaveParty(WorldServerAcceptConnection *player, int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == 0) {
		// Hacking
		return;
	}
	Party *party = pplayer->getParty();
	if (party->isLeader(playerid)) {
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->isOnline()) {
				SyncPacket::PartyPacket::disbandParty(iter->second->getChannel(), iter->second->getId());
				iter->second->setParty(0);
			}
		}
		WorldServerAcceptPacket::sendDisbandParty(party->getId());
		PlayerDataProvider::Instance()->removeParty(party->getId());
	}
	else {
		WorldServerAcceptPacket::sendRemovePartyPlayer(pplayer->getId(), pplayer->getParty()->getId());
		party->deleteMember(pplayer->getId());
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->isOnline()) {
				SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::Leave, iter->second->getId(), playerid);
			}
		}
		SyncPacket::PartyPacket::updateParty(pplayer->getChannel(), PartyActions::Leave, playerid, playerid);
		pplayer->setParty(0);
	}
}

void SyncHandler::joinParty(WorldServerAcceptConnection *player, int32_t playerid, int32_t partyid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() != 0) {
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
		WorldServerAcceptPacket::sendAddPartyPlayer(playerid, partyid);
	}
}

void SyncHandler::invitePlayer(WorldServerAcceptConnection *player, int32_t playerid, const string &invitee) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == 0 || !pplayer->getParty()->isLeader(playerid)) {
		// Hacking
		return;
	}
	Player *invited = PlayerDataProvider::Instance()->getPlayer(invitee);
	if (invited->isOnline() && invited->getChannel() == pplayer->getChannel()) {
		if (invited->getParty() != 0) {
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

void SyncHandler::playerConnect(WorldServerAcceptConnection *player, PacketReader &packet) {
	uint32_t ip = packet.get<uint32_t>();
	int32_t id = packet.get<int32_t>();
	string name = packet.getString();
	int32_t map = packet.get<int32_t>();
	int16_t job = static_cast<int16_t>(packet.get<int32_t>());
	uint8_t level = static_cast<uint8_t>(packet.get<int32_t>());
	int32_t guildid = packet.get<int32_t>();
	uint8_t guildrank = packet.get<uint8_t>();
	int32_t allianceid = packet.get<int32_t>();
	uint8_t alliancerank = packet.get<uint8_t>();
	
	Player *p = PlayerDataProvider::Instance()->getPlayer(id, true);
	if (p == 0) {
		p = new Player(id);
	}
	p->setIp(ip);
	p->setName(name);
	p->setMap(map);
	p->setJob(job);
	p->setLevel(level);
	p->setGuild(PlayerDataProvider::Instance()->getGuild(guildid));
	p->setGuildRank(guildrank);
	p->setAlliance(PlayerDataProvider::Instance()->getAlliance(allianceid));
	p->setAllianceRank(alliancerank);
	p->setChannel(player->getChannel());
	p->setOnline(true);
	PlayerDataProvider::Instance()->registerPlayer(p);
}

void SyncHandler::playerDisconnect(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	PlayerDataProvider::Instance()->remove(id, player->getChannel());
	int16_t channel = PlayerDataProvider::Instance()->removePendingPlayerEarly(id);
	if (channel != -1) {
		WorldServerAcceptPacket::sendHeldPacketRemoval(channel, id);
	}
}
void SyncHandler::partyOperation(WorldServerAcceptConnection *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	int32_t playerid = packet.get<int32_t>();
	switch (type) {
		case 0x01: SyncHandler::createParty(player, playerid); break;
		case 0x02: SyncHandler::leaveParty(player, playerid); break;
		case 0x03: SyncHandler::joinParty(player, playerid, packet.get<int32_t>()); break;
		case 0x04: SyncHandler::invitePlayer(player, playerid, packet.getString()); break;
		case 0x05: SyncHandler::expelPlayer(player, playerid, packet.get<int32_t>()); break;
		case 0x06: SyncHandler::giveLeader(player, playerid, packet.get<int32_t>(), 0); break;
	}
}

void SyncHandler::playerChangeChannel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Channel *chan = Channels::Instance()->getChannel(packet.get<int16_t>());
	if (chan) {
		WorldServerAcceptPacket::sendPacketToChannelForHolding(chan->getId(), playerid, packet);
		PlayerDataProvider::Instance()->addPendingPlayer(playerid, chan->getId());
	}
	else { // Channel doesn't exist (offline)
		WorldServerAcceptPacket::playerChangeChannel(player, playerid, 0, -1);
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
			WorldServerAcceptPacket::newConnectable(chan->getId(), playerid);
			uint32_t chanIp = IpUtilities::matchIpSubnet(gamePlayer->getIp(), chan->getExternalIps(), chan->getIp());
			WorldServerAcceptPacket::playerChangeChannel(curchan->getConnection(), playerid, chanIp, chan->getPort());
		}
		else {
			WorldServerAcceptPacket::playerChangeChannel(curchan->getConnection(), playerid, 0, -1);
		}
		PlayerDataProvider::Instance()->removePendingPlayer(playerid);
	}
}

void SyncHandler::updateJob(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int16_t job = packet.get<int16_t>();
	Player *plyr = PlayerDataProvider::Instance()->getPlayer(id);
	plyr->setJob(job);
	if (plyr->getParty() != 0) {
		SyncHandler::silentUpdate(id);
	}
	if (plyr->getGuild() != 0) {
		Guild *guild = plyr->getGuild();
		GuildPacket::sendPlayerStatUpdate(guild, plyr, false);
	}
}

void SyncHandler::updateLevel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	uint8_t level = packet.get<uint8_t>();
	Player *plyr = PlayerDataProvider::Instance()->getPlayer(id);
	plyr->setLevel(level);
	if (plyr->getParty() != 0) {
		SyncHandler::silentUpdate(id);
	}
	if (plyr->getGuild() != 0) {
		Guild *guild = plyr->getGuild();
		GuildPacket::sendPlayerStatUpdate(guild, plyr, true);
	}
}

void SyncHandler::updateMap(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int32_t map = packet.get<int32_t>();
	if (Player *p = PlayerDataProvider::Instance()->getPlayer(id)) {
		p->setMap(map);
		if (p->getParty() != 0) {
			SyncHandler::silentUpdate(id);
		}
	}
}
