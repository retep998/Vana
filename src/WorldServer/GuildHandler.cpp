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
#include "Alliance.h"
#include "AllianceHandler.h"
#include "Channels.h"
#include "Database.h"
#include "GameObjects.h"
#include "Guild.h"
#include "GuildPacket.h"
#include "InterHeader.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"

void GuildHandler::handlePacket(WorldServerAcceptConnection *player, PacketReader &packet) {
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

			GuildPacket::InterServerPacket::updatePlayerRank(victim);
			GuildPacket::sendRankUpdate(guild, victim);
			}
		break;
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

			GuildPacket::InterServerPacket::updatePlayerMesos(player, -cost);
			GuildPacket::sendCapacityUpdate(guild);
			GuildPacket::InterServerPacket::updateCapacity(guild);
			}
		break;
		case 0x08: { // Disband a guild
			Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
			if (guild == 0)
				return;
			Player *player = PlayerDataProvider::Instance()->getPlayer(guild->getLeader());
			if (player == 0)
				return;
			GuildPacket::InterServerPacket::updatePlayerMesos(player, -200000);

			if (guild->getAlliance() != 0)
				AllianceHandler::removeGuild(guild);

			for (unordered_map<int32_t, Player *>::iterator iter = guild->m_players.begin(); iter != guild->m_players.end(); iter++)
				GuildPacket::InterServerPacket::removePlayer(iter->second);

			GuildPacket::sendGuildDisband(guild);
			GuildPacket::InterServerPacket::updatePlayers(guild, true);
			GuildPacket::InterServerPacket::unloadGuild(guild->getId());

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

			GuildPacket::InterServerPacket::updatePlayerMesos(player, -5000000);
			GuildPacket::sendEmblemUpdate(guild);
			GuildPacket::InterServerPacket::updateEmblem(guild);
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

			GuildPacket::InterServerPacket::updatePlayer(0, player);
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
			
			GuildPacket::InterServerPacket::updatePlayerMesos(player, -1000000);
			GuildPacket::sendEmblemUpdate(guild);
			GuildPacket::InterServerPacket::updateEmblem(guild);
			break;
		}
	}
}

void GuildHandler::handleLoginServerPacket(LoginServerConnection *player, PacketReader &packet) {
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

void GuildHandler::loadGuild(int32_t id) {
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

void GuildHandler::handleGuildCreation(PacketReader &packet) {
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

				GuildPacket::InterServerPacket::loadGuild(gid);
				loadGuild(gid);
				Guild *guild = PlayerDataProvider::Instance()->getGuild(gid);

				if (guild == 0) {
					std::cout << "\aGuildHandler::handleGuildCreation(): The code cannot load the guild. Please check if the guild was inserted into the database. Guildid: " << gid << std::endl;
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

				GuildPacket::InterServerPacket::updatePlayers(guild, false);
				GuildPacket::InterServerPacket::updatePlayerMesos(leader, -1500000);
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

void GuildHandler::sendNewPlayer(int32_t guildid, int32_t pid, bool newGuild) {
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
	
	GuildPacket::InterServerPacket::addPlayer(player);
	GuildPacket::sendPlayerUpdate(guild, player, 0);
	GuildPacket::sendGuildInfo(guild, player, newGuild);
	GuildPacket::InterServerPacket::updatePlayer(guild, player);
}

void GuildHandler::sendGuildNotice(int32_t guildid, PacketReader &packet) {
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
	if (guild == 0) 
		return;

	guild->setNotice(packet.getString());
	guild->save();
	GuildPacket::sendNoticeUpdate(guild);
}

void GuildHandler::sendDeletePlayer(int32_t guildid, int32_t pid, const string &name, bool expelled) {
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
	GuildPacket::InterServerPacket::removePlayer(player);
	GuildPacket::InterServerPacket::updatePlayer(0, player);
}

void GuildHandler::sendUpdateOfTitles(int32_t guildid, PacketReader &packet) {
	Guild *guild = PlayerDataProvider::Instance()->getGuild(guildid);
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
