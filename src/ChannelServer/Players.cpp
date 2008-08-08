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
#include "Players.h"
#include "ChannelServer.h"
#include "Drops.h"
#include "Inventory.h"
#include "Levels.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Mobs.h"
#include "MySQLM.h"
#include "NPCs.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "ReadPacket.h"
#include "Shops.h"
#include "SkillsPacket.h"
#include "StringUtilities.h"
#include "WorldServerConnectPlayerPacket.h"
#include "Randomizer.h"

hash_map <int, Player*> Players::players;

void Players::addPlayer(Player *player) {
	players[player->getPlayerid()] = player;
}

void Players::deletePlayer(Player *player) {
	players.erase(player->getPlayerid());
}

void Players::handleMoving(Player *player, ReadPacket *packet) {
	packet->reset(-12);
	char type = packet->getByte();

	packet->reset(-4);
	short x = packet->getShort();
	short y = packet->getShort() - 1;

	player->setPos(Pos(x, y));
	player->setType(type);

	packet->reset(7);
	PlayersPacket::showMoving(player, packet->getBuffer(), packet->getBufferLength());
}

void Players::faceExperiment(Player *player, ReadPacket *packet) {
	int face = packet->getInt();
	PlayersPacket::faceExperiment(player, face);
}

void Players::chatHandler(Player *player, ReadPacket *packet) {
	string message = packet->getString();
	char bubbleOnly = packet->getByte(); // Skill Macros only display chat bubbles

	char *chat = const_cast<char *>(message.c_str()); // Leaving chat as char[] for GM commands for now
	size_t chatsize = message.size(); // See above line
	if (chat[0] == '!') {
		if (!player->isGM()) return;
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
			else shopid = atoi(next_token);

			if (Shops::shops.find(shopid) != Shops::shops.end()) {
				Shops::showShop(player, shopid);
			}
			else {
				PlayerPacket::showMessage(player, "Invalid shop. Available shops: gear, scrolls, nx, face, ring, chair, mega", 6);
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
			if (strlen(name) > 0)
				for (hash_map <int, Player*>::iterator iter = Players::players.begin(); iter != Players::players.end(); iter++)
					if (iter->second->getName() == string(name))
						if (strlen(next_token) > 0) {
							int mapid = atoi(strtok_s(0, " ", &next_token));
							if (Maps::maps.find(mapid) != Maps::maps.end()) {
								Maps::changeMap(iter->second, mapid, 0);
								break;
							}
						}
		}
		else if (strcmp(command, "warpto") == 0) {
			if (strlen(next_token) > 0)
				for (hash_map <int, Player*>::iterator iter = Players::players.begin(); iter != Players::players.end(); iter++)
					if (iter->second->getName() == string(next_token))
						Maps::changeMap(player , iter->second->getMap(), 0);
		}
		else if (strcmp(command, "mwarpto") == 0) {
			if (strlen(next_token) > 0)
				for (hash_map <int, Player*>::iterator iter = Players::players.begin(); iter != Players::players.end(); iter++)
					if (iter->second->getName() == string(next_token)) {
						Maps::changeMap(iter->second, player->getMap(), 0);
						break;
					}
		}
		else if (strcmp(command, "warpall") == 0) { // Warp everyone to MapID or your current map
			int mapid = 0;
			if (strlen(next_token) == 0)
				mapid = player->getMap();
			else
				mapid = atoi(next_token);

			for (hash_map <int, Player*>::iterator iter = Players::players.begin(); iter != Players::players.end(); iter++) {
				if (Maps::maps.find(mapid) != Maps::maps.end()) {
					if (mapid == player->getMap()) {
						if (player->getName() == iter->second->getName())
							Maps::changeMap(iter->second, mapid, 0);
					}
					else
						Maps::changeMap(iter->second, mapid, 0);
				}
			}
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
			packet.send(player);
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

void Players::damagePlayer(Player *player, ReadPacket *packet) {
	packet->skipBytes(4); // Ticks
	unsigned char type = packet->getByte();
	unsigned char hit = 0;
	packet->skipBytes(1); // Element - 0x00 = elementless, 0x01 = ice, 0x02 = fire, 0x03 = lightning
	int damage = packet->getInt();
	int mobid = 0; // Actual Mob ID - i.e. 8800000 for Zak
	int mapmobid = 0; // Map Mob ID
 	int fake = 0;
	short job = player->getJob();
	bool applieddamage = false;
 	PGMRInfo pgmr;
	MobAttackInfo attack;
	switch (type) {
		case 0xFE: // Map/fall damage is an oddball packet
			break;
		default: // Code in common, minimizes repeated code
			mobid = packet->getInt();
			if (Mobs::mobinfo.find(mobid) == Mobs::mobinfo.end()) {
				// Hacking
				return;
			}
			mapmobid = packet->getInt();
			if (type != 0xFF)
				attack = Mobs::mobinfo[mobid].skills[type];
			hit = packet->getByte(); // Knock direction
			break;
 	}
	switch (type) { // Account for special sections of the damage packet
		case 0xFE:
			break;		
		default: // Else: Power Guard/Mana Reflection
			pgmr.reduction = packet->getByte();
			packet->skipBytes(1); // I think reduction is a short, but it's a byte in the S -> C packet, so..
			if (pgmr.reduction != 0) {
				if (packet->getByte() == 0)
					pgmr.isphysical = false; // Initialized as true, so the opposite case doesn't matter
				pgmr.mapmobid = packet->getInt();
				packet->skipBytes(1); // 0x06 for Power Guard, 0x00 for Mana Reflection?
				packet->skipBytes(4); // Mob position garbage
				pgmr.pos_x = packet->getShort();
				pgmr.pos_y = packet->getShort();
				pgmr.damage = damage;
				if (pgmr.isphysical) // Mana Reflection does not decrease damage
					damage = (damage - (damage * pgmr.reduction / 100)); 
				Mob *mob = Maps::maps[player->getMap()]->getMob(mapmobid);
				if (mob != 0) {
					mob->setHP(mob->getHP() - (pgmr.damage * pgmr.reduction / 100));
					Mobs::displayHPBars(player, mob);
					if (mob->getHP() <= 0)
						mob->die(player);
				}
			}
			break;
	}
	switch (type) { // Yes, another one, end of packets
		case 0xFE:
			packet->skipBytes(2); // Disease, normal end of packet
			break;
		default: 
			packet->skipBytes(1); // Stance, normal end of packet
			break;
 	}
	if (damage == -1) { // 0 damage = regular miss, -1 = Fake
		short job = player->getJob() / 10 - 40;
		fake = 4020002 + (job * 100000);
		if (player->getSkills()->getSkillLevel(fake) < 0) {
			//hacking
			return;
		}
	}
	if (player->getSkills()->getActiveSkillLevel(4211005) > 0 && player->getInventory()->getMesos() > 0) { // Meso Guard 
		int mesorate = Skills::skills[4211005][player->getSkills()->getActiveSkillLevel(4211005)].x; // Meso Guard meso %
		unsigned short hp = player->getHP();
		int mesoloss = (int)(mesorate * (damage / 2) / 100);
		int mesos = player->getInventory()->getMesos();
		int newmesos = mesos - mesoloss;
		if (newmesos > -1) {
			damage = (int)(damage / 2); // Usually displays 1 below the actual damage but is sometimes accurate - no clue why
		}
		else { // Special damage calculation for not having enough mesos
			double mesos2 = mesos + 0.0; // You can't get a double from math involving 2 ints, even if a decimal results
			double reduction = 2.0 - ((mesos2 / mesoloss) / 2);
			damage = (int)(damage / reduction); // This puts us pretty close to the damage observed clientside, needs improvement
			newmesos = 0;
		}
		player->getInventory()->setMesos(newmesos);
		SkillsPacket::showSkillEffect(player, 4211005);
		player->setHP(player->getHP() - damage);
		if (attack.deadlyattack)
			if (player->getMP() > 0)
				player->setMP(1);

		if (attack.mpburn)
			player->setMP(player->getMP() - attack.mpburn);
		applieddamage = true;
	}
 	if (player->getSkills()->getActiveSkillLevel(2001002) > 0) { // Magic Guard
		unsigned short mp = player->getMP();
		unsigned short hp = player->getHP();
		if (attack.deadlyattack) {
			if (mp > 0)
				player->setMP(1);
			player->setHP(1);
		}
		else if (attack.mpburn > 0) {
			player->setMP(mp - attack.mpburn);
		}
		else {
			unsigned short reduc = Skills::skills[2001002][player->getSkills()->getActiveSkillLevel(2001002)].x;
			int mpdamage = ((damage * reduc) / 100);
			int hpdamage = damage - mpdamage;
			if (mpdamage >= mp) {
				player->setMP(0);
				player->setHP(hp - (hpdamage + (mpdamage - mp)));
			}
			if (mpdamage < mp) {
				player->setMP(mp - mpdamage);
				player->setHP(hp - hpdamage);
			}
		}
		applieddamage = true;
 	}
	if (((job / 100) == 1) && ((job % 10) == 2)) { // Achilles for 4th job warriors
		int achx = 1000;
		int sid = 1120004;
		switch (job) {
			case 112: sid = 1120004; break;
			case 122: sid = 1220005; break;
			case 132: sid = 1230005; break;
		}
		int slv = player->getSkills()->getSkillLevel(sid);
		if (slv > 0) { achx = Skills::skills[sid][slv].x; }
		double red = (2.0 - achx / 1000.0);
		player->setHP(player->getHP() - (int)(damage / red));
		if (attack.deadlyattack && damage > 0)
			if (player->getMP() > 0)
				player->setMP(1);
		if (attack.mpburn > 0 && damage > 0)
			player->setMP(player->getMP() - attack.mpburn);
		applieddamage = true;
	}
	if (attack.disease > 0) {
		// Status ailment processing here
	}
	if (damage > 0 && applieddamage == false) {
		if (attack.deadlyattack) {
			if (player->getMP() > 0)
				player->setMP(1);
			player->setHP(1);
		}
		else
			player->setHP(player->getHP() - damage);
		if (attack.mpburn > 0)
			player->setMP(player->getMP() - attack.mpburn);
	}
 	PlayersPacket::damagePlayer(player, damage, mobid, hit, type, fake, pgmr);
}

void Players::healPlayer(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	short hp = packet->getShort();
	short mp = packet->getShort();
	player->setHP(player->getHP() + hp);
	player->setMP(player->getMP() + mp);
}

void Players::getPlayerInfo(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	PlayersPacket::showInfo(player, players[packet->getInt()]);
}

void Players::commandHandler(Player *player, ReadPacket *packet) {
	unsigned char type = packet->getByte();
	string name = packet->getString();

	string chat;
	if (type == 0x06) {
		chat = packet->getString();
	}

	hash_map<int, Player*>::iterator iter = Players::players.begin();
	for (iter = Players::players.begin(); iter != Players::players.end(); iter++) {
		if (StringUtilities::noCaseCompare(iter->second->getName(), name) == 0) {	
			if (type == 0x06) {
				PlayersPacket::whisperPlayer(iter->second, player->getName(), ChannelServer::Instance()->getChannel(), chat);
				PlayersPacket::findPlayer(player,iter->second->getName(),-1,1);
			}
			else if (type == 0x05) {
				PlayersPacket::findPlayer(player, iter->second->getName(), iter->second->getMap());
			}
			break;
		}
	}	
	if (iter == Players::players.end()) {
		if (type == 0x05)
			WorldServerConnectPlayerPacket::findPlayer(ChannelServer::Instance()->getWorldPlayer(), player->getPlayerid(), name); // Let's connect to the world server to see if the player is on any other channel
		else
			WorldServerConnectPlayerPacket::whisperPlayer(ChannelServer::Instance()->getWorldPlayer(), player->getPlayerid(), name, chat);
	}
}

void Players::handleSpecialSkills(Player *player, ReadPacket *packet) {
	int skillid = packet->getInt();
	switch (skillid) {
		case 1121001: // Monster Magnet
		case 1221001:
		case 1321001:
		case 3221001: // Pierce
		case 2121001: // Big Bang x3
		case 2221001:
		case 2321001: {
			SpecialSkillInfo info;
			info.skillid = skillid;
			info.level = packet->getByte();
			info.direction = packet->getByte();
			info.w_speed = packet->getByte();
			player->setSpecialSkill(info);
			SkillsPacket::showSpecialSkill(player, info);
			break;
		}
		case 4211001: { // Chakra, unknown heal formula
			short dex = player->getDex();
			short luk = player->getLuk();
			short recovery = Skills::skills[4211001][player->getSkills()->getSkillLevel(4211001)].y;
			short minimum = (luk / 2) * ((1 + 3 / 10) + recovery / 100) + (dex * recovery / 100);
			short maximum = luk * ((1 + 3 / 10) + recovery / 100) + (dex * recovery / 100);
			short range = maximum - minimum;
			player->setHP(player->getHP() + (Randomizer::Instance()->randInt(range) + minimum));
			break;
		}
	}
}

void Players::groupChatHandler(Player *player, ReadPacket *packet) {
	vector<int> receivers;
	char type = packet->getByte();
	unsigned char amount = packet->getByte();
	for (size_t i = 0; i < amount; i++) {
		receivers.push_back(packet->getInt());
	}
	string chat = packet->getString();
	
	WorldServerConnectPlayerPacket::groupChat(ChannelServer::Instance()->getWorldPlayer(), type, player->getPlayerid(), receivers, chat);
}
