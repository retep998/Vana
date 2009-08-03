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
#include "BeautyDataProvider.h"
#include "Database.h"
#include "DropDataProvider.h"
#include "Inventory.h"
#include "IpUtilities.h"
#include "ItemDataProvider.h"
#include "MapDataProvider.h"
#include "MapleSession.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Mob.h"
#include "NPCs.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "Pos.h"
#include "ScriptDataProvider.h"
#include "ShopDataProvider.h"
#include "SkillDataProvider.h"
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
unordered_map<string, vector<string> > ChatHandler::commandnotes;
unordered_map<string, string> ChatHandler::commandsyntax;

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
	// Set up commands and appropriate GM levels

	// GM level 3
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

	// GM level 2
	commandlist["me"] = make_pair(CmdMe, 2);
	commandlist["kick"] = make_pair(CmdKick, 2);
	commandlist["warp"] = make_pair(CmdWarp, 2);
	commandlist["warpall"] = make_pair(CmdWarpAll, 2);
	commandlist["killall"] = make_pair(CmdKillAll, 2);
	commandlist["cleardrops"] = make_pair(CmdClearDrops, 2);
	commandlist["worldmessage"] = make_pair(CmdWorldMessage, 2);

	// GM level 1
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
	commandlist["reload"] = make_pair(CmdReload, 1);

	// No GM level needed
	commandlist["help"] = make_pair(CmdHelp, 0);

	// Set up syntax display, don't add commands that don't take parameters
	commandsyntax["ban"] = "<$playername> [#reason]";
	commandsyntax["ipban"] = "<$playername> [#reason]";
	commandsyntax["tempban"] = "<$playername> <#reason> <#length in days>";
	commandsyntax["unban"] = "<$playername>";
	commandsyntax["header"] = "[$message]";
	commandsyntax["packet"] = "<$hexbytes>";
	commandsyntax["timer"] = "<#time in seconds>";
	commandsyntax["instruction"] = "<$bubbletext>";
	commandsyntax["addnpc"] = "<#npcid>";
	commandsyntax["me"] = "<$message>";
	commandsyntax["kick"] = "<$playername>";
	commandsyntax["warp"] = "<$playername> [#mapid]";
	commandsyntax["warpall"] = "[#mapid]";
	commandsyntax["worldmessage"] = "<${notice | popup | event | purple}> <$message string>";
	commandsyntax["globalmessage"] = "<${notice | popup | event | purple}> <$message string>";
	commandsyntax["kill"] = "<${players | gm | all | me} | $playername>";
	commandsyntax["lookup"] = "<${item | skill | map | mob | npc | quest | continent | id}> <$search string | #id>";
	commandsyntax["map"] = "<${town | mapstring | bossmapstring} | #mapid>";
	commandsyntax["job"] = "<${jobstring} | #jobid>";
	commandsyntax["level"] = "<#level>";
	commandsyntax["hp"] = "<#hp>";
	commandsyntax["mp"] = "<#mp>";
	commandsyntax["ap"] = "<#ap>";
	commandsyntax["sp"] = "<#sp>";
	commandsyntax["addsp"] = "<#skillid> [#skillpoints]";
	commandsyntax["int"] = "<#int>";
	commandsyntax["luk"] = "<#luk>";
	commandsyntax["dex"] = "<#dex>";
	commandsyntax["str"] = "<#str>";
	commandsyntax["fame"] = "<#fame>";
	commandsyntax["npc"] = "<#npcid>";
	commandsyntax["item"] = "<#itemid> [#amount]";
	commandsyntax["summon"] = "<#mobid> [#amount]";
	commandsyntax["spawn"] = "<#mobid> [#amount]";
	commandsyntax["notice"] = "<$message>";
	commandsyntax["shop"] = "<${gear, scrolls, nx, face, ring, chair, mega, pet} | #shopid>";
	commandsyntax["mesos"] = "<#mesosamount>";
	commandsyntax["music"] = "[$musicname]";
	commandsyntax["warpto"] = "<$playername>";
	commandsyntax["getmobhp"] = "<#mapmobid>";
	commandsyntax["killmob"] = "<#mapmobid>";
	commandsyntax["reload"] = "<${items, drops, mobs, beauty, shops, skills}>";
	commandsyntax["help"] = "[$command]";

	// Set up function notes
	commandnotes["ban"].push_back("Permanently bans a player by name.");
	commandnotes["ban"].push_back("Reason codes:");
	commandnotes["ban"].push_back("1 - Hacking");
	commandnotes["ban"].push_back("2 - Using macro/auto-keyboard");
	commandnotes["ban"].push_back("3 - Illicit promotion or advertising");
	commandnotes["ban"].push_back("4 - Harassment");
	commandnotes["ban"].push_back("5 - Using profane language");
	commandnotes["ban"].push_back("6 - Scamming");
	commandnotes["ban"].push_back("7 - Misconduct");
	commandnotes["ban"].push_back("8 - Illegal cash transaction");
	commandnotes["ban"].push_back("9 - Illegal charging/funding");
	commandnotes["ban"].push_back("10 - Temporary request");
	commandnotes["ban"].push_back("11 - Impersonating GM");
	commandnotes["ban"].push_back("12 - Using illegal programs or violating the game policy");
	commandnotes["ban"].push_back("13 - Cursing, scamming, or illegal trading via megaphones");
	commandnotes["ipban"].push_back("Permanently bans a player's IP based on their name. Does not ban the account for various reasons.");
	commandnotes["ipban"].push_back("Use !help ban to see the applicable reason codes.");
	commandnotes["tempban"].push_back("Temporarily bans a player by name.");
	commandnotes["tempban"].push_back("Use !help ban to see the applicable reason codes.");
	commandnotes["unban"].push_back("Removes a ban from the database.");
	commandnotes["header"].push_back("Changes the scrolling message at the top of the screen.");
	commandnotes["shutdown"].push_back("Stops the current ChannelServer.");
	commandnotes["packet"].push_back("Sends a specific packet to yourself. Should only be used for testing.");
	commandnotes["timer"].push_back("Displays a timer at the top of the map.");
	commandnotes["instruction"].push_back("Displays a ");
	commandnotes["addnpc"].push_back("Permanently adds an NPC to a map.");
	commandnotes["dorankings"].push_back("Forces ranking recalculation.");
	commandnotes["globalmessage"].push_back("Displays a message to every channel on every world.");
	commandnotes["me"].push_back("Displays a message to all other online GMs.");
	commandnotes["kick"].push_back("Forcibly disconnects a player, cannot be used on players that outrank you in GM level.");
	commandnotes["warp"].push_back("Warps the specified player to your map or the map you specify.");
	commandnotes["warpall"].push_back("Warps all players to your map or the map you specify.");
	commandnotes["killall"].push_back("Kills all mobs on the current map.");
	commandnotes["cleardrops"].push_back("Clears all drops from the current map.");
	commandnotes["worldmessage"].push_back("Displays a message to every channel on the current world.");
	commandnotes["kill"].push_back("If you are GM level 1, you can only kill yourself with this.");
	commandnotes["kill"].push_back("If you are above GM level 1, you may kill GMs, players, everyone on a map, yourself, or the specified player.");
	commandnotes["lookup"].push_back("Uses the database to give you the string values for an ID or the IDs for a given string value.");
	commandnotes["lookup"].push_back("Use !help map to see valid string values for continent lookup.");
	commandnotes["map"].push_back("Warps you to a desired map.");
	commandnotes["map"].push_back("Valid map strings:");
	commandnotes["map"].push_back("southperry | amherst");
	commandnotes["map"].push_back("gm | fm | happyville | town | here");
	commandnotes["map"].push_back("showa | armory | shrine | singapore | quay");
	commandnotes["map"].push_back("henesys | perion | ellinia | sleepywood | lith | florina | kerning | port | dungeon | sharenian");
	commandnotes["map"].push_back("4th | orbis | nath | mine | leafre | temple | mulung | herbtown | ariant | magatia");
	commandnotes["map"].push_back("ludi | ereve | kft | aqua | omega | altair");
	commandnotes["map"].push_back("mansion | nlc | amoria | crimsonwood");
	commandnotes["map"].push_back("Valid boss map strings:");
	commandnotes["map"].push_back("ergoth | lordpirate | alishar | papapixie | kingslime");
	commandnotes["map"].push_back("pap | zakum | horntail | pianus | bean");
	commandnotes["map"].push_back("manon | griffey | jrbalrog | grandpa | anego | tengu | lilynouch | dodo | lyka");
	commandnotes["job"].push_back("Sets your job.");
	commandnotes["job"].push_back("Valid job strings:");
	commandnotes["job"].push_back("beginner");
	commandnotes["job"].push_back("warrior - fighter | sader | hero | page | wk | paladin | spearman | dk | drk");
	commandnotes["job"].push_back("magician - fpwiz | fpmage | fparch | ilwiz | ilmage | ilarch | cleric | priest | bishop");
	commandnotes["job"].push_back("bowman - hunter | ranger | bm | xbowman | sniper | marksman");
	commandnotes["job"].push_back("thief - sin | hermit | nl | dit | cb | shadower");
	commandnotes["job"].push_back("pirate - brawler | marauder | buccaneer | gunslinger | outlaw | corsair");
	commandnotes["job"].push_back("gm");
	commandnotes["job"].push_back("sgm");
	commandnotes["level"].push_back("Sets your player's level to the specified amount.");
	commandnotes["hp"].push_back("Sets your player's HP to the specified amount.");
	commandnotes["mp"].push_back("Sets your player's MP to the specified amount.");
	commandnotes["ap"].push_back("Sets your player's AP to the specified amount.");
	commandnotes["sp"].push_back("Sets your player's SP to the specified amount.");
	commandnotes["addsp"].push_back("Adds SP to the desired skill.");
	commandnotes["int"].push_back("Sets your player's INT to the specified amount.");
	commandnotes["luk"].push_back("Sets your player's LUK to the specified amount.");
	commandnotes["dex"].push_back("Sets your player's DEX to the specified amount.");
	commandnotes["str"].push_back("Sets your player's STR to the specified amount.");
	commandnotes["fame"].push_back("Sets your player's fame to the specified amount.");
	commandnotes["maxstats"].push_back("Sets all your core stats to their maximum values.");
	commandnotes["npc"].push_back("Runs the NPC script of the NPC you specify.");
	commandnotes["item"].push_back("Gives you an item.");
	commandnotes["summon"].push_back("Spawns monsters.");
	commandnotes["spawn"].push_back("Spawns monsters.");
	commandnotes["notice"].push_back("Displays a blue GM notice.");
	commandnotes["shop"].push_back("Shows you the desired shop.");
	commandnotes["pos"].push_back("Displays your current position and foothold on the map.");
	commandnotes["zakum"].push_back("Spawns Zakum.");
	commandnotes["horntail"].push_back("Spawns Horntail.");
	commandnotes["heal"].push_back("Sets your HP and MP to 100%.");
	commandnotes["mesos"].push_back("Sets your mesos to the specified amount.");
	commandnotes["dc"].push_back("Disconnects yourself.");
	commandnotes["music"].push_back("Sets the music for a given map.");
	commandnotes["storage"].push_back("Shows your storage.");
	commandnotes["eventinstruct"].push_back("Shows event instructions for Ola Ola, etc.");
	commandnotes["relog"].push_back("Logs you back in to the current channel.");
	commandnotes["save"].push_back("Saves your stats.");
	commandnotes["warpto"].push_back("Warps you to the specified player.");
	commandnotes["killnpc"].push_back("Used when scripts leave an NPC hanging. This command will clear the NPC and allow you to use other NPCs.");
	commandnotes["listmobs"].push_back("Lists all the mobs on the map.");
	commandnotes["getmobhp"].push_back("Gets the HP of a specific mob based on the map mob ID that you can get from !listmobs.");
	commandnotes["killmob"].push_back("Kills a specific mob based on the map mob ID that you can get from !listmobs.");
	commandnotes["reload"].push_back("Reloads data from the database.");
	commandnotes["help"].push_back("I wonder what it does?");
	commandnotes["help"].push_back("Syntax for help display:");
	commandnotes["help"].push_back("$ = string");
	commandnotes["help"].push_back("# = number");
	commandnotes["help"].push_back("${hi | bye} = specific choices, in this case, strings of hi or bye");
	commandnotes["help"].push_back("<#time in seconds> = required parameter");
	commandnotes["help"].push_back("[#time in seconds] = optional parameter");
}

void ChatHandler::showSyntax(Player *player, const string &command, bool fromHelp) {
	if (commandsyntax.find(command) != commandsyntax.end()) {
		string msg = "Usage: !" + command + " " + commandsyntax[command];
		PlayerPacket::showMessage(player, msg, 6);
	}
	else {
		PlayerPacket::showMessage(player, "Usage: !" + command, 6);
	}
	if (fromHelp && commandnotes.find(command) != commandnotes.end()) {
		PlayerPacket::showMessage(player, "Notes: " + commandnotes[command][0], 6);
		for (size_t i = 1; i < commandnotes[command].size(); i++) {
			PlayerPacket::showMessage(player, commandnotes[command][i], 6);
		}
	}
}

void ChatHandler::handleChat(Player *player, PacketReader &packet) {
	string message = packet.getString();
	int8_t bubbleOnly = packet.get<int8_t>(); // Skill Macros only display chat bubbles

	if (player->isGm() && message[0] == '!' && message.size() > 2) {
		char *chat = const_cast<char *>(message.c_str());
		string command = strtok(chat + 1, " ");
		string args = message.length() > command.length() + 2 ? message.substr(command.length() + 2) : "";
		regex re; // Regular expression for use by commands with more complicated structures
		cmatch matches; // Regular expressions match for such commands

		if (commandlist.find(command) == commandlist.end()) {
			PlayerPacket::showMessage(player, "Command \"" + command + "\" does not exist.", 6);
			return;
		}
		if (player->getGmLevel() < commandlist[command].second) { // GM level for the command
			PlayerPacket::showMessage(player, "You are not at a high enough GM level to use the command.", 6);
			return;
		}
		else {
			switch (commandlist[command].first) { // CMD constant associated with command
				case CmdHelp: {
					if (args.length() != 0) {
						if (commandlist.find(args) != commandlist.end()) {
							showSyntax(player, args, true);
						}
						else {
							PlayerPacket::showMessage(player, "Command \"" + args + "\" does not exist.", 6);
						}
					}
					else {
						string msg = "You may not use any commands.";
						bool has = false;
						for (unordered_map<string, pair<Commands, int32_t> >::iterator iter = commandlist.begin(); iter != commandlist.end(); iter++) {
							if (player->getGmLevel() >= iter->second.second) {
								if (!has) {
									msg = "Available commands: ";
									has = true;
								}
								msg += iter->first + " ";
							}
						}
						PlayerPacket::showMessage(player, msg, 6);
					}
					break;
				}
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
						showSyntax(player, command);
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
						showSyntax(player, command);
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
						showSyntax(player, command);
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
						showSyntax(player, command);
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
						showSyntax(player, command);
					}
					break;
				}
				case CmdTimer:
					if (args.length() != 0) {
						Maps::getMap(player->getMap())->setMapTimer(atoi(args.c_str()));
					}
					else {
						showSyntax(player, command);
					}
					break;
				case CmdInstruction:
					if (args.length() != 0) {
						for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
							PlayerPacket::instructionBubble(Maps::getMap(player->getMap())->getPlayer(i), args);
						}
					}
					else {
						showSyntax(player, command);
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
						showSyntax(player, command);
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
						showSyntax(player, command);
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
						showSyntax(player, command);
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
						showSyntax(player, command);
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

						else if (matches[1] == "continent") type = 200;

						if (type != 0 && type < 200) {
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
						else if (type == 200) {
							int32_t mapid = getMap(matches[2], player);
							if (Maps::getMap(mapid) != 0) {
								string message = boost::lexical_cast<string>(mapid) + " : " + boost::lexical_cast<string>((int32_t)(MapDataProvider::Instance()->getContinent(mapid)));
								PlayerPacket::showMessage(player, message, 6);
							}
							else {
								PlayerPacket::showMessage(player, "Invalid map", 6);
							}
						}
						else {
							PlayerPacket::showMessage(player, "Invalid search type - valid options are: {item, skill, map, mob, npc, quest, continent, id}", 6);
						}
					}
					else {
						showSyntax(player, command);
					}
					break;
				}
				case CmdMap: {
					if (args.length() != 0) {
						int32_t mapid = getMap(args, player);
						if (Maps::getMap(mapid)) {
							player->setMap(mapid);
						}
						else {
							PlayerPacket::showMessage(player, "Invalid Map ID", 6);
						}
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
						showSyntax(player, command);
					}
					break;
				}
				case CmdAddSp: {
					re = "(\\d+) ?(-{0,1}\\d+)?";
					if (regex_match(args.c_str(), matches, re)) {
						int32_t skillid = atoi(string(matches[1]).c_str());
						if (SkillDataProvider::Instance()->isSkill(skillid)) { // Don't allow skills that do not exist to be added
							string countstring = matches[2];
							uint8_t count = countstring.length() > 0 ? atoi(countstring.c_str()) : 1;

							player->getSkills()->addSkillLevel(skillid, count);
						}
						else {
							PlayerPacket::showMessage(player, "Invalid Skill ID", 6);
						}
					}
					else {
						showSyntax(player, command);
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
						showSyntax(player, command);
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
				case CmdReload: {
					if (args.length() != 0) {
						if (args == "items") ItemDataProvider::Instance()->loadData();
						else if (args == "drops") DropDataProvider::Instance()->loadData();
						else if (args == "shops") ShopDataProvider::Instance()->loadData();
						else if (args == "mobs") MobDataProvider::Instance()->loadData();
						else if (args == "beauty") BeautyDataProvider::Instance()->loadData();
						else if (args == "scripts") ScriptDataProvider::Instance()->loadData();
						else if (args == "skills") SkillDataProvider::Instance()->loadData();
						else {
							showSyntax(player, command);
							return;
						}
						PlayerPacket::showMessage(player, "Reloading " + args + " done!", 6);
					}
					else {
						showSyntax(player, command);
					}
					break;
				}
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
							showSyntax(player, command);
						}
					}
					else {
						showSyntax(player, command);
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
						showSyntax(player, command);
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
						else if (args == "magician") job = 200;
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
						else if (args == "brawler") job = 510;
						else if (args == "marauder") job = 511;
						else if (args == "buccaneer") job = 512;
						else if (args == "gunslinger") job = 520;
						else if (args == "outlaw") job = 521;
						else if (args == "corsair") job = 522;
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
						showSyntax(player, command);
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
						showSyntax(player, command);
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

int32_t ChatHandler::getMap(const string &query, Player *player) {
	int32_t mapid = -1;
	if (query == "here") mapid = player->getMap();
	else if (query == "town") mapid = Maps::getMap(player->getMap())->getInfo()->rm;
	else if (query == "southperry") mapid = 60000;
	else if (query == "amherst") mapid = 1010000;
	else if (query == "gm") mapid = 180000000;
	else if (query == "fm") mapid = 910000000;
	else if (query == "4th") mapid = 240010501;
	else if (query == "showa") mapid = 801000000;
	else if (query == "armory") mapid = 801040004;
	else if (query == "shrine") mapid = 800000000;
	else if (query == "mansion") mapid = 682000000;
	else if (query == "henesys") mapid = 100000000;
	else if (query == "perion") mapid = 102000000;
	else if (query == "sharenian") mapid = 101030104;
	else if (query == "ellinia") mapid = 101000000;
	else if (query == "sleepywood") mapid = 105040300;
	else if (query == "dungeon") mapid = 105090200;
	else if (query == "lith") mapid = 104000000;
	else if (query == "florina") mapid = 110000000;
	else if (query == "kerning") mapid = 103000000;
	else if (query == "port") mapid = 120000000;
	else if (query == "orbis") mapid = 200000000;
	else if (query == "nath") mapid = 211000000;
	else if (query == "mine") mapid = 211041400;
	else if (query == "ludi") mapid = 220000000;
	else if (query == "ereve") mapid = 130000200;
	else if (query == "kft") mapid = 222000000;
	else if (query == "aqua") mapid = 230000000;
	else if (query == "omega") mapid = 221000000;
	else if (query == "leafre") mapid = 240000000;
	else if (query == "mulung") mapid = 250000000;
	else if (query == "herbtown") mapid = 251000000;
	else if (query == "ariant") mapid = 260000000;
	else if (query == "nlc") mapid = 600000000;
	else if (query == "amoria") mapid = 680000000;
	else if (query == "happyville") mapid = 209000000;
	else if (query == "crimsonwood") mapid = 610020006;
	else if (query == "singapore") mapid = 540000000;
	else if (query == "quay") mapid = 541000000;
	else if (query == "magatia") mapid = 261000000;
	else if (query == "temple") mapid = 270000000;
	else if (query == "altair") mapid = 300000000;
	// Boss maps
	else if (query == "ergoth") mapid = 990000900;
	else if (query == "pap") mapid = 220080001;
	else if (query == "zakum") mapid = 280030000;
	else if (query == "horntail") mapid = 240060200;
	else if (query == "lordpirate") mapid = 925100500;
	else if (query == "alishar") mapid = 922010900;
	else if (query == "papapixie") mapid = 920010800;
	else if (query == "kingslime") mapid = 103000804;
	else if (query == "pianus") mapid = 230040420;
	else if (query == "manon") mapid = 240020401;
	else if (query == "griffey") mapid = 240020101;
	else if (query == "jrbalrog") mapid = 105090900;
	else if (query == "grandpa") mapid = 801040100;
	else if (query == "anego") mapid = 801040003;
	else if (query == "tengu") mapid = 800020130;
	else if (query == "lilynouch") mapid = 270020500;
	else if (query == "dodo") mapid = 270010500;
	else if (query == "lyka") mapid = 270030500;
	else if (query == "bean") mapid = 270050100;
	else {
		char *endptr;
		mapid = strtol(query.c_str(), &endptr, 0);
		if (strlen(endptr) != 0) mapid = -1;
	}
	return mapid;
}

void ChatHandler::handleGroupChat(Player *player, PacketReader &packet) {
	vector<int32_t> receivers;
	int8_t type = packet.get<int8_t>();
	uint8_t amount = packet.get<uint8_t>();
	for (uint8_t i = 0; i < amount; i++) {
		receivers.push_back(packet.get<int32_t>());
	}
	string chat = packet.getString();

	WorldServerConnectPacket::groupChat(ChannelServer::Instance()->getWorldPlayer(), type, player->getId(), receivers, chat);
}
