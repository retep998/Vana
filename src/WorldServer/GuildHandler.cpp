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
#include "GuildHandler.h"
#include "AllianceHandler.h"
#include "Channels.h"
#include "Database.h"
#include "Guild.h"
#include "GuildPacket.h"
#include "Guilds.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Parties.h"
#include "PartyHandler.h"
#include "Players.h"
#include "SendHeader.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"

void GuildHandler::handlePacket(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t option = packet.get<int8_t>();
	int32_t guildid = packet.get<int32_t>();
	switch(option) {
		case 0x01: sendGuildInvite(guildid, packet); break; // Invite a player
		case 0x02: { // Expell a player or leave
			Player *player = Players::Instance()->getPlayer(packet.get<int32_t>(), true);
			if (player == 0)
				return;
			string name = packet.getString();
			sendDeletePlayer(guildid, player->id, name, (packet.get<int8_t>() == 1 ? true : false));
			}
		break;
		case 0x03: { // Accept invite
			int32_t playerid = packet.get<int32_t>();
			Player *player = Players::Instance()->getPlayer(playerid);
			if (player == 0)
				return;
			int32_t inviteGuildId = player->invite_guild;
			if (!player->invited || difftime(player->invite_time, time(0)) >= 300) {
				if (difftime(player->invite_time, time(0)) >= 300) {
					player->invited = false;
					player->invite_time = 0;
					player->invite_guild = 0;
				}
				return;
			}
			Guild *guild = Guilds::Instance()->getGuild(inviteGuildId);
			if (guild == 0) 
				return;
			player->invited = false;
			player->invite_guild = 0;
			player->invite_time = 0;
			player->guildid = guild->getId();
			player->guildrank = guild->getLowestRank();
			player->allianceid = guild->getAllianceId();
			player->alliancerank = guild->getAllianceId() != 0 ? Alliances::Instance()->getAlliance(guild->getAllianceId())->getLowestRank() : 0;
			sendNewPlayer(inviteGuildId, playerid, false);
			}
		break;
		case 0x04: sendUpdateOfTitles(guildid, packet); break; // Change the title's
		case 0x05: sendGuildNotice(guildid, packet); break; // Change the notice
		case 0x06: { // Change someones rank
			int32_t playerid = packet.get<int32_t>();
			int32_t victimid = packet.get<int32_t>();
			uint8_t rank = packet.get<uint8_t>();
			Guild *guild = Guilds::Instance()->getGuild(guildid);
			if (guild == 0) 
				return;
			Player *player = Players::Instance()->getPlayer(playerid); // don't allow offline players..
			Player *victim = Players::Instance()->getPlayer(victimid, true);

			if (player == 0 || victim == 0)
				return;

			if (player->guildid != guildid || victim->guildid != guildid)
				return;
			if (victim->guildrank == 1)
				return;
			if (rank <= 2 && player->guildrank >= 2)
				return;
			if (rank <= 1 || rank > guild->getLowestRank())
				return;

			victim->guildrank = rank;

			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE characters SET guildrank = " <<  static_cast<int16_t>(rank) << " WHERE id = " << victimid << " LIMIT 1";
			query.exec();

			GuildPacket::InterServerPacket::updatePlayerRank(victim);
			GuildPacket::sendRankUpdate(guild, victim);
			}
		break;
		case 0x07: { // Add guild capacity
			Guild *guild = Guilds::Instance()->getGuild(guildid);
			if (guild == 0) 
				return;
			int32_t playerid = packet.get<int32_t>();
			Player *player = Players::Instance()->getPlayer(playerid);
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

			GuildPacket::InterServerPacket::updatePlayerMesos(player, -cost);
			GuildPacket::sendCapacityUpdate(guild);
			GuildPacket::InterServerPacket::updateCapacity(guild);
			}
		break;
		case 0x08: { // Disband a guild
			Guild *guild = Guilds::Instance()->getGuild(guildid);
			if (guild == 0)
				return;
			Player *player = Players::Instance()->getPlayer(guild->getLeader());
			if (player == 0)
				return;
			GuildPacket::InterServerPacket::updatePlayerMesos(player, -200000);

			if (guild->getAllianceId() != 0)
				AllianceHandler::removeGuild(guild);

			for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++)
				GuildPacket::InterServerPacket::removePlayer(iter->second);

			GuildPacket::sendGuildDisband(guild);
			GuildPacket::InterServerPacket::updatePlayers(guild, true);

			Guilds::Instance()->removeGuild(guild);

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
			Player *invitee = Players::Instance()->getPlayerFromName(packet.getString());
			Player *inviter = Players::Instance()->getPlayerFromName(packet.getString());
			if (invitee == 0 || inviter == 0 || !invitee->invited) 
				return;

			invitee->invite_guild = 0;
			invitee->invite_time = 0;
			invitee->invited = false;

			GuildPacket::sendGuildDenyResult(inviter, invitee, result);

			}
		break;
		case 0x0a: { // Add or remove guildpoints
			Guild *guild = Guilds::Instance()->getGuild(guildid);
			if (guild == 0) 
				return;
			guild->setGuildPoints(guild->getGuildPoints() + packet.get<int32_t>());
			guild->save();

			GuildPacket::sendGuildPointsUpdate(guild);

			}
		break;
		case 0x0b: { // Change Emblem
			Guild *guild = Guilds::Instance()->getGuild(guildid);
			if (guild == 0) 
				return;
			int32_t playerid = packet.get<int32_t>();
			Player *player = Players::Instance()->getPlayer(playerid);
			if (player == 0 || player->guildrank != 1 || player->guildid == 0) 
				return;

			guild->setLogoBg(packet.get<int16_t>());
			guild->setLogoBgColor(packet.get<uint8_t>());
			guild->setLogo(packet.get<int16_t>());
			guild->setLogoColor(packet.get<uint8_t>());

			guild->save();

			GuildPacket::InterServerPacket::updatePlayerMesos(player, -5000000);
			GuildPacket::sendEmblemUpdate(guild);
			GuildPacket::InterServerPacket::updateEmblem(guild);
			}
		break;
		case 0x0c: { // Send guild rank board
			int32_t playerid = packet.get<int32_t>();
			Player *player = Players::Instance()->getPlayer(playerid);
			if (player == 0)
				return;
			GuildPacket::sendGuildRankBoard(player, packet.get<int32_t>());
			}
			break;
		case 0x0d: handleGuildCreation(packet); break; // Guild Contract Check/Creation
		case 0x0e: { // Remove a character from the guild (Loginserver -> Worldserver packet)
			Player *player = Players::Instance()->getPlayer(packet.get<int32_t>(), true);
			if (player == 0)
				return;
			Guild *guild = Guilds::Instance()->getGuild(player->guildid);
			if (guild == 0)
				return;

			GuildPacket::InterServerPacket::updatePlayer(0, player);
			GuildPacket::sendPlayerUpdate(guild, player, 1);
			}
		case 0x0f: { // Remove emblem
			int32_t playerid = packet.get<int32_t>();
			Player *player = Players::Instance()->getPlayer(playerid);
			if (player == 0 || player->guildrank != 1) 
				return;
			Guild *guild = Guilds::Instance()->getGuild(guildid);
			if (guild == 0 || guild->getId() != player->guildid) 
				return;
			guild->setLogoBg(0);
			guild->setLogoBgColor(0);
			guild->setLogo(0);
			guild->setLogoColor(0);
			guild->save();
			
			GuildPacket::InterServerPacket::updatePlayerMesos(player, -1000000);
			GuildPacket::sendEmblemUpdate(guild);
			GuildPacket::InterServerPacket::updateEmblem(guild);
			}
		break;
	}
}

void GuildHandler::handleLoginServerPacket(LoginServerConnection *player, PacketReader &packet) {
	int32_t charid = packet.get<int32_t>();
	Player *character = Players::Instance()->getPlayer(charid);
	if (character == 0)
		return;
	Guild *guild = Guilds::Instance()->getGuild(character->guildid);
	if (guild == 0)
		return;
	
	guild->removePlayer(character);

	GuildPacket::sendPlayerUpdate(guild, character, 1);
}

void GuildHandler::loadGuild(int32_t id) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM guilds WHERE id = " << id << " LIMIT 1";
	mysqlpp::StoreQueryResult res = query.store();

	if ((int32_t) res.num_rows() == 0) {
		std::cout << "\aAlert! Can't load a guild! Guildid: " << id << " >_>" << std::endl;
		return;
	}
	
	Guilds::Instance()->createGuild((string) res[0]["name"],
		(string) res[0]["notice"],
		res[0]["id"], 
		res[0]["leaderid"], 
		res[0]["capacity"], 
		static_cast<int16_t>(res[0]["logo"]), 
		static_cast<uint8_t>(res[0]["logocolor"]), 
		static_cast<int16_t>(res[0]["logobg"]), 
		static_cast<uint8_t>(res[0]["logobgcolor"]), 
		res[0]["gp"], 
		(string) res[0]["rank1title"], 
		(string) res[0]["rank2title"], 
		(string) res[0]["rank3title"], 
		(string) res[0]["rank4title"], 
		(string) res[0]["rank5title"],
		res[0]["alliance"]);
}

void GuildHandler::handleGuildCreation(PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	int8_t option = packet.get<int8_t>();

	Player *player = Players::Instance()->getPlayer(playerid);
	if (player == 0 || player->party == 0 || player->guildid != 0) 
		return;
	Party *party = Parties::Instance()->getParty(player->party);
	if (party->members.size() < 2) 
		return;

	if (option == 1) { // Check the guildname and send the contract around
		if (!party->isLeader(playerid)) 
			return;
		string guildname = packet.getString();
		if (player->level <= 10) {
			GuildPacket::sendPlayerGuildMessage(player, 0x23);
			return;
		}
		if (Guilds::Instance()->getGuild(guildname) != 0) {
			GuildPacket::sendPlayerGuildMessage(player, 0x1c);
		}
		else {
			party->guildname = guildname;
			for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++)
				GuildPacket::sendGuildContract(iter->second, party->isLeader(iter->second->id), iter->second->party, player->name, guildname);
		}
	}
	else if (option == 2) {
		int32_t partyid = packet.get<int32_t>();
		if (player->party != partyid)
			return;
		int8_t result = packet.get<int8_t>();
		party->guildcontract += result;
		party->voters++;
		if (party->voters == 1) {
			if (party->guildcontract == 1) {
				Player *leader = Players::Instance()->getPlayer(party->getLeader());
				mysqlpp::Query query = Database::getCharDB().query();
				query << "INSERT INTO guilds (`world`, `name`, `leaderid`, `notice`, `rank4title`, `rank5title`, `capacity`) VALUES ("
					<< static_cast<int16_t>(WorldServer::Instance()->getWorldId()) << ", "
					<< mysqlpp::quote << party->guildname << ", "
					<< party->getLeader() << ", "
					<< mysqlpp::quote << "" << ", "
					<< mysqlpp::quote << "" << ", "
					<< mysqlpp::quote << "" << ", "
					<< "10)";

				if (!query.exec()) {
					std::cout << "/a[ALERT] The server cant create a guild! MySQL error: " << query.error() << std::endl;
					Player *leader = Players::Instance()->getPlayer(party->getLeader());
					GuildPacket::sendPlayerMessage(leader, 1, "Sorry, but something went wrong on the server. You didn't lose money and there was no guild created.");
					party->guildcontract = 0;
					party->guildname = "";
					party->voters = 0;
					return;
				}

				int32_t gid = static_cast<int32_t>(query.insert_id());

				if (gid == 0) {
					std::cout << "/a[ALERT] The server cant load a guild! MySQL error: " << query.error() << std::endl;
					GuildPacket::sendPlayerMessage(leader, 1, "Sorry, but something went wrong on the server. You didn't lose money and there was no guild created.");
					party->guildcontract = 0;
					party->guildname = "";
					party->voters = 0;
					return;
				}

				GuildPacket::InterServerPacket::loadGuild(gid);
				loadGuild(gid);
				Guild *guild = Guilds::Instance()->getGuild(gid);

				if (guild == 0) {
					std::cout << "\aGuildHandler::handleGuildCreation(): The code cannot load the guild. Please check if the guild was inserted into the database. Guildid: " << gid << std::endl;
					return;
				}

				for (map<int32_t, Player*>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
					iter->second->guildid = gid;
					iter->second->guildrank = party->isLeader(iter->second->id) ? 1 : guild->getLowestRank();
					query << "UPDATE characters SET guild = " << gid << ", guildrank = " << static_cast<int16_t>(iter->second->guildrank) << " WHERE id = " << iter->second->id << " LIMIT 1;";
					query.exec();
					guild->addPlayer(iter->second);
				}

				for (map<int32_t, Player*>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
					GuildPacket::sendGuildInfo(guild, iter->second, true);
				}

				GuildPacket::InterServerPacket::updatePlayers(guild, false);
				GuildPacket::InterServerPacket::updatePlayerMesos(leader, -1500000);
			}
			else {
				// Sorry guys, i need this packet...
				Player *leader = Players::Instance()->getPlayer(party->getLeader());
				GuildPacket::sendPlayerMessage(leader, 1, "I'm sorry but not all the partymembers have accepted the contract. Try again.");
			}
			party->guildname = "";
			party->guildcontract = 0;
			party->voters = 0;
		}
	}
}

void GuildHandler::sendNewPlayer(int32_t guildid, int32_t pid, bool newGuild) {
	Guild *guild = Guilds::Instance()->getGuild(guildid);
	if (guild == 0) 
		return;
	Player *player = Players::Instance()->getPlayer(pid, true);
	
	guild->addPlayer(player);

	mysqlpp::Query update = Database::getCharDB().query();
	update << "UPDATE characters SET " 
		<< "guild = " << guild->getId() << ", "
		<< "guildrank = " <<  static_cast<int16_t>(player->guildrank) << ", "
		<< "alliance = " << guild->getAllianceId() << ", "
		<< "alliancerank = " <<  static_cast<int16_t>(player->alliancerank)
		<< " WHERE ID = " << pid;
	update.exec();
	
	GuildPacket::InterServerPacket::addPlayer(player);
	GuildPacket::sendPlayerUpdate(guild, player, 0);
	GuildPacket::sendGuildInfo(guild, player, newGuild);
	GuildPacket::InterServerPacket::updatePlayer(guild, player);
}

void GuildHandler::sendGuildNotice(int32_t guildid, PacketReader &packet) {
	Guild *guild = Guilds::Instance()->getGuild(guildid);
	if (guild == 0) 
		return;

	guild->setNotice(packet.getString());
	guild->save();
	GuildPacket::sendNoticeUpdate(guild);
}

void GuildHandler::sendDeletePlayer(int32_t guildid, int32_t pid, const string &name, bool expelled) {
	Guild *guild = Guilds::Instance()->getGuild(guildid);
	if (guild == 0) 
		return;

	Player *player = Players::Instance()->getPlayer(pid, true);
	if (player == 0)
		return;

	player->guildid = 0;
	player->guildrank = 0;
	player->allianceid = 0;
	player->alliancerank = 0;

	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE characters SET guild = 0, guildrank = 5, alliance = 0, alliancerank = 5 WHERE id = " << pid << " LIMIT 1";
	query.exec();

	GuildPacket::sendPlayerUpdate(guild, player, (expelled ? 2 : 1));
	guild->removePlayer(player);
	GuildPacket::InterServerPacket::removePlayer(player);
	GuildPacket::InterServerPacket::updatePlayer(0, player);
}

void GuildHandler::sendUpdateOfTitles(int32_t guildid, PacketReader &packet) {
	Guild *guild = Guilds::Instance()->getGuild(guildid);
	if (guild == 0) 
		return;
	
	for (uint8_t i = 1; i <= 5; i++)
		guild->setTitle(i, packet.getString());

	guild->save();
	GuildPacket::sendTitlesUpdate(guild);	
}

void GuildHandler::sendGuildInvite(int32_t guildid, PacketReader &packet) {
	int32_t inviterid = packet.get<int32_t>();
	string name = packet.getString();
	Player *inviter = Players::Instance()->getPlayer(inviterid);
	Player *invitee = Players::Instance()->getPlayerFromName(name);
	if (inviter == 0 || invitee == 0 || inviter->guildid != guildid) 
		return;

	if (difftime(invitee->invite_time, time(0)) >= 300) {
		invitee->invited = false;
		invitee->invite_time = 0;
		invitee->invite_guild = 0;
	}

	if (invitee->channel != inviter->channel) {
		GuildPacket::sendPlayerGuildMessage(inviter, 0x2a);
		return;
	}
	if (invitee->guildid != 0) {
		GuildPacket::sendPlayerGuildMessage(inviter, 0x28);
		return;
	}

	if (!invitee->invited) {
		invitee->invited = true;
		invitee->invite_time = time(0);
		invitee->invite_guild = guildid;
	}

	GuildPacket::sendInvite(inviter, invitee);
}
