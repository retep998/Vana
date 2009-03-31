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
#include "ChatHandler.h"
#include "Inventory.h"
#include "ItemDataProvider.h"
#include "MapPacket.h"
#include "MapleSession.h"
#include "Maps.h"
#include "Mobs.h"
#include "Database.h"
#include "NPCs.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "Pos.h"
#include "PacketReader.h"
#include "ShopDataProvider.h"
#include "StoragePacket.h"
#include "WorldServerConnectPacket.h"
#include <vector>
#include <boost/tr1/regex.hpp>

using std::vector;
using std::tr1::regex;
using std::tr1::cmatch;
using std::tr1::regex_match;

unordered_map<string, pair<CMD, int32_t> > ChatHandler::commandlist;

struct MeFunctor {
	void operator() (Player *gmplayer) {
		if (gmplayer->isGM()) {
			PlayerPacket::showMessage(gmplayer, msg, 6);
		}
	}
	string msg;
};

struct WarpFunctor {
	void operator() (Player *warpee) {
		if (warpee->getMap() != mapid) {
			Maps::changeMap(warpee, mapid, 0);
		}
	}
	int32_t mapid;
	Player *player;
};

void ChatHandler::initializeCommands() {
	typedef pair<CMD, int32_t> command;
	commandlist["ban"] = command(CMD_BAN, 3);
	commandlist["unban"] = command(CMD_UNBAN, 3);
	commandlist["header"] = command(CMD_HEADER, 3);
	commandlist["shutdown"] = command(CMD_SHUTDOWN, 3);
	commandlist["packet"] = command(CMD_PACKET, 3);
	commandlist["timer"] = command(CMD_TIMER, 3);
	commandlist["instruction"] = command(CMD_INSTRUCTION, 3);
	commandlist["addnpc"] = command(CMD_ADDNPC, 3);
	commandlist["dorankings"] = command(CMD_RANKINGCALC, 3);

	commandlist["me"] = command(CMD_ME, 2);
	commandlist["kick"] = command(CMD_KICK, 2);
	commandlist["warp"] = command(CMD_WARP, 2);
	commandlist["warpall"] = command(CMD_WARPALL, 2);
	commandlist["killall"] = command(CMD_KILLALL, 2);
	commandlist["cleardrops"] = command(CMD_CLEARDROPS, 2);

	commandlist["kill"] = command(CMD_KILL, 1);
	commandlist["lookup"] = command(CMD_LOOKUP, 1);
	commandlist["map"] = command(CMD_MAP, 1);
	commandlist["job"] = command(CMD_JOB, 1);
	commandlist["level"] = command(CMD_LEVEL, 1);
	commandlist["hp"] = command(CMD_HP, 1);
	commandlist["mp"] = command(CMD_MP, 1);
	commandlist["ap"] = command(CMD_AP, 1);
	commandlist["sp"] = command(CMD_SP, 1);
	commandlist["addsp"] = command(CMD_ADDSP, 1);
	commandlist["int"] = command(CMD_INT, 1);
	commandlist["luk"] = command(CMD_LUK, 1);
	commandlist["dex"] = command(CMD_DEX, 1);
	commandlist["str"] = command(CMD_STR, 1);
	commandlist["fame"] = command(CMD_FAME, 1);
	commandlist["maxstats"] = command(CMD_MAXSTATS, 1);
	commandlist["npc"] = command(CMD_NPC, 1);
	commandlist["item"] = command(CMD_ITEM, 1);
	commandlist["summon"] = command(CMD_SUMMON, 1);
	commandlist["spawn"] = command(CMD_SUMMON, 1);
	commandlist["notice"] = command(CMD_NOTICE, 1);
	commandlist["shop"] = command(CMD_SHOP, 1);
	commandlist["pos"] = command(CMD_POS, 1);
	commandlist["zakum"] = command(CMD_ZAKUM, 1);
	commandlist["horntail"] = command(CMD_HORNTAIL, 1);
	commandlist["heal"] = command(CMD_HEAL, 1);
	commandlist["mesos"] = command(CMD_MESOS, 1);
	commandlist["dc"] = command(CMD_DC, 1);
	commandlist["music"] = command(CMD_MUSIC, 1);
	commandlist["storage"] = command(CMD_STORAGE, 1);
	commandlist["eventinstruct"] = command(CMD_EVENTINSTRUCTION, 1);
	commandlist["save"] = command(CMD_SAVE, 1);
	commandlist["warpto"] = command(CMD_WARPTO, 1);
	commandlist["killnpc"] = command(CMD_KILLNPC, 1);
}

void ChatHandler::handleChat(Player *player, PacketReader &packet) {
	string message = packet.getString();
	int8_t bubbleOnly = packet.get<int8_t>(); // Skill Macros only display chat bubbles

	if (player->isGM() && message[0] == '!' && message.size() > 2) {
		char *chat = const_cast<char *>(message.c_str());
		string command = strtok(chat+1, " ");
		string args = message.length() > command.length() + 2 ? message.substr(command.length() + 2) : "";
		regex re; // Regular expression for use by commands with more complicated structures
		cmatch matches; // Regular expressions match for such commands

		if (commandlist.find(command) == commandlist.end())
			return;

		if (player->getGMLevel() >= commandlist[command].second) { // GM level for the command
			switch (commandlist[command].first) { // CMD constant associated with command
				case CMD_HEADER:
					WorldServerConnectPacket::scrollingHeader(ChannelServer::Instance()->getWorldPlayer(), args);
					break;
				case CMD_BAN: {
					re = "(\\w+) ?(\\d+)?";
					if (regex_match(args.c_str(), matches, re)) {
						string targetname = matches[1];
						if (Player *target = Players::Instance()->getPlayer(targetname))
							target->getSession()->disconnect();

						string reasonstring = matches[2];
						int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;

						// Ban account
						mysqlpp::Query accbanquery = Database::getCharDB().query();
						accbanquery << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_reason = " << (int16_t) reason << ", users.ban_expire = '9000-00-00 00:00:00' WHERE characters.name = '" << targetname << "'";
						accbanquery.exec();

						string banmsg = targetname + " has been banned";

						switch (reason) {
							case 0x01: banmsg += " for hacking."; break;
							case 0x02: banmsg += " for using macro/auto-keyboard."; break;
							case 0x03: banmsg += " for illicit promotion or advertising."; break;
							case 0x04: banmsg += " for harassment."; break;
							case 0x05: banmsg += " for using profane language."; break;
							case 0x06: banmsg += " for scamming."; break;
							case 0x07: banmsg += " for misconduct."; break;
							case 0x08: banmsg += " for illegal cash transaction."; break;
							case 0x09: banmsg += " for illegal charging/funding. Please contact customer support for further details."; break;
							case 0x0A: banmsg += " for temporary request."; break;
							case 0x0B: banmsg += " for impersonating GM."; break;
							case 0x0C: banmsg += " for using illegal programs or violating the game policy."; break;
							case 0x0D: banmsg += " for one of cursing, scamming, or illegal trading via Megaphones."; break;
							default: banmsg += "."; break;
						}
						PlayersPacket::showMessage(banmsg, 0);
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !ban <$playername> [#reason]", 6);
					}
					break;
				}
				case CMD_UNBAN: {
					if (args.length() != 0) {
						// Unban account
						mysqlpp::Query accbanquery = Database::getCharDB().query();
						accbanquery << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_expire = '0000-00-00 00:00:00' WHERE characters.name = '" << args << "'";
						accbanquery.exec();

						PlayerPacket::showMessage(player, string(args) + " has been unbanned.", 6);
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !unban <$playername>", 6);
					}
					break;
				}
				case CMD_SHUTDOWN:
					ChannelServer::Instance()->shutdown();
					break;
				case CMD_PACKET: {
					if (args.length() != 0) {
						PacketCreator packet;
						packet.addBytes(args.c_str());
						player->getSession()->send(packet);
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !packet <$hexbytes>", 6);
					}
					break;
				}
				case CMD_TIMER:
					if (args.length() != 0) {
						Maps::getMap(player->getMap())->setMapTimer(atoi(args.c_str()));
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !timer <#time>", 6);
					}
					break;
				case CMD_INSTRUCTION:
					if (args.length() != 0) {
						for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
							PlayerPacket::instructionBubble(Maps::getMap(player->getMap())->getPlayer(i), args);
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !instruction <$bubbletext>", 6);
					}
					break;
				case CMD_ADDNPC: {
					if (args.length() != 0) {
						NPCSpawnInfo npc;
						npc.id = atoi(args.c_str());
						npc.fh = 0;
						npc.pos = player->getPos();
						npc.rx0 = npc.pos.x - 50;
						npc.rx1 = npc.pos.x + 50;
						Maps::getMap(player->getMap())->addNPC(npc);
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !addnpc <#npcid>", 6);
					}
					break;
				}
				case CMD_ME: {
					if (args.length() != 0) {
						string msg = player->getName() + " : " + args;
						MeFunctor func = {msg};
						Players::Instance()->run(func);
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !me <$message>", 6);
					}
					break;
				}
				case CMD_KICK:
					if (args.length() != 0) {
						if (Player *target = Players::Instance()->getPlayer(args)) {
							if (player->getGMLevel() > target->getGMLevel())
								target->getSession()->disconnect();
							else
								PlayerPacket::showMessage(player, "Player outranks you.", 6);
						}
						else
							PlayerPacket::showMessage(player, "Invalid player or player is offline.", 6);
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !kick <$playername>", 6);
					}
					break;
				case CMD_WARP:
					re = "(\\w+) ?(\\d*)?";
					if (regex_match(args.c_str(), matches, re)) {
						if (Player *warpee = Players::Instance()->getPlayer(matches[1])) {
							string mapstring = matches[2];
							int32_t mapid = mapstring.length() > 0 ? atoi(mapstring.c_str()) : player->getMap();

							if (Maps::getMap(mapid)) {
								Maps::changeMap(warpee, mapid, 0);
							}
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !warp <$playername> [#mapid]", 6);
					}
					break;
				case CMD_WARPALL: {
					int32_t mapid = args.length() != 0 ? atoi(args.c_str()) : player->getMap();

					if (Maps::getMap(mapid)) {
						WarpFunctor func = {mapid, player};
						Players::Instance()->run(func);
					}
					else {
						PlayerPacket::showMessage(player, "Invalid Map ID", 6);
					}
					break;
				}
				case CMD_KILLALL:
					Maps::getMap(player->getMap())->killMobs(player);
					break;
				case CMD_CLEARDROPS:
					Maps::getMap(player->getMap())->clearDrops();
					break;
				case CMD_KILL:
					if (player->getGMLevel() == 1)
						player->setHP(0);
					else {
						if (args == "all") {
							for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
								Player *killpsa;
								killpsa = Maps::getMap(player->getMap())->getPlayer(i);
								if (killpsa != player) {
									killpsa->setHP(0);
								}
							}
						}
						else if (args == "gm") {
							for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
								Player *killpsa;
								killpsa = Maps::getMap(player->getMap())->getPlayer(i);
								if (killpsa != player) {
									if (killpsa->isGM()) {
										killpsa->setHP(0);
									}
								}
							}
						}
						else if (args == "players") {
							for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
								Player *killpsa;
								killpsa = Maps::getMap(player->getMap())->getPlayer(i);
								if (killpsa != player) {
									if (!killpsa->isGM()) {
										killpsa->setHP(0);
									}
								}
							}
						}
						else if (args == "me") {
							player->setHP(0);
						}
						else if (Player *killpsa = Players::Instance()->getPlayer(args)) { // Kill by name
							killpsa->setHP(0);
						}
					}
					break;
				case CMD_LOOKUP: {
					re = "(\\w+) (.+)";
					if (regex_match(args.c_str(), matches, re)) {
						uint16_t type = 0;
						if (matches[1] == "item") type = 1;
						else if (matches[1] == "skill") type = 2;
						else if (matches[1] == "map") type = 3;
						else if (matches[1] == "mob") type = 4;
						else if (matches[1] == "npc") type = 5;

						if (type != 0) {
							mysqlpp::Query query = Database::getDataDB().query();
							query << "SELECT objectid, name FROM stringdata WHERE type=" << type << " AND name LIKE " << mysqlpp::quote << ("%" + (string) matches[2] + "%");
							mysqlpp::StoreQueryResult res = query.store();

							if (res.num_rows() == 0) {
								PlayerPacket::showMessage(player, "No results", 6);
							}
							else {
								for (size_t i = 0; i < res.num_rows(); i++) {
									string msg = (string) res[i][0] + " : " + (string) res[i][1];
									PlayerPacket::showMessage(player, msg, 6);
								}
							}
						}
						else {
							PlayerPacket::showMessage(player, "Invalid search type - valid options are: {item, skill, map, mob, npc}", 6);
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !lookup <${item | skill | map | mob | npc}> <$search string>", 6);
					}
					break;
				}
				case CMD_MAP: {
					if (args.length() != 0) {
						int32_t mapid = -1;
						if (args == "town") mapid = Maps::getMap(player->getMap())->getInfo()->rm;
						else if (args == "southperry") mapid = 60000;
						else if (args == "amherst") mapid = 1010000;
						else if (args == "gm") mapid = 180000000;
						else if (args == "fm") mapid = 910000000;
						else if (args == "4th") mapid = 240010501;
						else if (args == "showa") mapid = 801000000;
						else if (args == "armory") mapid = 801040004;
						else if (args == "shrine") mapid = 800000000;
						else if (args == "mansion") mapid = 682000000;
						else if (args == "henesys") mapid = 100000000;
						else if (args == "perion") mapid = 102000000;
						else if (args == "ellinia") mapid = 101000000;
						else if (args == "sleepywood") mapid = 105040300;
						else if (args == "lith") mapid = 104000000;
						else if (args == "florina") mapid = 110000000;
						else if (args == "kerning") mapid = 103000000;
						else if (args == "port") mapid = 120000000;
						else if (args == "orbis") mapid = 200000000;
						else if (args == "nath") mapid = 211000000;
						else if (args == "ludi") mapid = 220000000;
						else if (args == "kft") mapid = 222000000;
						else if (args == "aqua") mapid = 230000000;
						else if (args == "omega") mapid = 221000000;
						else if (args == "leafre") mapid = 240000000;
						else if (args == "mulung") mapid = 250000000;
						else if (args == "herbtown") mapid = 251000000;
						else if (args == "ariant") mapid = 260000000;
						else if (args == "nlc") mapid = 600000000;
						else if (args == "amoria") mapid = 680000000;
						else if (args == "happyville") mapid = 209000000;
						else if (args == "crimsonwood") mapid = 610020006;
						else if (args == "singapore") mapid = 540000000;
						else if (args == "quay") mapid = 541000000;
						// Boss maps
						else if (args == "ergoth") mapid = 990000900;
						else if (args == "pap") mapid = 220080001;
						else if (args == "zakum") mapid = 280030000;
						else if (args == "horntail") mapid = 240060200;
						else if (args == "lordpirate") mapid = 925100500;
						else if (args == "alishar") mapid = 922010900;
						else if (args == "papapixie") mapid = 920010800;
						else if (args == "kingslime") mapid = 103000804;
						else if (args == "pianus") mapid = 230040420;
						else if (args == "manon") mapid = 240020401;
						else if (args == "griffey") mapid = 240020101;
						else if (args == "jrbalrog") mapid = 105090900;
						else if (args == "grandpa") mapid = 801040100;
						else if (args == "anego") mapid = 801040003;
						else if (args == "tengu") mapid = 800020130;
						else {
							char *endptr;
							mapid = strtol(args.c_str(), &endptr, 0);
							if (strlen(endptr) != 0) mapid = -1;
						}
						if (Maps::getMap(mapid))
							Maps::changeMap(player, mapid, 0);
						else
							PlayerPacket::showMessage(player, "Invalid Map ID", 6);
					}
					else {
						char msg[60];
						sprintf(msg, "Current Map: %i", player->getMap());
						PlayerPacket::showMessage(player, msg, 6);
					}
					break;
				}
				case CMD_NPC: {
					if (args.length() != 0) {
						int32_t npcid = atoi(args.c_str());
						NPC *npc = new NPC(npcid, player);
						npc->run();
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !npc <#npcid>", 6);
					}
					break;
				}
				case CMD_ADDSP: {
					re = "(\\d+) ?(-{0,1}\\d+)?";
					if (regex_match(args.c_str(), matches, re)) {
						int32_t skillid = atoi(string(matches[1]).c_str());
						if (Skills::skills.find(skillid) != Skills::skills.end()) { // Don't allow skills that do not exist to be added
							string countstring = matches[2];
							uint8_t count = countstring.length() > 0 ? atoi(countstring.c_str()) : 1;

							player->getSkills()->addSkillLevel(skillid, count);
						}
						else {
							PlayerPacket::showMessage(player, "Invalid Skill ID", 6);
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !addsp <#skillid> [#amount]", 6);
					}
					break;
				}
				case CMD_SUMMON: {
					re = "(\\d+) ?(\\d+)?";
					if (regex_match(args.c_str(), matches, re)) {
						int32_t mobid = atoi(string(matches[1]).c_str());
						if (MobDataProvider::Instance()->mobExists(mobid)) {
							string countstring = matches[2];
							int32_t count = countstring.length() > 0 ? atoi(countstring.c_str()) : 1;
							for (int32_t i = 0; i < count && i < 100; i++) {
								Maps::getMap(player->getMap())->spawnMob(mobid, player->getPos());
							}
						}
						else {
							PlayerPacket::showMessage(player, "Invalid Mob ID", 6);
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !" + command + " <#mobid> [#amount]", 6);
					}
					break;
				}
				case CMD_NOTICE:
					if (args.length() != 0)
						PlayersPacket::showMessage(args, 0);
					break;
				case CMD_MAXSTATS:
					player->setFame(30000);
					player->setRMHP(30000);
					player->setRMMP(30000);
					player->setMHP(30000);
					player->setMMP(30000);
					player->setStr(30000);
					player->setDex(30000);
					player->setInt(30000);
					player->setLuk(30000);
					break;
				case CMD_STR:
					if (args.length() != 0)
						player->setStr(atoi(args.c_str()));
					break;
				case CMD_DEX:
					if (args.length() != 0)
						player->setDex(atoi(args.c_str()));
					break;
				case CMD_LUK:
					if (args.length() != 0)
						player->setLuk(atoi(args.c_str()));
					break;
				case CMD_INT:
					if (args.length() != 0)
						player->setInt(atoi(args.c_str()));
					break;
				case CMD_HP:
					if (args.length() != 0) {
						uint16_t amount = atoi(args.c_str());
						player->setRMHP(amount);
						player->setMHP(amount);
						if (player->getHP() > amount)
							player->setHP(player->getMHP());
					}
					break;
				case CMD_MP:
					if (args.length() != 0) {
						uint16_t amount = atoi(args.c_str());
						player->setRMMP(amount);
						player->setMMP(amount);
						if (player->getMP() > amount)
							player->setMP(player->getMMP());
					}
					break;
				case CMD_FAME:
					if (args.length() != 0)
						player->setFame(atoi(args.c_str()));
					break;
				case CMD_SHOP: {
					if (args.length() != 0) {
						int32_t shopid = -1;
						if (args == "gear") shopid = 9999999;
						else if (args == "scrolls") shopid = 9999998;
						else if (args == "nx") shopid = 9999997;
						else if (args == "face") shopid = 9999996;
						else if (args == "ring") shopid = 9999995;
						else if (args == "chair") shopid = 9999994;
						else if (args == "mega") shopid = 9999993;
						else if (args == "pet") shopid = 9999992;
						else shopid = atoi(args.c_str());

						if (!ShopDataProvider::Instance()->showShop(player, shopid)) {
							PlayerPacket::showMessage(player, "Usage: !shop <${gear, scrolls, nx, face, ring, chair, mega, pet} | #shopid>", 6);
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !shop <${gear, scrolls, nx, face, ring, chair, mega, pet} | #shopid>", 6);
					}
					break;
				}
				case CMD_POS: {
					char msg[50];
					sprintf(msg, "X: %d Y: %d FH: %d", player->getPos().x, player->getPos().y, player->getFH());
					PlayerPacket::showMessage(player, msg, 6);
					break;
				}
				case CMD_ITEM: {
					re = "(\\d+) ?(\\d*)?";
					if (regex_match(args.c_str(), matches, re)) {
						int32_t itemid = atoi(string(matches[1]).c_str());
						if (ItemDataProvider::Instance()->itemExists(itemid)) {
							string countstring = matches[2];
							uint16_t count = countstring.length() > 0 ? atoi(countstring.c_str()) : 1;
							Inventory::addNewItem(player, itemid, count);
						}
						else {
							PlayerPacket::showMessage(player, "Invalid Item ID", 6);
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !item <#itemid> [#amount]", 6);
					}
					break;
				}
				case CMD_LEVEL:
					if (args.length() != 0)
						player->setLevel(atoi(args.c_str()));
					break;
				case CMD_JOB: {
					if (args.length() != 0) {
						int16_t job = -1;
						if (args == "beginner") job = 0;
						else if (args == "warrior") job = 100;
						else if (args == "fighter") job = 110;
						else if (args == "sader") job = 111;
						else if (args == "hero") job = 112;
						else if (args == "page") job = 120;
						else if (args == "wk") job = 121;
						else if (args == "paladin") job = 122;
						else if (args == "spearman") job = 130;
						else if (args == "dk") job  = 131;
						else if (args == "drk") job = 132;
						else if (args == "mage") job = 200;
						else if (args == "fpwiz") job = 210;
						else if (args == "fpmage") job = 211;
						else if (args == "fparch") job = 212;
						else if (args == "ilwiz") job = 220;
						else if (args == "ilmage") job = 221;
						else if (args == "ilarch") job = 222;
						else if (args == "cleric") job = 230;
						else if (args == "priest") job = 231;
						else if (args == "bishop") job = 232;
						else if (args == "bowman") job = 300;
						else if (args == "hunter") job = 310;
						else if (args == "ranger") job = 311;
						else if (args == "bm") job = 312;
						else if (args == "xbowman") job = 320;
						else if (args == "sniper") job = 321;
						else if (args == "marksman") job = 322;
						else if (args == "thief") job = 400;
						else if (args == "sin") job = 410;
						else if (args == "hermit") job = 411;
						else if (args == "nl") job = 412;
						else if (args == "dit") job = 420;
						else if (args == "cb") job = 421;
						else if (args == "shadower") job = 422;
						else if (args == "pirate") job = 500;
						else if (args == "infighter") job = 510;
						else if (args == "buccaneer") job = 511;
						else if (args == "viper") job = 512;
						else if (args == "gunslinger") job = 520;
						else if (args == "valkyrie") job = 521;
						else if (args == "captain") job = 522;
						else if (args == "gm") job = 900;
						else if (args == "sgm") job = 910;
						else job = atoi(args.c_str());

						if (job >= 0)
							player->setJob(job);
					}
					else {
						char msg[60];
						sprintf(msg, "Current Job: %i", player->getJob());
						PlayerPacket::showMessage(player, msg, 6);
					}
					break;
				}
				case CMD_AP:
					if (args.length() != 0)
						player->setAP(atoi(args.c_str()));
					break;
				case CMD_SP:
					if (args.length() != 0)
						player->setSP(atoi(args.c_str()));
					break;
				case CMD_KILLNPC:
					player->setNPC(0);
					break;
				case CMD_HORNTAIL:
					Maps::getMap(player->getMap())->spawnMob(8810026, player->getPos());
					Maps::getMap(player->getMap())->killMobs(player, 8810026);
					break;
				case CMD_HEAL:
					player->setHP(player->getMHP());
					player->setMP(player->getMMP());
					break;
				case CMD_MESOS:
					if (args.length() != 0)
						player->getInventory()->setMesos(atoi(args.c_str()));
					break;
				case CMD_SAVE:
					player->saveAll();
					PlayerPacket::showMessage(player, "Your progress has been saved.", 5);
					break;
				case CMD_WARPTO:
					Player *warptoee;
					if (warptoee = Players::Instance()->getPlayer(args)) {
						Maps::changeMap(player, warptoee->getMap(), 0);
					}
					break;
				case CMD_ZAKUM:
					Maps::getMap(player->getMap())->spawnMob(8800000, player->getPos());
					Maps::getMap(player->getMap())->spawnMob(8800003, player->getPos());
					Maps::getMap(player->getMap())->spawnMob(8800004, player->getPos());
					Maps::getMap(player->getMap())->spawnMob(8800005, player->getPos());
					Maps::getMap(player->getMap())->spawnMob(8800006, player->getPos());
					Maps::getMap(player->getMap())->spawnMob(8800007, player->getPos());
					Maps::getMap(player->getMap())->spawnMob(8800008, player->getPos());
					Maps::getMap(player->getMap())->spawnMob(8800009, player->getPos());
					Maps::getMap(player->getMap())->spawnMob(8800010, player->getPos());
					break;
				case CMD_MUSIC:
					Maps::getMap(player->getMap())->setMusic(args);
					break;
				case CMD_DC:
					player->getSession()->disconnect();
					break;
				case CMD_EVENTINSTRUCTION:
					MapPacket::showEventInstructions(player->getMap());
					break;
				case CMD_STORAGE:
					StoragePacket::showStorage(player, 9900000);
					break;
				case CMD_RANKINGCALC:
					WorldServerConnectPacket::rankingCalculation(ChannelServer::Instance()->getWorldPlayer());
					PlayerPacket::showMessage(player, "Sent a signal to force the calculation of rankings.", 5);
					break;
			}
		}
		return;
	}
	PlayersPacket::showChat(player, message, bubbleOnly);
}

void ChatHandler::handleGroupChat(Player *player, PacketReader &packet) {
	vector<int32_t> receivers;
	int8_t type = packet.get<int8_t>();
	uint8_t amount = packet.get<int8_t>();
	for (size_t i = 0; i < amount; i++) {
		receivers.push_back(packet.get<int32_t>());
	}
	string chat = packet.getString();

	WorldServerConnectPacket::groupChat(ChannelServer::Instance()->getWorldPlayer(), type, player->getId(), receivers, chat);
}
