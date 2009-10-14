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
#include "WorldServerConnectHandler.h"
#include "Alliances.h"
#include "BeautyDataProvider.h"
#include "ChannelServer.h"
#include "Connectable.h"
#include "DropDataProvider.h"
#include "Guilds.h"
#include "GuildPacket.h"
#include "ItemDataProvider.h"
#include "MapDataProvider.h"
#include "MapleSession.h"
#include "MiscUtilities.h"
#include "MobDataProvider.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "QuestDataProvider.h"
#include "Quests.h"
#include "Rates.h"
#include "ReactorDataProvider.h"
#include "ScriptDataProvider.h"
#include "ShopDataProvider.h"
#include "SkillDataProvider.h"
#include "WorldServerConnection.h"
#include <iostream>
#include <limits>

void WorldServerConnectHandler::connectLogin(WorldServerConnection *player, PacketReader &packet) {
	int8_t worldid = packet.get<int8_t>();
	if (worldid != -1) {
		ChannelServer::Instance()->setWorld(worldid);
		ChannelServer::Instance()->setWorldIp(packet.get<uint32_t>());
		ChannelServer::Instance()->setWorldPort(packet.get<int16_t>());
		std::cout << "Connecting to world " << (int16_t) worldid << std::endl;
		ChannelServer::Instance()->connectWorld();
	}
	else {
		std::cout << "Error: No world server to connect" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::connect(WorldServerConnection *player, PacketReader &packet) {
	int16_t channel = packet.get<int16_t>();
	if (channel != -1) {
		ChannelServer::Instance()->setChannel(channel);
		uint16_t port = packet.get<uint16_t>();
		vector<int8_t> bosschannels;
		int8_t chid = channel + 1;
		ChannelServer::Instance()->setPort(port);
		ChannelServer::Instance()->setMaxMultiLevel(packet.get<int8_t>());
		ChannelServer::Instance()->setMaxStats(packet.get<int16_t>());
		ChannelServer::Instance()->setMaxChars(packet.get<int32_t>());

		ChannelServer::Instance()->setPianusAttempts(packet.get<int16_t>());
		ChannelServer::Instance()->setPapAttempts(packet.get<int16_t>());
		ChannelServer::Instance()->setZakumAttempts(packet.get<int16_t>());
		ChannelServer::Instance()->setHorntailAttempts(packet.get<int16_t>());
		ChannelServer::Instance()->setPinkBeanAttempts(packet.get<int16_t>());

		bosschannels = packet.getVector<int8_t>();
		ChannelServer::Instance()->setPianusChannel(MiscUtilities::isBossChannel(bosschannels, chid));
		ChannelServer::Instance()->setPianusChannels(bosschannels);

		bosschannels = packet.getVector<int8_t>();
		ChannelServer::Instance()->setPapChannel(MiscUtilities::isBossChannel(bosschannels, chid));
		ChannelServer::Instance()->setPapChannels(bosschannels);

		bosschannels = packet.getVector<int8_t>();
		ChannelServer::Instance()->setZakumChannel(MiscUtilities::isBossChannel(bosschannels, chid));
		ChannelServer::Instance()->setZakumChannels(bosschannels);

		bosschannels = packet.getVector<int8_t>();
		ChannelServer::Instance()->setHorntailChannel(MiscUtilities::isBossChannel(bosschannels, chid));
		ChannelServer::Instance()->setHorntailChannels(bosschannels);

		bosschannels = packet.getVector<int8_t>();
		ChannelServer::Instance()->setPinkBeanChannel(MiscUtilities::isBossChannel(bosschannels, chid));
		ChannelServer::Instance()->setPinkBeanChannels(bosschannels);

		ChannelServer::Instance()->listen();
		std::cout << "Handling channel " << channel << " on port " << port << std::endl;
	}
	else {
		std::cout << "Error: No channel to handle" << std::endl;
		ChannelServer::Instance()->shutdown();
	}
}

void WorldServerConnectHandler::playerChangeChannel(WorldServerConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	uint32_t ip = packet.get<uint32_t>();
	int16_t port = packet.get<int16_t>();

	Player *ccPlayer = Players::Instance()->getPlayer(playerid);
	if (!ccPlayer) {
		return;
	}
	if (ip == 0) {
		PlayerPacket::sendBlockedMessage(ccPlayer, 0x01);
	}
	else {
		ccPlayer->setOnline(0); // Set online to 0 BEFORE CC packet is sent to player
		PlayerPacket::changeChannel(ccPlayer, ip, port);
		ccPlayer->saveAll(true);
		ccPlayer->setSaveOnDc(false);
	}
}

void WorldServerConnectHandler::findPlayer(PacketReader &packet) {
	int32_t finder = packet.get<int32_t>();
	int16_t channel = packet.get<int16_t>();
	string name = packet.getString();
	int8_t is = packet.get<int8_t>();
	if (channel == -1) {
		PlayersPacket::findPlayer(Players::Instance()->getPlayer(finder), name, -1, is);
	}
	else {
		PlayersPacket::findPlayer(Players::Instance()->getPlayer(finder), name, channel, is, 1);
	}
}

void WorldServerConnectHandler::whisperPlayer(PacketReader &packet) {
	int32_t whisperee = packet.get<int32_t>();
	string whisperer = packet.getString();
	uint16_t channel = packet.get<int16_t>();
	string message = packet.getString();

	PlayersPacket::whisperPlayer(Players::Instance()->getPlayer(whisperee), whisperer, channel, message);
}

void WorldServerConnectHandler::scrollingHeader(PacketReader &packet) {
	string message = packet.getString();
	ChannelServer::Instance()->setScrollingHeader(message);
}

void WorldServerConnectHandler::newConnectable(PacketReader &packet) {
	Connectable::Instance()->newPlayer(packet.get<int32_t>());
}

void WorldServerConnectHandler::forwardPacket(PacketReader &packet) {
	PacketCreator ppacket;
	int32_t playerid = packet.get<int32_t>();
	ppacket.addBuffer(packet);

	Player * player = Players::Instance()->getPlayer(playerid);
	if (player != 0)
		player->getSession()->send(ppacket);
}

void WorldServerConnectHandler::guildPacketHandlerWorld(PacketReader &packet) {
	switch(packet.get<int8_t>()) {
		case 0x01: Guilds::Instance()->unloadGuild(packet.get<int32_t>()); break;
		case 0x02: GuildPacket::handleEmblemChange(packet); break;
		case 0x03: // Remove player guild info
			{
			Player * player = Players::Instance()->getPlayer(packet.get<int32_t>());
			if (player == 0) 
				return;

			player->setGuildId(0);
			player->setGuildRank(5);
			}
		break;
		case 0x04: // Add guild to player
			{
			Player * player = Players::Instance()->getPlayer(packet.get<int32_t>());
			if (player == 0) 
				return;

			player->setGuildId(packet.get<int32_t>());
			player->setGuildRank(packet.get<uint8_t>());
			player->setAllianceId(packet.get<int32_t>());
			player->setAllianceRank(packet.get<uint8_t>());
			}
		break;
		case 0x05: // Update player guild and alliance rank
			{
			Player * player = Players::Instance()->getPlayer(packet.get<int32_t>());
			if (player == 0) 
				return;

			player->setGuildRank(packet.get<uint8_t>());
			player->setAllianceRank(packet.get<uint8_t>());
			}
		break;
		case 0x06: // Remove/add money from/to the player
			{
			Player * player = Players::Instance()->getPlayer(packet.get<int32_t>());
			if (player == 0) 
				return;

			Quests::giveMesos(player, packet.get<int32_t>());
			}
		break;
		case 0x07: // Change guild capacity
			{
			Guild * gi = Guilds::Instance()->getGuild(packet.get<int32_t>());
			if (gi == 0) 
				return;
			gi->capacity = packet.get<int32_t>();
			}
		break;
		case 0x08: GuildPacket::handleNameChange(packet); break;
		case 0x09: Guilds::Instance()->loadGuild(packet.get<int32_t>()); break;
		case 0x0a: {
			int32_t guilds = packet.get<int32_t>();
			int32_t id, capacity, alliance;
			int16_t logo, logobg;
			uint8_t logocolor, logobgcolor;
			string name;
			for (int32_t i = 0; i < guilds; i++) {
				id = packet.get<int32_t>();
				name = packet.getString();
				logo = packet.get<int16_t>();
				logocolor = packet.get<uint8_t>();
				logobg = packet.get<int16_t>();
				logobgcolor = packet.get<uint8_t>();
				capacity = packet.get<int32_t>();
				alliance = packet.get<int32_t>();

				Guilds::Instance()->addGuild(id, name, logo, logocolor, logobg, logobgcolor, capacity, alliance);
			}

			}
		break;
	}
}

void WorldServerConnectHandler::alliancePacketHandlerWorld(PacketReader &packet) {
	switch(packet.get<int8_t>()) {
		case 0x01: { // (un)Load Alliance and set the allianceids and ranks
			uint8_t option = packet.get<uint8_t>();
			int32_t allianceid = packet.get<int32_t>();
			if (option == 0)
				Alliances::Instance()->unloadAlliance(allianceid);
			else
				Alliances::Instance()->loadAlliance(allianceid);
			uint8_t guilds = packet.get<uint8_t>();
			for (uint8_t i = 0; i < guilds; i++) {
				Guild *guild = Guilds::Instance()->getGuild(packet.get<int32_t>());
				guild->allianceid = allianceid;

				int32_t players = packet.get<int32_t>();
				for (int32_t j = 0; j < players; j++) {
					int32_t playerid = packet.get<int32_t>();
					uint8_t rank = packet.get<uint8_t>();
					if (Player *player = Players::Instance()->getPlayer(playerid)) {
						player->setAllianceId(allianceid);
						player->setAllianceRank(rank);
					}
				}
			}
		}
		break;
		case 0x02: { // Changing the Alliance Leader
			int32_t allianceid = packet.get<int32_t>();
			Player *to = Players::Instance()->getPlayer(packet.get<int32_t>());
			Player *from = Players::Instance()->getPlayer(packet.get<int32_t>());
			if ((to != 0 && to->getAllianceId() != allianceid) || (from != 0 && from->getAllianceId() != allianceid)) 
				return;
			if (to != 0) 
				to->setAllianceRank(2);
			if (from != 0) 
				from->setAllianceRank(1);
		}
		break;
		case 0x03: { // Changing the Alliance Capacity
			if (Alliance *alliance = Alliances::Instance()->getAlliance(packet.get<int32_t>())) 
				alliance->capacity = packet.get<int32_t>();
		}
		break;
		case 0x04: { // Changing the alliance id and rank
			int32_t allianceid = packet.get<int32_t>();
			Guild *guild = Guilds::Instance()->getGuild(packet.get<int32_t>());
			guild->allianceid = allianceid;
			int32_t players = packet.get<int32_t>();
			for (int32_t i_2 = 0; i_2 < players; i_2++) {
				if (Player *player = Players::Instance()->getPlayer(packet.get<int32_t>())) {
					player->setAllianceId(allianceid);
					player->setAllianceRank(packet.get<uint8_t>());
				}
			}
		}
		break;
		case 0x05: { // Changing the rank of someone
			int32_t allianceid = packet.get<int32_t>();
			Player *victim = Players::Instance()->getPlayer(packet.get<int32_t>());
			if (victim != 0 || victim->getAllianceId() != allianceid || victim->getGuildId() == 0) 
				return;
			victim->setAllianceRank(packet.get<uint8_t>());
		}
		break;
		case 0x06: { // Channel Server Alliance Data Packet
			int32_t alliances = packet.get<int32_t>();
			int32_t id, capacity;
			string name;
			for (int32_t i = 0; i < alliances; i++) {
				id = packet.get<int32_t>();
				name = packet.getString();
				capacity = packet.get<int32_t>();
				Alliances::Instance()->addAlliance(id, name, capacity);
			}
		}
		break;
	}
}

void WorldServerConnectHandler::setRates(PacketReader &packet) {
	int32_t ratesSetBit = packet.get<int32_t>();
	if (ratesSetBit & Rates::SetBits::exp) {
		ChannelServer::Instance()->setExprate(packet.get<int32_t>());
	}
	if (ratesSetBit & Rates::SetBits::questExp) {
		ChannelServer::Instance()->setQuestExprate(packet.get<int32_t>());
	}
	if (ratesSetBit & Rates::SetBits::meso) {
		ChannelServer::Instance()->setMesorate(packet.get<int32_t>());
	}
	if (ratesSetBit & Rates::SetBits::drop) {
		ChannelServer::Instance()->setDroprate(packet.get<int32_t>());
	}
}

void WorldServerConnectHandler::reloadMcdb(PacketReader &packet) {
	string args = packet.getString();
	if (args == "all") {
		ItemDataProvider::Instance()->loadData();
		DropDataProvider::Instance()->loadData();
		ShopDataProvider::Instance()->loadData();
		MobDataProvider::Instance()->loadData();
		BeautyDataProvider::Instance()->loadData();
		ScriptDataProvider::Instance()->loadData();
		SkillDataProvider::Instance()->loadData();
		ReactorDataProvider::Instance()->loadData();
		QuestDataProvider::Instance()->loadData();
	}
	else if (args == "items") ItemDataProvider::Instance()->loadData();
	else if (args == "drops") DropDataProvider::Instance()->loadData();
	else if (args == "shops") ShopDataProvider::Instance()->loadData();
	else if (args == "mobs") MobDataProvider::Instance()->loadData();
	else if (args == "beauty") BeautyDataProvider::Instance()->loadData();
	else if (args == "scripts") ScriptDataProvider::Instance()->loadData();
	else if (args == "skills") SkillDataProvider::Instance()->loadData();
	else if (args == "reactors") ReactorDataProvider::Instance()->loadData();
	else if (args == "quest") QuestDataProvider::Instance()->loadData();
}