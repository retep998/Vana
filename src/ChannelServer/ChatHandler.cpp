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
#include "Database.h"
#include "Inventory.h"
#include "IpUtilities.h"
#include "ItemDataProvider.h"
#include "MapleSession.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Mobs.h"
#include "NPCs.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "Pos.h"
#include "ShopDataProvider.h"
#include "StoragePacket.h"
#include "WorldServerConnectPacket.h"
#include <boost/tr1/regex.hpp>
#include <utility>
#include <vector>

using std::make_pair;
using std::tr1::cmatch;
using std::tr1::regex;
using std::tr1::regex_match;
using std::vector;

unordered_map<string, pair<Commands, int32_t> > ChatHandler::commandlist;

struct MeFunctor {
	void operator() (Player *gmplayer) {
		if (gmplayer->isGm()) {
			PlayerPacket::showMessage(gmplayer, msg, 6);
		}
	}
	string msg;
};

struct WarpFunctor {
	void operator() (Player *warpee) {
		if (warpee->getMap() != mapid) {
			warpee->setMap(mapid);
		}
	}
	int32_t mapid;
	Player *player;
};

void ChatHandler::initializeCommands() {
	commandlist["ban"] = make_pair(CmdBan, 3);
	commandlist["ipban"] = make_pair(CmdIpBan, 3);
	commandlist["tempban"] = make_pair(CmdTempBan, 3);
	commandlist["unban"] = make_pair(CmdUnban, 3);
	commandlist["header"] = make_pair(CmdHeader, 3);
	commandlist["shutdown"] = make_pair(CmdShutdown, 3);
	commandlist["packet"] = make_pair(CmdPacket, 3);
	commandlist["timer"] = make_pair(CmdTimer, 3);
	commandlist["instruction"] = make_pair(CmdInstruction, 3);
	commandlist["addnpc"] = make_pair(CmdAddNpc, 3);
	commandlist["dorankings"] = make_pair(CmdRankingCalc, 3);
	commandlist["globalmessage"] = make_pair(CmdGlobalMessage, 3);

	commandlist["me"] = make_pair(CmdMe, 2);
	commandlist["kick"] = make_pair(CmdKick, 2);
	commandlist["warp"] = make_pair(CmdWarp, 2);
	commandlist["warpall"] = make_pair(CmdWarpAll, 2);
	commandlist["killall"] = make_pair(CmdKillAll, 2);
	commandlist["cleardrops"] = make_pair(CmdClearDrops, 2);
	commandlist["worldmessage"] = make_pair(CmdWorldMessage, 2);

	commandlist["kill"] = make_pair(CmdKill, 1);
	commandlist["lookup"] = make_pair(CmdLookUp, 1);
	commandlist["map"] = make_pair(CmdMap, 1);
	commandlist["job"] = make_pair(CmdJob, 1);
	commandlist["level"] = make_pair(CmdLevel, 1);
	commandlist["hp"] = make_pair(CmdHp, 1);
	commandlist["mp"] = make_pair(CmdMp, 1);
	commandlist["ap"] = make_pair(CmdAp, 1);
	commandlist["sp"] = make_pair(CmdSp, 1);
	commandlist["addsp"] = make_pair(CmdAddSp, 1);
	commandlist["int"] = make_pair(CmdInt, 1);
	commandlist["luk"] = make_pair(CmdLuk, 1);
	commandlist["dex"] = make_pair(CmdDex, 1);
	commandlist["str"] = make_pair(CmdStr, 1);
	commandlist["fame"] = make_pair(CmdFame, 1);
	commandlist["maxstats"] = make_pair(CmdMaxStats, 1);
	commandlist["npc"] = make_pair(CmdNpc, 1);
	commandlist["item"] = make_pair(CmdItem, 1);
	commandlist["summon"] = make_pair(CmdSummon, 1);
	commandlist["spawn"] = make_pair(CmdSummon, 1);
	commandlist["notice"] = make_pair(CmdNotice, 1);
	commandlist["shop"] = make_pair(CmdShop, 1);
	commandlist["pos"] = make_pair(CmdPos, 1);
	commandlist["zakum"] = make_pair(CmdZakum, 1);
	commandlist["horntail"] = make_pair(CmdHorntail, 1);
	commandlist["heal"] = make_pair(CmdHeal, 1);
	commandlist["mesos"] = make_pair(CmdMesos, 1);
	commandlist["dc"] = make_pair(CmdDisconnect, 1);
	commandlist["music"] = make_pair(CmdMusic, 1);
	commandlist["storage"] = make_pair(CmdStorage, 1);
	commandlist["eventinstruct"] = make_pair(CmdEventInstruction, 1);
	commandlist["relog"] = make_pair(CmdRelog, 1);
	commandlist["save"] = make_pair(CmdSave, 1);
	commandlist["warpto"] = make_pair(CmdWarpTo, 1);
	commandlist["killnpc"] = make_pair(CmdKillNpc, 1);
	commandlist["listmobs"] = make_pair(CmdListMobs, 1);
	commandlist["getmobhp"] = make_pair(CmdGetMobHp, 1);
	commandlist["killmob"] = make_pair(CmdKillMob, 1);
}

void ChatHandler::handleChat(Player *player, PacketReader &packet) {
	string message = packet.getString();
	int8_t bubbleOnly = packet.get<int8_t>(); // Skill Macros only display chat bubbles

	if (player->isGm() && message[0] == '!' && message.size() > 2) {
		char *chat = const_cast<char *>(message.c_str());
		string command = strtok(chat+1, " ");
		string args = message.length() > command.length() + 2 ? message.substr(command.length() + 2) : "";
		regex re; // Regular expression for use by commands with more complicated structures
		cmatch matches; // Regular expressions match for such commands

		if (commandlist.find(command) == commandlist.end())
			return;

		if (player->getGmLevel() >= commandlist[command].second) { // GM level for the command
			switch (commandlist[command].first) { // CMD constant associated with command
				case CmdHeader:
					WorldServerConnectPacket::scrollingHeader(ChannelServer::Instance()->getWorldPlayer(), args);
					break;
				case CmdBan: {
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
				case CmdIpBan: {
					re = "(\\w+) ?(\\d+)?";
					if (regex_match(args.c_str(), matches, re)) {
						string targetname = matches[1];
						if (Player *target = Players::Instance()->getPlayer(targetname)) {
							string targetip = IpUtilities::ipToString(target->getIp());
							target->getSession()->disconnect();

							string reasonstring = matches[2];
							int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;

							// Ip ban
							mysqlpp::Query accipbanquery = Database::getCharDB().query();
							accipbanquery << "INSERT INTO `ipbans`(`id`, `ip`) VALUES (NULL, '" << targetip << "')";
							accipbanquery.exec();

							string banmsg = targetname + " has been IP banned";

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
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !ipban <$playername> [#reason]", 6);
					}
					break;
				}
				case CmdTempBan: {
					re = "(\\w+) (\\d+) (\\d+)";
					if (regex_match(args.c_str(), matches, re)) {
						string targetname = matches[1];
						if (Player *target = Players::Instance()->getPlayer(targetname))
							target->getSession()->disconnect();

						string reasonstring = matches[2];
						string length = matches[3];
						int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;

						// Ban account
						mysqlpp::Query accbanquery = Database::getCharDB().query();
						accbanquery << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_reason = " << (int16_t) reason << ", users.ban_expire = DATE_ADD(NOW(), INTERVAL " << length << " DAY) WHERE characters.name = '" << targetname << "'";
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
						PlayerPacket::showMessage(player, "Usage: !tempban <$playername> <#reason> <#length in days>", 6);
					}
					break;
				}
				case CmdUnban: {
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
				case CmdShutdown:
					ChannelServer::Instance()->shutdown();
					break;
				case CmdPacket: {
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
				case CmdTimer:
					if (args.length() != 0) {
						Maps::getMap(player->getMap())->setMapTimer(atoi(args.c_str()));
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !timer <#time>", 6);
					}
					break;
				case CmdInstruction:
					if (args.length() != 0) {
						for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
							PlayerPacket::instructionBubble(Maps::getMap(player->getMap())->getPlayer(i), args);
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !instruction <$bubbletext>", 6);
					}
					break;
				case CmdAddNpc: {
					if (args.length() != 0) {
						NPCSpawnInfo npc;
						npc.id = atoi(args.c_str());
						npc.fh = 0;
						npc.pos = player->getPos();
						npc.rx0 = npc.pos.x - 50;
						npc.rx1 = npc.pos.x + 50;
						npc.facingside = 1;
						Maps::getMap(player->getMap())->addNPC(npc);
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !addnpc <#npcid>", 6);
					}
					break;
				}
				case CmdMe: {
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
				case CmdKick:
					if (args.length() != 0) {
						if (Player *target = Players::Instance()->getPlayer(args)) {
							if (player->getGmLevel() > target->getGmLevel())
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
				case CmdWarp:
					re = "(\\w+) ?(\\d*)?";
					if (regex_match(args.c_str(), matches, re)) {
						if (Player *warpee = Players::Instance()->getPlayer(matches[1])) {
							string mapstring = matches[2];
							int32_t mapid = mapstring.length() > 0 ? atoi(mapstring.c_str()) : player->getMap();

							if (Maps::getMap(mapid)) {
								warpee->setMap(mapid);
							}
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !warp <$playername> [#mapid]", 6);
					}
					break;
				case CmdWarpAll: {
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
				case CmdKillAll:
					Maps::getMap(player->getMap())->killMobs(player);
					break;
				case CmdClearDrops:
					Maps::getMap(player->getMap())->clearDrops();
					break;
				case CmdKill:
					if (player->getGmLevel() == 1)
						player->setHp(0);
					else {
						if (args == "all") {
							for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
								Player *killpsa;
								killpsa = Maps::getMap(player->getMap())->getPlayer(i);
								if (killpsa != player) {
									killpsa->setHp(0);
								}
							}
						}
						else if (args == "gm") {
							for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
								Player *killpsa;
								killpsa = Maps::getMap(player->getMap())->getPlayer(i);
								if (killpsa != player) {
									if (killpsa->isGm()) {
										killpsa->setHp(0);
									}
								}
							}
						}
						else if (args == "players") {
							for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
								Player *killpsa;
								killpsa = Maps::getMap(player->getMap())->getPlayer(i);
								if (killpsa != player) {
									if (!killpsa->isGm()) {
										killpsa->setHp(0);
									}
								}
							}
						}
						else if (args == "me") {
							player->setHp(0);
						}
						else if (Player *killpsa = Players::Instance()->getPlayer(args)) { // Kill by name
							killpsa->setHp(0);
						}
					}
					break;
				case CmdLookUp: {
					re = "(\\w+) (.+)";
					if (regex_match(args.c_str(), matches, re)) {
						uint16_t type = 0;

						if (matches[1] == "item") type = 1;
						else if (matches[1] == "skill") type = 2;
						else if (matches[1] == "map") type = 3;
						else if (matches[1] == "mob") type = 4;
						else if (matches[1] == "npc") type = 5;
						else if (matches[1] == "quest") type = 6;

						else if (matches[1] == "id")  type = 100;

						if (type != 0) {
							mysqlpp::Query query = Database::getDataDB().query();
							if (type == 100) {
								query << "SELECT objectid, name FROM stringdata WHERE objectid = " << matches[2];
							}
							else {
								query << "SELECT objectid, name FROM stringdata WHERE type = " << type << " AND name LIKE " << mysqlpp::quote << ("%" + (string) matches[2] + "%");
							}

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
							PlayerPacket::showMessage(player, "Invalid search type - valid options are: {item, skill, map, mob, npc, quest, id}", 6);
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !lookup <${item | skill | map | mob | npc | quest | id}> <$search string>", 6);
					}
					break;
				}
				case CmdMap: {
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
						else if (args == "mine") mapid = 211041400;
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
						else if (args == "magatia") mapid = 261000000;
						else if (args == "temple") mapid = 270000000;
						else if (args == "altair") mapid = 300000000;
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
						else if (args == "lilynouch") mapid = 270020500;
						else if (args == "dodo") mapid = 270010500;
						else if (args == "lyka") mapid = 270030500;
						else if (args == "bean") mapid = 270050100;
						else {
							char *endptr;
							mapid = strtol(args.c_str(), &endptr, 0);
							if (strlen(endptr) != 0) mapid = -1;
						}
						if (Maps::getMap(mapid))
							player->setMap(mapid);
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
				case CmdNpc: {
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
				case CmdAddSp: {
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
				case CmdSummon: {
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
				case CmdNotice:
					if (args.length() != 0)
						PlayersPacket::showMessage(args, 0);
					break;
				case CmdMaxStats:
					player->setFame(30000);
					player->setRMHp(30000);
					player->setRMMp(30000);
					player->setMHp(30000);
					player->setMMp(30000);
					player->setStr(32767);
					player->setDex(32767);
					player->setInt(32767);
					player->setLuk(32767);
					break;
				case CmdStr:
					if (args.length() != 0)
						player->setStr(atoi(args.c_str()));
					break;
				case CmdDex:
					if (args.length() != 0)
						player->setDex(atoi(args.c_str()));
					break;
				case CmdLuk:
					if (args.length() != 0)
						player->setLuk(atoi(args.c_str()));
					break;
				case CmdInt:
					if (args.length() != 0)
						player->setInt(atoi(args.c_str()));
					break;
				case CmdHp:
					if (args.length() != 0) {
						uint16_t amount = atoi(args.c_str());
						player->setRMHp(amount);
						player->setMHp(amount);
						if (player->getHp() > amount)
							player->setHp(player->getMHp());
					}
					break;
				case CmdMp:
					if (args.length() != 0) {
						uint16_t amount = atoi(args.c_str());
						player->setRMMp(amount);
						player->setMMp(amount);
						if (player->getMp() > amount)
							player->setMp(player->getMMp());
					}
					break;
				case CmdFame:
					if (args.length() != 0)
						player->setFame(atoi(args.c_str()));
					break;
				case CmdShop: {
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
				case CmdPos: {
					char msg[50];
					sprintf(msg, "X: %d Y: %d FH: %d", player->getPos().x, player->getPos().y, player->getFh());
					PlayerPacket::showMessage(player, msg, 6);
					break;
				}
				case CmdItem: {
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
				case CmdLevel:
					if (args.length() != 0)
						player->setLevel(atoi(args.c_str()));
					break;
				case CmdJob: {
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
				case CmdAp:
					if (args.length() != 0)
						player->setAp(atoi(args.c_str()));
					break;
				case CmdSp:
					if (args.length() != 0)
						player->setSp(atoi(args.c_str()));
					break;
				case CmdKillNpc:
					player->setNPC(0);
					break;
				case CmdHorntail:
					Maps::getMap(player->getMap())->spawnMob(Mobs::SummonHorntail, player->getPos());
					break;
				case CmdHeal:
					player->setHp(player->getMHp());
					player->setMp(player->getMMp());
					break;
				case CmdMesos:
					if (args.length() != 0)
						player->getInventory()->setMesos(atoi(args.c_str()));
					break;
				case CmdRelog:
					player->changeChannel((int8_t)ChannelServer::Instance()->getChannel());
					break;
				case CmdSave:
					player->saveAll();
					PlayerPacket::showMessage(player, "Your progress has been saved.", 5);
					break;
				case CmdWarpTo:
					Player *warptoee;
					if (warptoee = Players::Instance()->getPlayer(args)) {
						player->setMap(warptoee->getMap());
					}
					break;
				case CmdZakum:
					Maps::getMap(player->getMap())->spawnZakum(player->getPos());
					break;
				case CmdMusic:
					Maps::getMap(player->getMap())->setMusic(args);
					break;
				case CmdDisconnect:
					player->getSession()->disconnect();
					break;
				case CmdEventInstruction:
					MapPacket::showEventInstructions(player->getMap());
					break;
				case CmdStorage:
					StoragePacket::showStorage(player, 9900000);
					break;
				case CmdRankingCalc:
					WorldServerConnectPacket::rankingCalculation(ChannelServer::Instance()->getWorldPlayer());
					PlayerPacket::showMessage(player, "Sent a signal to force the calculation of rankings.", 5);
					break;
				case CmdWorldMessage: {
					re = "(\\w+) (.+)";
					if (regex_match(args.c_str(), matches, re)) {
						int8_t type = -1;
						if (matches[1] == "notice") type = 0;
						else if (matches[1] == "popup") type = 1;
						else if (matches[1] == "event") type = 5;
						else if (matches[1] == "purple") type = 6;

						if (type != -1) {
							WorldServerConnectPacket::worldMessage(ChannelServer::Instance()->getWorldPlayer(), (string) matches[2], type);
						}
						else {
							PlayerPacket::showMessage(player, "Invalid message type - valid options are: {notice, popup, event, purple}", 6);
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !worldmessage <${notice | popup | event | purple}> <$message string>", 6);
					}
					break;
				}
				case CmdGlobalMessage: {
					re = "(\\w+) (.+)";
					if (regex_match(args.c_str(), matches, re)) {
						int8_t type = -1;
						if (matches[1] == "notice") type = 0;
						else if (matches[1] == "popup") type = 1;
						else if (matches[1] == "event") type = 5;
						else if (matches[1] == "purple") type = 6;

						if (type != -1) {
							WorldServerConnectPacket::globalMessage(ChannelServer::Instance()->getWorldPlayer(), (string) matches[2], type);
						}
						else {
							PlayerPacket::showMessage(player, "Invalid message type - valid options are: {notice, popup, event, purple}", 6);
						}
					}
					else {
						PlayerPacket::showMessage(player, "Usage: !globalmessage <${notice | popup | event | purple}> <$message string>", 6);
					}
					break;
				}
				case CmdListMobs: {
					string message = "No mobs on the current map.";
					if (Maps::getMap(player->getMap())->countMobs(0) > 0) {
						typedef unordered_map<int32_t, Mob *> mobmap;
						mobmap mobs = Maps::getMap(player->getMap())->getMobs();
						for (mobmap::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
							message = "Mob ";
							message += boost::lexical_cast<string>(iter->first);
							message += " (ID: ";
							message += boost::lexical_cast<string>(iter->second->getMobId());
							message += ", HP: ";
							message += boost::lexical_cast<string>(iter->second->getHp());
							message += "/";
							message += boost::lexical_cast<string>(iter->second->getMHp());
							message += ")";
							PlayerPacket::showMessage(player, message, 6);
						}
					}
					else {
						PlayerPacket::showMessage(player, message, 5);
					}
					break;
				}
				case CmdGetMobHp: {
					string message = "Mob does not exist.";
					if (args.length() != 0) {
						int32_t mobid = atoi(args.c_str());
						Mob *mob = Maps::getMap(player->getMap())->getMob(mobid);
						if (mob != 0) {
							message = "Mob ";
							message += boost::lexical_cast<string>(mobid);
							message += " HP: ";
							message += boost::lexical_cast<string>(mob->getHp());
							message += "/";
							message += boost::lexical_cast<string>(mob->getMHp());
							message += " (";
							message += boost::lexical_cast<string>(static_cast<int64_t>(mob->getHp()) * 100 / mob->getMHp());
							message += "%)";
						}
					}
					PlayerPacket::showMessage(player, message, 5);
					break;
				}
				case CmdKillMob: {
					if (args.length() != 0) {
						int32_t mobid = atoi(args.c_str());
						Mob *mob = Maps::getMap(player->getMap())->getMob(mobid);
						if (mob != 0) {
							mob->applyDamage(player->getId(), mob->getHp());
						}
					}
					break;
				}
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
