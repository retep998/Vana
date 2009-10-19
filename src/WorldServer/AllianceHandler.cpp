/*
Copyright (C) 2009 Vana Development Team

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
#include "AllianceHandler.h"
#include "Alliance.h"
#include "AlliancePacket.h"
#include "Channels.h"
#include "Database.h"
#include "Guild.h"
#include "GuildPacket.h"
#include "InterHeader.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"
#include "WorldServer.h"

void AllianceHandler::handlePacket(WorldServerAcceptConnection *player, PacketReader &packet) {
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

void AllianceHandler::handleAllianceCreation(PacketReader &packet) {
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
	AlliancePacket::InterServerPacket::changeAlliance(alliance, 1);
}

void AllianceHandler::loadAlliance(int32_t allianceid) {
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

void AllianceHandler::sendAllianceInfo(int32_t playerid, int32_t allianceid) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	if (alliance == 0)
		return;
	Player * player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == 0) 
		return;
	AlliancePacket::sendAllianceInfo(alliance, player);
}

void AllianceHandler::sendChangeGuild(int32_t allianceid, PacketReader &packet) {
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

			AlliancePacket::InterServerPacket::changeGuild(alliance, guild);

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

		AlliancePacket::InterServerPacket::changeGuild(0, guild);

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

void AllianceHandler::sendNoticeUpdate(int32_t allianceid, PacketReader &packet) {
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

void AllianceHandler::sendPlayerUpdate(int32_t allianceid, int32_t playerid) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	if (alliance == 0) 
		return;
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (player == 0 || player->getAlliance() == 0 || player->getGuild() == 0) 
		return;
	
	AlliancePacket::sendUpdatePlayer(alliance, player, 1);
}

void AllianceHandler::sendTitleUpdate(int32_t allianceid, PacketReader &packet) {
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

void AllianceHandler::sendPlayerChangeRank(int32_t allianceid, PacketReader &packet) {
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
	AlliancePacket::InterServerPacket::changePlayerRank(alliance, victim);
}

void AllianceHandler::sendInvite(int32_t allianceid, PacketReader &packet) {
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

void AllianceHandler::sendAllianceDisband(int32_t allianceid, int32_t playerid) {
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
	AlliancePacket::InterServerPacket::changeAlliance(alliance, 0);

	delete alliance;
}

void AllianceHandler::sendChangeLeader(int32_t allianceid, PacketReader &packet) {
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
	AlliancePacket::InterServerPacket::changeLeader(alliance, player);
}

void AllianceHandler::sendIncreaseCapacity(int32_t allianceid, int32_t playerid) {
	Alliance *alliance = PlayerDataProvider::Instance()->getAlliance(allianceid);
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerid);

	if (alliance == 0 || player == 0 || player->getAlliance() != alliance || 
		player->getAllianceRank() != 1) 
		return;

	alliance->setCapacity(alliance->getCapacity() + 1);
	alliance->save();

	GuildPacket::InterServerPacket::updatePlayerMesos(player, -1000000);

	AlliancePacket::sendUpdateCapacity(alliance);
	AlliancePacket::InterServerPacket::changeCapacity(alliance);
}

void AllianceHandler::sendDenyPacket(PacketReader &packet) {
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

void AllianceHandler::removeGuild(Guild *guild) {
	Alliance *alliance = guild->getAlliance();
	guild->setAlliance(0);

	AlliancePacket::InterServerPacket::changeGuild(0, guild);

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
