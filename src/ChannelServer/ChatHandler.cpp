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
#include "ChatHandler.h"
#include "Inventory.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Mobs.h"
#include "MySQLM.h"
#include "NPCs.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "Pos.h"
#include "ReadPacket.h"
#include "Shops.h"
#include "WorldServerConnectPlayerPacket.h"

void ChatHandler::handleChat(Player *player, ReadPacket *packet) {
	string message = packet->getString();
	char bubbleOnly = packet->getByte(); // Skill Macros only display chat bubbles

	char *chat = const_cast<char *>(message.c_str()); // Leaving chat as char[] for GM commands for now
	size_t chatsize = message.size(); // See above line
	if (chat[0] == '!' && player->isGM()) {
		char *next_token;
		char command[90] = "";
		if (chatsize > 2)
			strcpy_s(command, 90, strtok_s(chat+1, " ", &next_token));
		if (strcmp(command, "lookup") == 0) {
			short type = 0;
			if (strlen(next_token) == 0) {
				PlayerPacket::showMessage(player, "Sub Commands: item, skill, map, mob, npc", 6);
				return;
			}
			char *subcommand = strtok_s(0, " ", &next_token);

			if (string(subcommand) == "item") type = 1;
			else if (string(subcommand) == "skill") type = 2;
			else if (string(subcommand) == "map") type = 3;
			else if (string(subcommand) == "mob") type = 4;
			else if (string(subcommand) == "npc") type = 5;

			if (type != 0 && strlen(next_token) == 0) {
				PlayerPacket::showMessage(player, "You must specify a search string", 5);
			}
			else if (type != 0) {
				mysqlpp::Query query = Database::getDataQuery();
				query << "SELECT objectid, name FROM stringdata WHERE type=" << type << " AND name LIKE '%" << next_token << "%'";
				mysqlpp::StoreQueryResult res = query.store();

				for (size_t i = 0; i < res.num_rows(); i++) {
					string msg = (string) res[i][0] + " : " + (string) res[i][1];
					PlayerPacket::showMessage(player, msg, 6);
				}
			}
		}
		else if (strcmp(command, "map") == 0) {
			if (strlen(next_token) == 0) {
				char msg[60];
				sprintf_s(msg, 60, "Current Map: %i", player->getMap());
				PlayerPacket::showMessage(player, msg, 6);
				return;
			}
			int mapid = -1;
			if (strcmp("town", next_token) == 0) mapid = Maps::maps[player->getMap()]->getInfo().rm;
			else if (strcmp("gm", next_token) == 0) mapid = 180000000;
			else if (strcmp("fm", next_token) == 0) mapid = 910000000;
			else if (strcmp("4th", next_token) == 0) mapid = 240010501;
			else if (strcmp("showa", next_token) == 0) mapid = 801000000;
			else if (strcmp("armory", next_token) == 0) mapid = 801040004;
			else if (strcmp("shrine", next_token) == 0) mapid = 800000000;
			else if (strcmp("mansion", next_token) == 0) mapid = 682000100;
			else if (strcmp("phantom", next_token) == 0) mapid = 682000000;
			else if (strcmp("henesys", next_token) == 0) mapid = 100000000;
			else if (strcmp("perion", next_token) == 0) mapid = 102000000;
			else if (strcmp("ellinia", next_token) == 0) mapid = 101000000;
			else if (strcmp("sleepywood", next_token) == 0) mapid = 105040300;
			else if (strcmp("lith", next_token) == 0) mapid = 104000000;
			else if (strcmp("moose", next_token) == 0) mapid = 924000001;
			else if (strcmp("kerning", next_token) == 0) mapid = 103000000;
			else if (strcmp("orbis", next_token) == 0) mapid = 200000000;
			else if (strcmp("nath", next_token) == 0) mapid = 211000000;
			else if (strcmp("ludi", next_token) == 0) mapid = 220000000;
			else if (strcmp("kft", next_token) == 0) mapid = 222000000;
			else if (strcmp("aqua", next_token) == 0) mapid = 230000000;
			else if (strcmp("omega", next_token) == 0) mapid = 221000000;
			else if (strcmp("leafre", next_token) == 0) mapid = 240000000;
			else if (strcmp("mulung", next_token) == 0) mapid = 250000000;
			else if (strcmp("herbtown", next_token) == 0) mapid = 251000000;
			else if (strcmp("ariant", next_token) == 0) mapid = 260000000;
			else if (strcmp("nlc", next_token) == 0) mapid = 600000000;
			else if (strcmp("amoria", next_token) == 0) mapid = 680000000;
			else if (strcmp("happyville", next_token) == 0) mapid = 209000000;
			// Boss maps
			else if (strcmp("ergoth", next_token) == 0) mapid = 990000900;
			else if (strcmp("pap", next_token) == 0) mapid = 220080001;
			else if (strcmp("zakum", next_token) == 0) mapid = 280030000;
			else if (strcmp("horntail", next_token) == 0) mapid = 240060200;
			else if (strcmp("lordpirate", next_token) == 0) mapid = 925100500;
			else if (strcmp("alishar", next_token) == 0) mapid = 922010900;
			else if (strcmp("papapixie", next_token) == 0) mapid = 920010800;
			else if (strcmp("kingslime", next_token) == 0) mapid = 103000804;
			else if (strcmp("pianus", next_token) == 0) mapid = 230040420;
			else if (strcmp("manon", next_token) == 0) mapid = 240020401;
			else if (strcmp("griffey", next_token) == 0) mapid = 240020101;
			else if (strcmp("jrbalrog", next_token) == 0) mapid = 105090900;
			else if (strcmp("grandpa", next_token) == 0) mapid = 801040100;
			else if (strcmp("anego", next_token) == 0) mapid = 801040003;
			else if (strcmp("tengu", next_token) == 0) mapid = 800020130;
			else { 
				char *endptr;
				mapid = strtol(next_token, &endptr, 0);
				if (strlen(endptr) != 0) mapid = -1;
			}
			if (Maps::maps.find(mapid) != Maps::maps.end())
				Maps::changeMap(player, mapid, 0);
			else
				PlayerPacket::showMessage(player, "Invalid map entered.", 5);
		}
		else if (strcmp(command, "npc") == 0) {
			int npcid = atoi(next_token);
			NPC *npc = new NPC(npcid, player);
			if (!npc->run()) {
				PlayerPacket::showMessage(player, "Invalid NPC entered.", 5);
			}
		}
		else if (strcmp(command, "addsp") == 0) {
			if (strlen(next_token) > 0) {
				int skillid = atoi(strtok_s(0, " ", &next_token));
				if (Skills::skills.find(skillid) == Skills::skills.end()) { // Don't allow skills that do not exist to be added
					PlayerPacket::showMessage(player, "Invalid Skill ID.", 5);
					return;
				}
				unsigned char count = 1;
				if (strlen(next_token) > 0)
					count = atoi(next_token);
				player->getSkills()->addSkillLevel(skillid, count);
			}
		}
		else if (strcmp(command, "summon") == 0 || strcmp(command, "spawn") == 0) {
			if (strlen(next_token) == 0) return;
			int mobid = atoi(strtok_s(0, " ", &next_token));
			if (Mobs::mobinfo.find(mobid) == Mobs::mobinfo.end()) {
				PlayerPacket::showMessage(player, "Invalid Mob ID.", 5);
				return;
			}
			int count = 1;
			if (strlen(next_token) > 0)
				count = atoi(next_token);
			for (int i = 0; i < count && i < 100; i++) {
				Mobs::spawnMob(player, mobid);
			}
		}
		else if (strcmp(command, "addnpc") == 0) {
			NPCSpawnInfo npc;
			npc.id = atoi(next_token);
			npc.fh = 0;
			npc.x = player->getPos().x;
			npc.cy = player->getPos().y;
			npc.rx0 = npc.x - 50;
			npc.rx1 = npc.x + 50;
			Maps::maps[player->getMap()]->addNPC(npc);
		}
		else if (strcmp(command, "notice") == 0) {
			if (strlen(next_token) == 0) return;
			PlayersPacket::showMessage(next_token, 0);
		}
		else if (strcmp(command, "me") == 0) {
			string msg = player->getName() + " : " + string(next_token);
			PlayersPacket::showMessage(msg, 6);
		}
		else if (strcmp(command, "maxstats") == 0) {
			player->setFame(30000);
			player->setRMHP(30000);
			player->setRMMP(30000);
			player->setMHP(30000);
			player->setMMP(30000);
			player->setStr(30000);
			player->setDex(30000);
			player->setInt(30000);
			player->setLuk(30000);
		}
		else if (strcmp(command,"str") == 0) {
			if (strlen(next_token) > 0)
				player->setStr(atoi(next_token));
		}
		else if (strcmp(command,"dex") == 0) {
			if (strlen(next_token) > 0)
				player->setDex(atoi(next_token));
		}
		else if (strcmp(command,"int") == 0) {
			if (strlen(next_token) > 0)
				player->setInt(atoi(next_token));
		}
		else if (strcmp(command,"luk") == 0) {
			if (strlen(next_token) > 0)
				player->setLuk(atoi(next_token));
		}
		else if (strcmp(command,"hp") == 0) {
			if (strlen(next_token) > 0) {
				int amount = atoi(next_token);
				player->setRMHP(amount);
				player->setMHP(amount);
				if (player->getHP() > amount)
					player->setHP(player->getMHP());
			}
		}
		else if (strcmp(command,"mp") == 0) {
			if (strlen(next_token) > 0) {
				int amount = atoi(next_token);
				player->setRMMP(amount);
				player->setMMP(amount);
				if (player->getMP() > amount)
					player->setMP(player->getMMP());
			}
		}
		else if (strcmp(command, "shop") == 0) {
			int shopid = -1;
			if (strcmp(next_token, "gear") == 0) shopid = 9999999;
			else if (strcmp(next_token, "scrolls") == 0) shopid = 9999998;
			else if (strcmp(next_token, "nx") == 0) shopid = 9999997;
			else if (strcmp(next_token, "face") == 0) shopid = 9999996;
			else if (strcmp(next_token, "ring") == 0) shopid = 9999995;
			else if (strcmp(next_token, "chair") == 0) shopid = 9999994;
			else if (strcmp(next_token, "mega") == 0) shopid = 9999993;
			else if (strcmp(next_token, "pet") == 0) shopid = 9999992;
			else shopid = atoi(next_token);

			if (Shops::shops.find(shopid) != Shops::shops.end()) {
				Shops::showShop(player, shopid);
			}
			else {
				PlayerPacket::showMessage(player, "Invalid shop. Available shops: gear, scrolls, nx, face, ring, chair, mega, pet", 6);
			}
		}
		else if (strcmp(command, "pos") == 0) {
			char text[50];
			sprintf_s(text, 50, "X: %d Y: %d", player->getPos().x, player->getPos().y);
			PlayerPacket::showMessage(player, text, 6);
		}
		else if (strcmp(command, "item") == 0) {
			if (strlen(next_token) == 0) return;
			int itemid = atoi(strtok_s(0, " ", &next_token));
			if (Inventory::items.find(itemid) == Inventory::items.end() && Inventory::equips.find(itemid) == Inventory::equips.end()) {
				PlayerPacket::showMessage(player, "Invalid item ID", 6);
				return;
			}
			int count = 1;
			if (strlen(next_token) > 0)
				count = atoi(next_token);
			Inventory::addNewItem(player, itemid, count);
		}
		else if (strcmp(command, "level") == 0) {
			if (strlen(next_token) == 0) return;
			player->setLevel(atoi(next_token));
		}
		// Jobs
		else if (strcmp(command, "job") == 0) {
			if (strlen(next_token) == 0) {
				char msg[60];
				sprintf_s(msg, 60, "Current Job: %i", player->getJob());
				PlayerPacket::showMessage(player, msg, 6);
				return;
			}

			int job = -1;
			if (strcmp(next_token, "beginner") == 0) job = 0;
			else if (strcmp(next_token, "warrior") == 0) job = 100;
			else if (strcmp(next_token, "fighter") == 0) job = 110;
			else if (strcmp(next_token, "sader") == 0) job = 111;
			else if (strcmp(next_token, "hero") == 0) job = 112;
			else if (strcmp(next_token, "page") == 0) job = 120;
			else if (strcmp(next_token, "wk") == 0) job = 121;
			else if (strcmp(next_token, "paladin") == 0) job = 122;
			else if (strcmp(next_token, "spearman") == 0) job = 130;
			else if (strcmp(next_token, "dk") == 0) job  = 131;
			else if (strcmp(next_token, "drk") == 0) job = 132;
			else if (strcmp(next_token, "mage") == 0) job = 200;
			else if (strcmp(next_token, "fpwiz") == 0) job = 210;
			else if (strcmp(next_token, "fpmage") == 0) job = 211;
			else if (strcmp(next_token, "fparch") == 0) job = 212;
			else if (strcmp(next_token, "ilwiz") == 0) job = 220;
			else if (strcmp(next_token, "ilmage") == 0) job = 221;
			else if (strcmp(next_token, "ilarch") == 0) job = 222;
			else if (strcmp(next_token, "cleric") == 0) job = 230;
			else if (strcmp(next_token, "priest") == 0) job = 231;
			else if (strcmp(next_token, "bishop") == 0) job = 232;
			else if (strcmp(next_token, "bowman") == 0) job = 300;
			else if (strcmp(next_token, "hunter") == 0) job = 310;
			else if (strcmp(next_token, "ranger") == 0) job = 311;
			else if (strcmp(next_token, "bm") == 0) job = 312;
			else if (strcmp(next_token, "xbowman") == 0) job = 320;
			else if (strcmp(next_token, "sniper") == 0) job = 321;
			else if (strcmp(next_token, "marksman") == 0) job = 322;
			else if (strcmp(next_token, "thief") == 0) job = 400;
			else if (strcmp(next_token, "sin") == 0) job = 410;
			else if (strcmp(next_token, "hermit") == 0) job = 411;
			else if (strcmp(next_token, "nl") == 0) job = 412;
			else if (strcmp(next_token, "dit") == 0) job = 420;
			else if (strcmp(next_token, "cb") == 0) job = 421;
			else if (strcmp(next_token, "shadower") == 0) job = 422;
			else if (strcmp(next_token, "pirate") == 0) job = 500;
			else if (strcmp(next_token, "infighter") == 0) job = 510;
			else if (strcmp(next_token, "buccaneer") == 0) job = 511;
			else if (strcmp(next_token, "viper") == 0) job = 512;
			else if (strcmp(next_token, "gunslinger") == 0) job = 520;
			else if (strcmp(next_token, "valkyrie") == 0) job = 521;
			else if (strcmp(next_token, "captain") == 0) job = 522;
			else if (strcmp(next_token, "gm") == 0) job = 900;
			else if (strcmp(next_token, "sgm") == 0) job = 910;
			else job = atoi(next_token);

			if (job >= 0)
				player->setJob(job);
		}
		else if (strcmp(command, "ap") == 0) {
			if (strlen(next_token) == 0) return;
			player->setAp(atoi(next_token));
		}
		else if (strcmp(command, "sp") == 0) {
			if (strlen(next_token) == 0) return;
			player->setSp(atoi(next_token));
		}
		else if (strcmp(command, "killnpc") == 0) {
			player->setNPC(0);
		}
		else if (strcmp(command, "killall") == 0) {
			Maps::maps[player->getMap()]->killMobs(player);
		}
		else if (strcmp(command, "horntail") == 0) {
			Mobs::spawnMob(player, 8810002);
			Mobs::spawnMob(player, 8810003);
			Mobs::spawnMob(player, 8810004);
			Mobs::spawnMob(player, 8810005);
			Mobs::spawnMob(player, 8810006);
			Mobs::spawnMob(player, 8810007);
			Mobs::spawnMob(player, 8810008);
			Mobs::spawnMob(player, 8810009);
		}
		else if (strcmp(command, "heal") == 0) {
			player->setHP(player->getMHP());
			player->setMP(player->getMMP());
		}
		else if (strcmp(command, "mesos") == 0) {
			if (strlen(next_token) == 0) return;
			long mesos = atoi(next_token);
			player->getInventory()->setMesos(mesos);
		}
		else if (strcmp(command, "cleardrops") == 0) {
			Maps::maps[player->getMap()]->clearDrops();
		}
		else if (strcmp(command, "save") == 0) {
			player->saveAll();
			PlayerPacket::showMessage(player, "Your progress has been saved.", 5);
		}
		else if (strcmp(command, "warp") == 0) {
			char *name = strtok_s(0, " ", &next_token);
			if (strlen(next_token) == 0) return;

			Player *warpee;
			if (warpee = Players::Instance()->getPlayer(name)) {
				int mapid = atoi(strtok_s(0, " ", &next_token));
				if (Maps::maps.find(mapid) != Maps::maps.end()) {
					Maps::changeMap(warpee, mapid, 0);
				}
			}
		}
		else if (strcmp(command, "warpto") == 0) {
			Player *warptoee;
			if (warptoee = Players::Instance()->getPlayer(next_token)) {
				Maps::changeMap(player , warptoee->getMap(), 0);
			}
		}
		else if (strcmp(command, "mwarpto") == 0) {
			Player *warpee;
			if (warpee = Players::Instance()->getPlayer(next_token)) {
				Maps::changeMap(warpee, player->getMap(), 0);
			}
		}
		else if (strcmp(command, "warpall") == 0) { // Warp everyone to MapID or your current map
			int mapid;
			if (strlen(next_token) == 0)
				mapid = player->getMap();
			else
				mapid = atoi(next_token);

			if (Maps::maps.find(mapid) == Maps::maps.end()) {
				return;
			}

			struct {
				void operator()(Player *warpee) {
					if (mapid != player->getMap() || player == warpee) {
						Maps::changeMap(warpee, mapid, 0);
					}
				}
				int mapid;
				Player *player;
			} changeMap = {mapid, player};

			Players::Instance()->run(changeMap);
		}
		else if (strcmp(command, "kill") == 0) {
			if (strcmp(next_token, "all") == 0) {
				for (size_t i = 0; i < Maps::maps[player->getMap()]->getNumPlayers(); i++) {
					Player *killpsa;
					killpsa = Maps::maps[player->getMap()]->getPlayer(i);
					if (killpsa != player) {
						killpsa->setHP(0);
					}
				}
			}
			else if (strcmp(next_token, "gm") == 0) {
				for (size_t i = 0; i < Maps::maps[player->getMap()]->getNumPlayers(); i++) {
					Player *killpsa;
					killpsa = Maps::maps[player->getMap()]->getPlayer(i);
					if (killpsa != player) {
						if (killpsa->isGM()) {	
							killpsa->setHP(0);
						}
					}
				}
			}
			else if (strcmp(next_token, "players") == 0) {
				for (size_t i = 0; i < Maps::maps[player->getMap()]->getNumPlayers(); i++) {	
					Player *killpsa;
					killpsa = Maps::maps[player->getMap()]->getPlayer(i);
					if (killpsa != player) {
						if (!killpsa->isGM()) {
							killpsa->setHP(0);
						}
					}
				}
			}
			else if (strcmp(next_token, "me") == 0) {
				player->setHP(0);
			}
			else {
				for (size_t i = 0; i < Maps::maps[player->getMap()]->getNumPlayers(); i++) {
					Player *killpsa;
					killpsa = Maps::maps[player->getMap()]->getPlayer(i);
					if (killpsa != player) {
						killpsa->setHP(0);
					}
				}
			}
		}
		else if (strcmp(command, "zakum") == 0) {
			Mobs::spawnMob(player, 8800000);
			Mobs::spawnMob(player, 8800003);
			Mobs::spawnMob(player, 8800004);
			Mobs::spawnMob(player, 8800005);
			Mobs::spawnMob(player, 8800006);
			Mobs::spawnMob(player, 8800007);
			Mobs::spawnMob(player, 8800008);
			Mobs::spawnMob(player, 8800009);
			Mobs::spawnMob(player, 8800010);
		}
		else if (strcmp(command, "header") == 0) {
			WorldServerConnectPlayerPacket::scrollingHeader(ChannelServer::Instance()->getWorldPlayer(), next_token);
		}
		else if (strcmp(command, "music") == 0) {
			Maps::changeMusic(player->getMap(), next_token);
		}
		else if	(strcmp(command, "dc") == 0)	{
			player->getPacketHandler()->disconnect();
			return;
		}
		else if (strcmp(command, "shutdown") == 0) {
			ChannelServer::Instance()->shutdown();
		}
		else if (strcmp(command, "packet") == 0) {
			PacketCreator packet;
			packet.addBytes(next_token);
			player->getPacketHandler()->send(packet);
		}
		else if (strcmp(command, "timer") == 0) {
			MapPacket::showTimer(player, atoi(next_token));
		}
		else if (strcmp(command, "instruction") == 0) {
			if (strlen(next_token) == 0) {
				PlayerPacket::showMessage(player, "No instruction entered.", 5);
				return;
			}
			for (size_t i = 0; i < Maps::maps[player->getMap()]->getNumPlayers(); i++) {
				Player *p = Maps::maps[player->getMap()]->getPlayer(i);
				PlayerPacket::instructionBubble(p, next_token);
			}
		}
		return;
	}
	PlayersPacket::showChat(player, message, bubbleOnly);
}

void ChatHandler::handleGroupChat(Player *player, ReadPacket *packet) {
	vector<int> receivers;
	char type = packet->getByte();
	unsigned char amount = packet->getByte();
	for (size_t i = 0; i < amount; i++) {
		receivers.push_back(packet->getInt());
	}
	string chat = packet->getString();

	WorldServerConnectPlayerPacket::groupChat(ChannelServer::Instance()->getWorldPlayer(), type, player->getId(), receivers, chat);
}
