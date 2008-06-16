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
#include "PlayersPacket.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Maps.h" 
#include "Mobs.h"
#include "Shops.h"
#include "Inventory.h"
#include "Drops.h"
#include "Levels.h"
#include "ChannelServer.h"
#include "SkillsPacket.h"
#include "MapPacket.h"
#include "PacketCreator.h"
#include "BufferUtilities.h"
#include "ReadPacket.h"
#include "WorldServerConnectPlayerPacket.h"

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
	Pos cpos(packet->getShort(), packet->getShort());

	player->setPos(cpos);
	player->setType(type);

	packet->reset(7);
	PlayersPacket::showMoving(player, Maps::maps[player->getMap()]->getPlayers(), packet->getBuffer(), packet->getBufferLength());
}

void Players::faceExperiment(Player *player, ReadPacket *packet) {
	int face = packet->getInt();
	PlayersPacket::faceExperiment(player, Maps::maps[player->getMap()]->getPlayers(), face);
}

void Players::chatHandler(Player *player, ReadPacket *packet) {
	string message = packet->getString();
	char *chat = const_cast<char *>(message.c_str()); // Leaving chat as char[] for GM commands for now
	size_t chatsize = message.size(); // See above line
	if (chat[0] == '!') {
		if (!player->isGM()) return;
		char *next_token;
		char command[90] = "";
		if (chatsize > 2)
			strcpy_s(command, 90, strtok_s(chat+1, " ", &next_token));
		if (strcmp(command, "map") == 0) {
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
			else if (strcmp("nlc", next_token) == 0) mapid = 600000000;
			else if (strcmp("amoria", next_token) == 0) mapid = 680000000;
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
		else if (strcmp(command, "addsp") == 0) {
			if (strlen(next_token) > 0) {
				int skillid = atoi(strtok_s(0, " ", &next_token));
				int count = 1;
				if (strlen(next_token) > 0)
				count = atoi(next_token);
				player->skills->addSkillLevel(skillid, count);
			}
		}
		else if (strcmp(command, "summon") == 0 || strcmp(command, "spawn") == 0) {
			if (strlen(next_token) == 0) return;
			int mobid = atoi(strtok_s(0, " ", &next_token));
			if (Mobs::mobinfo.find(mobid) == Mobs::mobinfo.end())
				return;
			int count = 1;
			if (strlen(next_token) > 0)
				count = atoi(next_token);
			for (int i=0; i<count && i<100; i++) {
				Mobs::spawnMob(player, mobid);
			}
		}
		else if (strcmp(command, "notice") == 0) {
			if (strlen(next_token) == 0) return;
			PlayersPacket::showMessage(next_token, 0);
		}
		else if (strcmp(command, "shop") == 0) {
			Shops::showShop(player, 9999999);
		}
		else if (strcmp(command, "shop2") == 0) {
			Shops::showShop(player, 9999996);
		}
		else if (strcmp(command, "shop3") == 0) {
			Shops::showShop(player, 9999995);
		}
		else if (strcmp(command, "shop4") == 0) {
			Shops::showShop(player, 9999998);
		}
		else if (strcmp(command, "shop5") == 0) {
			Shops::showShop(player, 9999997);
		}
		else if (strcmp(command, "pos") == 0) {
			char text[50];
			sprintf_s(text, 50, "X: %d Y: %d", player->getPos().x, player->getPos().y);
			PlayerPacket::showMessage(player, text, 6);
		}
		else if (strcmp(command, "item") == 0) {
			if (strlen(next_token) == 0) return;
			int itemid = atoi(strtok_s(0, " ",&next_token));
			if (Drops::items.find(itemid) == Drops::items.end() && Drops::equips.find(itemid) == Drops::equips.end())
				return;
			int count = 1;
			if (strlen(next_token) > 0)
				count = atoi(next_token);
			Inventory::addNewItem(player, itemid, count);
		}
		else if (strcmp(command, "level") == 0) {
			if (strlen(next_token) == 0) return;
			Levels::setLevel(player, atoi(next_token));
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
			else if (strcmp(next_token, "gm") == 0) job = 500;
			else if (strcmp(next_token, "sgm") == 0) job = 510;
			else job = atoi(next_token);

			if (job >= 0)
				Levels::setJob(player, job);
		}
		else if (strcmp(command, "ap") == 0) {
			if (strlen(next_token) == 0) return;
			player->setAp(player->getAp()+atoi(next_token));
		}
		else if (strcmp(command, "sp") == 0) {
			if (strlen(next_token) == 0) return;
			player->setSp(player->getSp()+atoi(next_token));
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
			player->inv->setMesos(mesos);
		}
		else if (strcmp(command, "cleardrops") == 0) {
			Maps::maps[player->getMap()]->clearDrops();
		}
		else if (strcmp(command, "save") == 0) {
			player->save();
			PlayerPacket::showMessage(player, "Your progress has been saved.", 5);
		}
		else if (strcmp(command, "warp") == 0) {
			char *name = strtok_s(0, " ", &next_token);
			if (strlen(next_token) == 0) return;
			if (strlen(name) > 0)
				for (hash_map <int, Player*>::iterator iter = Players::players.begin(); iter != Players::players.end(); iter++)
					if (strcmp(iter->second->getName(), name) == 0)
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
					if (strcmp(iter->second->getName(), next_token) == 0)
						Maps::changeMap(player , iter->second->getMap(), iter->second->getMappos());
		}
		else if (strcmp(command, "mwarpto") == 0) {
			if (strlen(next_token) > 0)
				for (hash_map <int, Player*>::iterator iter = Players::players.begin(); iter != Players::players.end(); iter++)
					if (strcmp(iter->second->getName(), next_token) == 0) {
						Maps::changeMap(iter->second, player->getMap(), player->getMappos());
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
						if (strcmp(player->getName(), iter->second->getName())!=0)
							Maps::changeMap(iter->second, mapid, 0);
					}
					else
						Maps::changeMap(iter->second, mapid, 0);
				}
			}
		}
		else if (strcmp(command, "kill") == 0) {
			if (strcmp(next_token, "all") == 0) {
				for (unsigned int i = 0; i < Maps::maps[player->getMap()]->getPlayers().size(); i++) {
					Player *killpsa;
					killpsa = Maps::maps[player->getMap()]->getPlayers()[i];
					if (killpsa != player) {
						killpsa->setHP(0);
					}
				}
			}
			else if (strcmp(next_token, "gm") == 0) {
				for (unsigned int i = 0; i < Maps::maps[player->getMap()]->getPlayers().size(); i++) {
					Player *killpsa;
					killpsa = Maps::maps[player->getMap()]->getPlayers()[i];
					if (killpsa != player) {
						if (killpsa->isGM()) {	
							killpsa->setHP(0);
						}
					}
				}
			}
			else if (strcmp(next_token, "players") == 0) {
				for (unsigned int i = 0; i < Maps::maps[player->getMap()]->getPlayers().size(); i++) {	
					Player *killpsa;
					killpsa = Maps::maps[player->getMap()]->getPlayers()[i];
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
				for (unsigned int i = 0; i < Maps::maps[player->getMap()]->getPlayers().size(); i++) {
					Player *killpsa;
					killpsa = Maps::maps[player->getMap()]->getPlayers()[i];
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
		else if	(strcmp(command, "dc") == 0)	{
			player->disconnect();
			return;
		}
		else if (strcmp(command, "shutdown") == 0) {
			ChannelServer::Instance()->shutdown();
		}
		else if (strcmp(command, "packet") == 0) {
			Packet packet;
			packet.addBytes(next_token);
			packet.send(player);
		}
		else if (strcmp(command, "timer") == 0) {
			MapPacket::showTimer(player, atoi(next_token));
		}
		return;
	}
	PlayersPacket::showChat(player, Maps::maps[player->getMap()]->getPlayers(), message);
}

void Players::damagePlayer(Player *player, ReadPacket *packet) {
	packet->skipBytes(4); // Ticks
	unsigned char type = packet->getByte(); // 0xFF = bump, 0xFE = fall/map damage, 0x00-0x0* = mob skill ID
	int damage = packet->getInt();
	int mobid = packet->getInt(); // Actual Mob ID - i.e. 8800000 for Zak
	int mapmobid = packet->getInt(); // Map Mob ID
	packet->skipBytes(4); // No clue yet
	packet->skipBytes(1); // Knock direction
	PowerGuardInfo pg;
	pg.reduction = packet->getByte();
	packet->skipBytes(1); // Possible Mana Reflection?
	if (pg.reduction != 0 && type == -1) {
		packet->skipBytes(1); // Power Guard exists?
		pg.mapmobid = packet->getInt();
		packet->skipBytes(1); // 0x06, damage coming up
		packet->skipBytes(4); // Mob position garbage
		pg.pos_x = packet->getShort();
		pg.pos_y = packet->getShort();
		pg.damage = damage;
		damage = (damage - (damage * pg.reduction / 100));
	}
	packet->skipBytes(1); // Stance
	unsigned char hit = packet->getByte();
	int fake = 0;
	if (damage == -1) { // 0 damage = regular miss, this = Fake
		short job = player->getJob() / 10 - 40;
		fake = 4020002 + (job * 100000);
		if (player->skills->getSkillLevel(fake) < 0) {
			//hacking
			return;
		}
	}
	packet->skipBytes(4); // 4 random bytes if hit
	packet->skipBytes(4 * 10); // All take 4 bytes, in order: job, level, str, dex, int, luk, wdef, wdef buff, mdef, mdef buff
	packet->skipBytes(4); // Invincible %
	int mesorate = packet->getInt(); // Meso Guard meso %
	packet->skipBytes(4); // Power Guard % again
	packet->skipBytes(4); // Elemental/Partial Resistance %, normal end of packet
	if (player->skills->getActiveSkillLevel(2001002) > 0) { // Magic Guard
		unsigned short mp = player->getMP();
		unsigned short hp = player->getHP();
		unsigned short reduc = Skills::skills[2001002][player->skills->getActiveSkillLevel(2001002)].x;
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
	else if (mesorate > 0 && player->inv->getMesos() > 0) { // Meso Guard
		unsigned short hp = player->getHP();
		int mesoloss = (int)(mesorate * (damage / 2) / 100);
		int mesos = player->inv->getMesos();
		int newmesos = mesos - mesoloss;
		if (newmesos > -1) {
			damage = (int)(damage / 2); // Usually displays 1 below the actual damage but is sometimes accurate - no clue why
		}
		else { // Special damage calculation for not having enough mesos
			double mesos2 = mesos + 0.0; // For some reason, you can't get a double from math involving 2 ints, even if a decimal results
			double reduction = 2.0 - ((mesos2 / mesoloss) / 2);
			damage = (int)(damage / reduction); // This puts us pretty close to the damage observed clientside, needs improvement
			newmesos = 0;
		}
		player->inv->setMesos(newmesos);
		SkillsPacket::showSkillEffect(player, Maps::maps[player->getMap()]->getPlayers(), 4211005);
		player->setHP(player->getHP() - damage);
 	}
	else {
		if (damage > 0)
			player->setHP(player->getHP() - damage);
	}
	if (type == -1 && pg.reduction != 0) // Remove that damage, HP warrior!
		Mobs::damageMobPG(player, (pg.damage * pg.reduction / 100), Maps::maps[player->getMap()]->getMob(mapmobid));
	if (type != -2) // Fall damage and map damage don't play by these rules
		PlayersPacket::damagePlayer(player, Maps::maps[player->getMap()]->getPlayers(), damage, mobid, hit, type, fake, pg);
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
	for ( iter = Players::players.begin(); iter != Players::players.end(); iter++) {
		if (_stricmp(iter->second->getName(), name.c_str()) == 0) {	
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
			SkillsPacket::showSpecialSkill(player, Maps::maps[player->getMap()]->getPlayers(), info);
			break;
		}
		case 4211001: // Chakra, unknown heal formula
			break;
	}
}
