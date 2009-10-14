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
#include "AlliancePacket.h"
#include "Alliances.h"
#include "Channels.h"
#include "Database.h"
#include "GuildPacket.h"
#include "Guilds.h"
#include "PacketCreator.h"
#include "Parties.h"
#include "PartyHandler.h"
#include "Players.h"
#include "SendHeader.h"
#include "InterHeader.h"
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
	Player *player = Players::Instance()->getPlayer(playerid);
	if (player == 0 || player->party == 0 || player->guildid == 0 || 
		player->allianceid != 0 || player->guildrank != 1) 
		return;

	Party *party = Parties::Instance()->getParty(player->party);
	if (party->members.size() != 2) 
		return;

	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++)
		if (iter->second->allianceid != 0 || iter->second->guildid == 0)
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
		std::cout << "\a Warning! " << player->name << " has created " << alliancename << " but it failed! " << query.error() << std::endl;
		return;
	}

	int32_t allianceid = static_cast<int32_t>(query.insert_id());
	
	loadAlliance(allianceid);
	Alliance *alliance = Alliances::Instance()->getAlliance(allianceid);

	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		Guild *playerGuild = Guilds::Instance()->getGuild(iter->second->guildid);
		if (playerGuild != 0) {
			for (unordered_map<int32_t, Player *>::iterator iter2 = playerGuild->m_players.begin(); iter2 != playerGuild->m_players.end(); iter2++) {
				if (iter2->second->allianceid != 0 || iter2->second->guildid == 0) 
					continue;
				iter2->second->allianceid = allianceid;
				if (party->getLeader() == iter2->second->id) // The party/alliance leader
					iter2->second->alliancerank = 1;
				else if (iter->second->id == iter2->second->id) // The other member in the party
					iter2->second->alliancerank = 2;
				else // The other members
					iter2->second->alliancerank = 3;
				query << "UPDATE characters SET alliance = " << allianceid << ", "
					<< "alliancerank = " <<	static_cast<int16_t>(iter2->second->alliancerank) << " WHERE id = " << iter2->second->id << " LIMIT 1";
				query.exec();
			}
			playerGuild->setAllianceId(allianceid);
			query << "UPDATE guilds SET alliance = " << allianceid << " WHERE id = " << iter->second->guildid << " LIMIT 1";
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
		std::cout << "Can't load alliance! allianceid " << allianceid << std::endl;
		return;
	}

	for (int32_t i = 0; i < (int32_t) res.num_rows(); i++) {
		Alliances::Instance()->addAlliance(res[i]["id"], 
			(string) res[i]["name"], 
			(string) res[i]["notice"],
			(string) res[i]["rank1title"], 
			(string) res[i]["rank2title"], 
			(string) res[i]["rank3title"], 
			(string) res[i]["rank4title"], 
			(string) res[i]["rank5title"],
			res[i]["capacity"],
			res[i]["leader"]);
	}
}

void AllianceHandler::sendAllianceInfo(int32_t playerid, int32_t allianceid) {
	Alliance * alliance = Alliances::Instance()->getAlliance(allianceid);
	if (alliance == 0)
		return;
	Player * player = Players::Instance()->getPlayer(playerid);
	if (player == 0) 
		return;
	AlliancePacket::sendAllianceInfo(alliance, player);
}

void AllianceHandler::sendChangeGuild(int32_t allianceid, PacketReader &packet) {
	int32_t guildid = packet.get<int32_t>();
	int32_t playerid = packet.get<int32_t>();
	uint8_t option = packet.get<uint8_t>();
	Alliance * alliance;

	if (option != 0)
		alliance = Alliances::Instance()->getAlliance(allianceid);
	else {
		Guild *otherGuild = Guilds::Instance()->getGuild(allianceid);

		alliance = Alliances::Instance()->getAlliance(otherGuild->getAllianceId());
		allianceid = alliance->getId();
	}

	Guild *guild = Guilds::Instance()->getGuild(guildid);
	Player *player = Players::Instance()->getPlayer(playerid);

	if (alliance == 0 || (guild == 0 && option == 0) || player == 0 || (option != 0 && player->allianceid != allianceid)) 
		return;

	if (option == 0) {
		if (guild->getInvited() && guild->getInvitedId() == alliance->getId()) {
			alliance->addGuild(guild); // add the guild before sending the packet
			
			guild->removeInvite();
			guild->setAllianceId(allianceid);

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
				if (iter->second->guildrank == 1)
					iter->second->alliancerank = 2;
				else
					iter->second->alliancerank = (int8_t) lowestAllianceRank;
				iter->second->allianceid = allianceid;
			}

			AlliancePacket::sendInviteAccepted(alliance, guild);
		}
		else
			return;
	}
	else {
		guild->setAllianceId(0);

		AlliancePacket::InterServerPacket::changeGuild(0, guild);

		for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
			iter->second->alliancerank = 5;
			iter->second->allianceid = 0;
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
	Alliance *alliance = Alliances::Instance()->getAlliance(allianceid);
	if (alliance == 0) 
		return;
	Player *player = Players::Instance()->getPlayer(packet.get<int32_t>());
	if (player == 0 || player->alliancerank > 2 || player->guildid == 0) 
		return;

	alliance->setNotice(packet.getString());
	alliance->save();

	AlliancePacket::sendUpdateNotice(alliance);
}

void AllianceHandler::sendPlayerUpdate(int32_t allianceid, int32_t playerid) {
	Alliance *alliance = Alliances::Instance()->getAlliance(allianceid);
	if (alliance == 0) 
		return;
	Player *player = Players::Instance()->getPlayer(playerid);
	if (player == 0 || player->allianceid == 0 || player->guildid == 0) 
		return;
	
	AlliancePacket::sendUpdatePlayer(alliance, player, 1);
}

void AllianceHandler::sendTitleUpdate(int32_t allianceid, PacketReader &packet) {
	Alliance *alliance = Alliances::Instance()->getAlliance(allianceid);
	if (alliance == 0) 
		return;
	Player *player = Players::Instance()->getPlayer(packet.get<int32_t>());
	if (player == 0 || player->alliancerank > 1 || player->guildid == 0) 
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

	Alliance *alliance = Alliances::Instance()->getAlliance(allianceid);
	Player *changer = Players::Instance()->getPlayer(changerid);
	Player *victim = Players::Instance()->getPlayer(victimid, true);

	if (alliance == 0 || changer == 0 || victim == 0) 
		return;

	if (changer->alliancerank > 2 || changer->guildid == 0 || changer->allianceid == 0 || 
		changer->allianceid != allianceid) 
		return;
	if (victim->alliancerank == 1 || victim->guildid == 0 || victim->allianceid == 0 || 
		victim->allianceid != allianceid) 
		return;

	// Client sends a zero for increasing the rank, and an one for decreasing the rank....
	uint8_t option = packet.get<uint8_t>();

	if (option == 1 && victim->alliancerank > 2)
		victim->alliancerank--;
	else if (option == 0 && victim->alliancerank > 2 && victim->alliancerank < 5)
		victim->alliancerank++;
	else
		return;

	AlliancePacket::sendUpdatePlayer(alliance, victim, 0);
	AlliancePacket::InterServerPacket::changePlayerRank(alliance, victim);
}

void AllianceHandler::sendInvite(int32_t allianceid, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	string guildname = packet.getString();

	Alliance *alliance = Alliances::Instance()->getAlliance(allianceid);
	Guild *guild = Guilds::Instance()->getGuild(guildname);
	Player *inviter = Players::Instance()->getPlayer(playerid);

	if (alliance == 0 || guild == 0 || guild->getLeader() == inviter->id) 
		return;

	Player *invitee = Players::Instance()->getPlayer(guild->getLeader());
	if (invitee == 0 || inviter == 0 || inviter->alliancerank > 1 || 
		inviter->guildid == 0 || inviter->allianceid == 0 || invitee->guildid == 0) 
		return;

	if (!guild->getInvited()) {
		guild->setInvite(allianceid);
		AlliancePacket::sendInvite(alliance, inviter, invitee);
	}
}

void AllianceHandler::sendAllianceDisband(int32_t allianceid, int32_t playerid) {
	Alliance *alliance = Alliances::Instance()->getAlliance(allianceid);
	Player *player = Players::Instance()->getPlayer(playerid);

	if (alliance == 0 || player == 0 || player->allianceid == 0 || 
		player->guildid == 0 || player->alliancerank != 1 || 
		player->allianceid != allianceid || alliance->getLeaderId() != playerid) 
		return;

	// Todo: update the characters and guilds so the alliance gets deleted!

	unordered_map<int32_t, Guild *> guilds = alliance->getGuilds();
	unordered_map<int32_t, Guild *>::iterator iter;
	unordered_map<int32_t, Player *>::iterator iter2;

	for (iter = guilds.begin(); iter != guilds.end(); iter++) {
		for (iter2 = iter->second->m_players.begin(); iter2 != iter->second->m_players.end(); iter2++) {
			iter2->second->allianceid = 0;
			iter2->second->alliancerank = 5;
		}
		iter->second->setAllianceId(0);
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
	Alliance *alliance = Alliances::Instance()->getAlliance(allianceid);
	Player *player = Players::Instance()->getPlayer(packet.get<int32_t>());
	Player *victim = Players::Instance()->getPlayer(packet.get<int32_t>());

	if (alliance == 0 || player == 0 || victim == 0 || 
		player->allianceid != allianceid || victim->allianceid != allianceid || 
		player->alliancerank != 1 || victim->alliancerank != 2) 
		return;

	player->alliancerank = 2;
	victim->alliancerank = 1;

	alliance->setLeaderId(victim->id);
	alliance->save();

	AlliancePacket::sendUpdateLeader(alliance, player);
	AlliancePacket::InterServerPacket::changeLeader(alliance, player);
}

void AllianceHandler::sendIncreaseCapacity(int32_t allianceid, int32_t playerid) {
	Alliance *alliance = Alliances::Instance()->getAlliance(allianceid);
	Player *player = Players::Instance()->getPlayer(playerid);

	if (alliance == 0 || player == 0 || player->allianceid != allianceid || 
		player->alliancerank != 1) 
		return;

	alliance->setCapacity(alliance->getCapacity() + 1);
	alliance->save();

	GuildPacket::InterServerPacket::updatePlayerMesos(player, -1000000);

	AlliancePacket::sendUpdateCapacity(alliance);
	AlliancePacket::InterServerPacket::changeCapacity(alliance);
}

void AllianceHandler::sendDenyPacket(PacketReader &packet) {
	Player *denier = Players::Instance()->getPlayer(packet.get<int32_t>());
	if (denier == 0 || denier->guildid == 0 || denier->guildrank != 1) 
		return;

	Guild *dguild = Guilds::Instance()->getGuild(denier->guildid);
	if (!dguild->getInvited()) 
		return;

	Alliance * alliance = Alliances::Instance()->getAlliance(dguild->getInvitedId());
	if (alliance == 0)
		return;

	AlliancePacket::sendInviteDenied(alliance, dguild);
}

void AllianceHandler::removeGuild(Guild *guild) {
	Alliance *alliance = Alliances::Instance()->getAlliance(guild->getAllianceId());
	guild->setAllianceId(0);

	AlliancePacket::InterServerPacket::changeGuild(0, guild);

	for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++) {
		iter->second->allianceid = 0;
		iter->second->alliancerank = 5;
	}

	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET alliance = 0, alliancerank = 5 WHERE guild = " << guild->getId();
	query.exec();

	query << "UPDATE guilds SET alliance = 0 WHERE id = " << guild->getId(); // Update the guild in the database
	query.exec();

	alliance->removeGuild(guild);
	AlliancePacket::sendGuildLeft(alliance, guild, false);
}
