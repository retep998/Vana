/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "ChannelServer.h"
#include "Database.h"
#include "Inventory.h"
#include "IpUtilities.h"
#include "ItemDataProvider.h"
#include "MapDataProvider.h"
#include "MapleSession.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Mob.h"
#include "MobConstants.h"
#include "Npc.h"
#include "NpcHandler.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "Pos.h"
#include "ShopDataProvider.h"
#include "WorldServerConnectPacket.h"
#include <boost/lexical_cast.hpp>
#include <boost/tr1/regex.hpp>
#include <vector>

using boost::lexical_cast;
using std::tr1::cmatch;
using std::tr1::regex;
using std::tr1::regex_match;
using std::vector;
using ChatHandler::ChatCommand;

unordered_map<string, ChatCommand> ChatHandler::commandlist;

namespace Functors {
	struct MeFunctor {
		void operator() (Player *gmplayer) {
			if (gmplayer->isGm()) {
				PlayerPacket::showMessage(gmplayer, msg, PlayerPacket::NoticeTypes::Blue);
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

	struct NameFunctor {
		void operator() (Player *player) {
			if (*i < max) {
				if (*i != 0) {
					*names = *names + ", ";
				}
				*names = *names + player->getName();
				(*i)++;
			}
		}
		int32_t max;
		int32_t *i;
		string *names;
	};
}

void ChatHandler::initializeCommands() {
	// Set up commands and appropriate GM levels
	ChatCommand command;

	// Notes:
	// Don't add syntax to things that have no parameters
	// Every command needs at least one line of notes that describes what the command does

	// GM level 3
#pragma region GM Level 3
	command.level = 3;

	command.command = CmdBan;
	command.syntax = "<$playername> [#reason]";
	command.notes.push_back("Permanently bans a player by name.");
	command.notes.push_back("Reason codes:");
	command.notes.push_back("1 - Hacking");
	command.notes.push_back("2 - Using macro/auto-keyboard");
	command.notes.push_back("3 - Illicit promotion or advertising");
	command.notes.push_back("4 - Harassment");
	command.notes.push_back("5 - Using profane language");
	command.notes.push_back("6 - Scamming");
	command.notes.push_back("7 - Misconduct");
	command.notes.push_back("8 - Illegal cash transaction");
	command.notes.push_back("9 - Illegal charging/funding");
	command.notes.push_back("10 - Temporary request");
	command.notes.push_back("11 - Impersonating GM");
	command.notes.push_back("12 - Using illegal programs or violating the game policy");
	command.notes.push_back("13 - Cursing, scamming, or illegal trading via megaphones");
	commandlist["ban"] = command.addToMap();

	command.command = CmdIpBan;
	command.syntax = "<$playername> [#reason]";
	command.notes.push_back("Permanently bans a player's IP based on their name. Does not ban the account for various reasons.");
	command.notes.push_back("Use !help ban to see the applicable reason codes.");
	commandlist["ipban"] = command.addToMap();

	command.command = CmdTempBan;
	command.syntax = "<$playername> <#reason> <#length in days>";
	command.notes.push_back("Temporarily bans a player by name.");
	command.notes.push_back("Use !help ban to see the applicable reason codes.");
	commandlist["tempban"] = command.addToMap();

	command.command = CmdUnban;
	command.syntax = "<$playername>";
	command.notes.push_back("Removes a ban from the database.");
	commandlist["unban"] = command.addToMap();

	command.command = CmdHeader;
	command.syntax = "[$message]";
	command.notes.push_back("Changes the scrolling message at the top of the screen.");
	commandlist["header"] = command.addToMap();

	command.command = CmdShutdown;
	command.notes.push_back("Stops the current ChannelServer.");
	commandlist["shutdown"] = command.addToMap();

	command.command = CmdPacket;
	command.syntax = "<$hexbytes>";
	command.notes.push_back("Sends a specific packet to yourself. Should only be used for testing.");
	commandlist["packet"] = command.addToMap();

	command.command = CmdTimer;
	command.syntax = "<#time in seconds>";
	command.notes.push_back("Displays a timer at the top of the map.");
	commandlist["timer"] = command.addToMap();

	command.command = CmdInstruction;
	command.syntax = "<$bubbletext>";
	command.notes.push_back("Displays a bubble. With shiny text. Somewhat useless for managing players.");
	commandlist["instruction"] = command.addToMap();

	command.command = CmdAddNpc;
	command.syntax = "<#npcid>";
	command.notes.push_back("Permanently adds an NPC to a map.");
	commandlist["addnpc"] = command.addToMap();

	command.command = CmdRankingCalc;
	command.notes.push_back("Forces ranking recalculation.");
	commandlist["dorankings"] = command.addToMap();

	command.command = CmdGlobalMessage;
	command.syntax = "<${notice | box | red | blue}> <$message string>";
	command.notes.push_back("Displays a message to every channel on every world.");
	commandlist["globalmessage"] = command.addToMap();
#pragma endregion

	// GM level 2
#pragma region GM Level 2
	command.level = 2;

	command.command = CmdMe;
	command.syntax = "<$message>";
	command.notes.push_back("Displays a message to all other online GMs.");
	commandlist["me"] = command.addToMap();

	command.command = CmdKick;
	command.syntax = "<$playername>";
	command.notes.push_back("Forcibly disconnects a player, cannot be used on players that outrank you in GM level.");
	commandlist["kick"] = command.addToMap();

	command.command = CmdWarp;
	command.syntax = "<$playername> [#mapid]";
	command.notes.push_back("Warps the specified player to your map or the map you specify.");
	commandlist["warp"] = command.addToMap();

	command.command = CmdWarpAll;
	command.syntax = "[#mapid]";
	command.notes.push_back("Warps all players to your map or the map you specify.");
	commandlist["warpall"] = command.addToMap();

	command.command = CmdWarpMap;
	command.syntax = "[#mapid]";
	command.notes.push_back("Warps all players in your current map to your map or the map you specify.");
	commandlist["warpmap"] = command.addToMap();

	command.command = CmdKillAll;
	command.notes.push_back("Kills all mobs on the current map.");
	commandlist["killall"] = command.addToMap();

	command.command = CmdClearDrops;
	command.notes.push_back("Clears all drops from the current map.");
	commandlist["cleardrops"] = command.addToMap();

	command.command = CmdWorldMessage;
	command.syntax = "<${notice | box | red | blue}> <$message string>";
	command.notes.push_back("Displays a message to every channel on the current world.");
	commandlist["worldmessage"] = command.addToMap();
#pragma endregion

	// GM level 1
#pragma region GM Level 1
	command.level = 1;

	command.command = CmdKill;
	command.syntax = "<${players | gm | all | me} | $playername>";
	command.notes.push_back("If you are GM level 1, you can only kill yourself with this.");
	command.notes.push_back("If you are above GM level 1, you may kill GMs, players, everyone on a map, yourself, or the specified player.");
	commandlist["kill"] = command.addToMap();

	command.command = CmdLookUp;
	command.syntax = "<${item | skill | map | mob | npc | quest | continent | id | scriptbyname | scriptbyid}> <$search string | #id>";
	command.notes.push_back("Uses the database to give you the string values for an ID or the IDs for a given string value.");
	command.notes.push_back("Use !help map to see valid string values for continent lookup.");
	commandlist["lookup"] = command.addToMap();

	command.command = CmdMap;
	command.syntax = "<${town | mapstring | bossmapstring} | #mapid>";
	command.notes.push_back("Warps you to a desired map.");
	command.notes.push_back("-------------");
	command.notes.push_back("Valid maps");
	command.notes.push_back("-------------");
	command.notes.push_back("Special: gm | fm | happyville | town | here | 3rd | 4th | grendel | athena | darklord | danceswb | kyrin");
	command.notes.push_back("Maple Island: southperry | amherst");
	command.notes.push_back("Victoria: henesys | perion | ellinia | sleepywood | lith | florina | kerning | port | sharenian");
	command.notes.push_back("Ossyria: orbis | nath | leafre | mulung | herbtown | ariant | magatia");
	command.notes.push_back("Ludus Lake: ludi |  kft | aqua | omega | altaire");
	command.notes.push_back("Masteria: nlc | amoria | crimsonwood");
	command.notes.push_back("Landmasses: temple | ereve | rien");
	command.notes.push_back("World Tour: showa | shrine | singapore | quay | malaysia | kampung");
	command.notes.push_back("Dungeons: dungeon | mine | armory | mansion");
	command.notes.push_back("-------------");
	command.notes.push_back("Boss maps");
	command.notes.push_back("-------------");
	command.notes.push_back("PQ Bosses: ergoth | lordpirate | alishar | papapixie | kingslime");
	command.notes.push_back("Area Bosses: manon | griffey | jrbalrog | anego | tengu | lilynouch | dodo | lyka");
	command.notes.push_back("Bosses: pap | zakum | horntail | pianus | grandpa | bean");
	commandlist["map"] = command.addToMap();

	command.command = CmdJob;
	command.syntax = "<${jobstring} | #jobid>";
	command.notes.push_back("Sets your job.");
	command.notes.push_back("Valid job strings:");
	command.notes.push_back("beginner | noblesse");
	command.notes.push_back("warrior | fighter | sader | hero | page | wk | paladin | spearman | dk | drk");
	command.notes.push_back("magician | fpwiz | fpmage | fparch | ilwiz | ilmage | ilarch | cleric | priest | bishop");
	command.notes.push_back("bowman | hunter | ranger | bm | xbowman | sniper | marksman");
	command.notes.push_back("thief | sin | hermit | nl | dit | cb | shadower");
	command.notes.push_back("pirate | brawler | marauder | buccaneer | gunslinger | outlaw | corsair");
	command.notes.push_back("gm | sgm");
	command.notes.push_back("dawn1 | dawn2 | dawn3 | dawn4");
	command.notes.push_back("blaze1 | blaze2 | blaze3 | blaze4");
	command.notes.push_back("wind1 | wind2 | wind3 | wind4");
	command.notes.push_back("night1 | night2 | night3 | night4");
	command.notes.push_back("thunder1 | thunder2 | thunder3 | thunder4");
	commandlist["job"] = command.addToMap();

	command.command = CmdLevel;
	command.syntax = "<#level>";
	command.notes.push_back("Sets your player's level to the specified amount.");
	commandlist["level"] = command.addToMap();

	command.command = CmdHp;
	command.syntax = "<#hp>";
	command.notes.push_back("Sets your player's HP to the specified amount.");
	commandlist["hp"] = command.addToMap();

	command.command = CmdMp;
	command.syntax = "<#mp>";
	command.notes.push_back("Sets your player's MP to the specified amount.");
	commandlist["mp"] = command.addToMap();

	command.command = CmdAp;
	command.syntax = "<#ap>";
	command.notes.push_back("Sets your player's AP to the specified amount.");
	commandlist["ap"] = command.addToMap();

	command.command = CmdSp;
	command.syntax = "<#sp>";
	command.notes.push_back("Sets your player's SP to the specified amount.");
	commandlist["sp"] = command.addToMap();

	command.command = CmdAddSp;
	command.syntax = "<#skillid> [#skillpoints]";
	command.notes.push_back("Adds SP to the desired skill.");
	commandlist["addsp"] = command.addToMap();

	command.command = CmdInt;
	command.syntax = "<#int>";
	command.notes.push_back("Sets your player's INT to the specified amount.");
	commandlist["int"] = command.addToMap();

	command.command = CmdLuk;
	command.syntax = "<#luk>";
	command.notes.push_back("Sets your player's LUK to the specified amount.");
	commandlist["luk"] = command.addToMap();

	command.command = CmdDex;
	command.syntax = "<#dex>";
	command.notes.push_back("Sets your player's DEX to the specified amount.");
	commandlist["dex"] = command.addToMap();

	command.command = CmdStr;
	command.syntax = "<#str>";
	command.notes.push_back("Sets your player's STR to the specified amount.");
	commandlist["str"] = command.addToMap();

	command.command = CmdFame;
	command.syntax = "<#fame>";
	command.notes.push_back("Sets your player's fame to the specified amount.");
	commandlist["fame"] = command.addToMap();

	command.command = CmdMaxStats;
	command.notes.push_back("Sets all your core stats to their maximum values.");
	commandlist["maxstats"] = command.addToMap();

	command.command = CmdNpc;
	command.syntax = "<#npcid>";
	command.notes.push_back("Runs the NPC script of the NPC you specify.");
	commandlist["npc"] = command.addToMap();

	command.command = CmdItem;
	command.syntax = "<#itemid> [#amount]";
	command.notes.push_back("Gives you an item.");
	commandlist["item"] = command.addToMap();

	command.command = CmdSummon;
	command.syntax = "<#mobid> [#amount]";
	command.notes.push_back("Spawns monsters.");
	commandlist["summon"] = command;
	commandlist["spawn"] = command.addToMap();

	command.command = CmdNotice;
	command.syntax = "<$message>";
	command.notes.push_back("Displays a blue GM notice.");
	commandlist["notice"] = command.addToMap();

	command.command = CmdShop;
	command.syntax = "<${gear, scrolls, nx, face, ring, chair, mega, pet} | #shopid>";
	command.notes.push_back("Shows you the desired shop.");
	commandlist["shop"] = command.addToMap();

	command.command = CmdPos;
	command.notes.push_back("Displays your current position and foothold on the map.");
	commandlist["pos"] = command.addToMap();

	command.command = CmdZakum;
	command.notes.push_back("Spawns Zakum.");
	commandlist["zakum"] = command.addToMap();

	command.command = CmdHorntail;
	command.notes.push_back("Spawns Horntail.");
	commandlist["horntail"] = command.addToMap();

	command.command = CmdHeal;
	command.notes.push_back("Sets your HP and MP to 100%.");
	commandlist["heal"] = command.addToMap();

	command.command = CmdMesos;
	command.syntax = "<#mesosamount>";
	command.notes.push_back("Sets your mesos to the specified amount.");
	commandlist["mesos"] = command.addToMap();

	command.command = CmdDisconnect;
	command.notes.push_back("Disconnects yourself.");
	commandlist["dc"] = command.addToMap();

	command.command = CmdMusic;
	command.syntax = "[$musicname]";
	command.notes.push_back("Sets the music for a given map.");
	command.notes.push_back("Using \"default\" will reset the map to default music.");
	commandlist["music"] = command.addToMap();

	command.command = CmdStorage;
	command.notes.push_back("Shows your storage.");
	commandlist["storage"] = command.addToMap();

	command.command = CmdEventInstruction;
	command.notes.push_back("Shows event instructions for Ola Ola, etc.");
	commandlist["eventinstruct"] = command.addToMap();

	command.command = CmdRelog;
	command.notes.push_back("Logs you back in to the current channel.");
	commandlist["relog"] = command.addToMap();

	command.command = CmdSave;
	command.notes.push_back("Saves your stats.");
	commandlist["save"] = command.addToMap();

	command.command = CmdWarpTo;
	command.syntax = "<$playername>";
	command.notes.push_back("Warps you to the specified player.");
	commandlist["warpto"] = command.addToMap();

	command.command = CmdKillNpc;
	command.notes.push_back("Used when scripts leave an NPC hanging. This command will clear the NPC and allow you to use other NPCs.");
	commandlist["killnpc"] = command.addToMap();

	command.command = CmdListMobs;
	command.notes.push_back("Lists all the mobs on the map.");
	commandlist["listmobs"] = command.addToMap();

	command.command = CmdGetMobHp;
	command.syntax = "<#mapmobid>";
	command.notes.push_back("Gets the HP of a specific mob based on the map mob ID that you can get from !listmobs.");
	commandlist["getmobhp"] = command.addToMap();

	command.command = CmdKillMob;
	command.syntax = "<#mapmobid>";
	command.notes.push_back("Kills a specific mob based on the map mob ID that you can get from !listmobs.");
	commandlist["killmob"] = command.addToMap();

	command.command = CmdReload;
	command.syntax = "<${all, items, drops, mobs, beauty, shops, scripts, reactors, pets, quests, skills, cashshop}>";
	command.notes.push_back("Reloads data from the database.");
	commandlist["reload"] = command.addToMap();

	command.command = CmdCc;
	command.syntax = "<#channel>";
	command.notes.push_back("Allows you to change channels on any map.");
	commandlist["cc"] = command.addToMap();

	command.command = CmdOnline;
	command.notes.push_back("Allows you to see up to 100 players on the current channel.");
	commandlist["online"] = command.addToMap();

	command.command = CmdLag;
	command.syntax = "<$player>";
	command.notes.push_back("Allows you to view the lag of any player.");
	commandlist["lag"] = command.addToMap();
#pragma endregion

	// No GM level needed
#pragma region GM Level 0
	command.level = 0;

	command.command = CmdHelp;
	command.syntax = "[$command]";
	command.notes.push_back("I wonder what it does?");
	command.notes.push_back("Syntax for help display:");
	command.notes.push_back("$ = string");
	command.notes.push_back("# = number");
	command.notes.push_back("${hi | bye} = specific choices, in this case, strings of hi or bye");
	command.notes.push_back("<#time in seconds> = required parameter");
	command.notes.push_back("[#time in seconds] = optional parameter");
	commandlist["help"] = command.addToMap();
#pragma endregion
}

void ChatHandler::showSyntax(Player *player, const string &command, bool fromHelp) {
	if (commandlist.find(command) != commandlist.end()) {
		ChatCommand &cmd = commandlist[command];
		string msg = "Usage: !" + command + " " + cmd.syntax;
		PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
		if (fromHelp) {
			PlayerPacket::showMessage(player, "Notes: " + cmd.notes[0], PlayerPacket::NoticeTypes::Blue);
			for (size_t i = 1; i < cmd.notes.size(); i++) {
				PlayerPacket::showMessage(player, cmd.notes[i], PlayerPacket::NoticeTypes::Blue);
			}
		}
	}
}

void ChatHandler::handleChat(Player *player, PacketReader &packet) {
	string message = packet.getString();
	bool bubbleOnly = packet.getBool(); // Skill macros only display chat bubbles

	if (!ChatHandler::handleCommand(player, message)) {
		PlayersPacket::showChat(player, message, bubbleOnly);
	}
}

bool ChatHandler::handleCommand(Player *player, const string &message) {
	using namespace Functors;

	if (player->isAdmin() && message[0] == '/') {
		// Prevent command printing for Admins
		return true;
	}

	if (player->isGm() && message[0] == '!' && message.size() > 2) {
		char *chat = const_cast<char *>(message.c_str());
		string command = strtok(chat + 1, " ");
		string args = message.length() > command.length() + 2 ? message.substr(command.length() + 2) : "";
		regex re; // Regular expression for use by commands with more complicated structures
		cmatch matches; // Regular expressions match for such commands

		if (commandlist.find(command) == commandlist.end()) {
			PlayerPacket::showMessage(player, "Command \"" + command + "\" does not exist.", PlayerPacket::NoticeTypes::Blue);
		}
		else {
			ChatCommand &cmd = commandlist[command];
			if (player->getGmLevel() < cmd.level) {
				PlayerPacket::showMessage(player, "You are not at a high enough GM level to use the command.", PlayerPacket::NoticeTypes::Blue);
			}
			else {
				switch (cmd.command) {
					case CmdCc:
						re = "(\\d+)";
						if (regex_match(args.c_str(), matches, re)) {
							string targetChannel = matches[1];
							int8_t channel = atoi(targetChannel.c_str()) - 1;
							player->changeChannel(channel);
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdLag:
						re = "(\\w+)";
						if (regex_match(args.c_str(), matches, re)) {
							string target = matches[1];
							if (Player *p = PlayerDataProvider::Instance()->getPlayer(target)) {
								PlayerPacket::showMessage(player, p->getName() + "'s lag: " + lexical_cast<string>(p->getLatency()) + "ms", PlayerPacket::NoticeTypes::Blue);
							}
							else {
								PlayerPacket::showMessage(player, "Player not found.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdOnline: {
						string igns = "IGNs: ";
						int32_t i = 0;
						NameFunctor func = {100, &i, &igns}; // Max of 100, may decide to change this in the future
						PlayerDataProvider::Instance()->run(func);
						PlayerPacket::showMessage(player, igns, PlayerPacket::NoticeTypes::Blue);
						break;
					}
					case CmdHelp: {
						if (args.length() != 0) {
							if (commandlist.find(args) != commandlist.end()) {
								showSyntax(player, args, true);
							}
							else {
								PlayerPacket::showMessage(player, "Requested command \"" + args + "\" does not exist.", PlayerPacket::NoticeTypes::Blue);
							}
						}
						else {
							string msg = "You may not use any commands.";
							bool has = false;
							for (unordered_map<string, ChatCommand>::iterator iter = commandlist.begin(); iter != commandlist.end(); iter++) {
								if (player->getGmLevel() >= iter->second.level) {
									if (!has) {
										msg = "Available commands: ";
										has = true;
									}
									msg += iter->first + " ";
								}
							}
							PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
						}
						break;
					}
					case CmdHeader:
						WorldServerConnectPacket::scrollingHeader(ChannelServer::Instance()->getWorldConnection(), args);
						break;
					case CmdBan:
						re = "(\\w+) ?(\\d+)?";
						if (regex_match(args.c_str(), matches, re)) {
							string targetname = matches[1];
							if (Player *target = PlayerDataProvider::Instance()->getPlayer(targetname)) {
								target->getSession()->disconnect();
							}
							string reasonstring = matches[2];
							int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;

							// Ban account
							mysqlpp::Query accbanquery = Database::getCharDB().query();
							accbanquery << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_reason = " << (int16_t) reason << ", users.ban_expire = '9000-00-00 00:00:00' WHERE characters.name = '" << targetname << "'";
							accbanquery.exec();
								
							int32_t affects = static_cast<int32_t>(accbanquery.affected_rows());
							if (affects != 0) {
								string banmsg = targetname + " has been banned" + getBanString(reason);
								PlayerPacket::showMessageChannel(banmsg, PlayerPacket::NoticeTypes::Notice);
								ChannelServer::Instance()->log(LogTypes::GmCommand, "GM banned a character with reason " + lexical_cast<string>((int16_t)reason) + ". GM: " + player->getName() + ", Character: " + targetname);
							}
							else {
								PlayerPacket::showMessage(player, "Couldn't ban " + string(args) + ". Character not found.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdIpBan:
						re = "(\\w+) ?(\\d+)?";
						if (regex_match(args.c_str(), matches, re)) {
							string targetname = matches[1];
							if (Player *target = PlayerDataProvider::Instance()->getPlayer(targetname)) {
								string targetip = IpUtilities::ipToString(target->getIp());
								target->getSession()->disconnect();

								string reasonstring = matches[2];
								int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;

								// Ip ban
								mysqlpp::Query accipbanquery = Database::getCharDB().query();
								accipbanquery << "INSERT INTO `ipbans`(`id`, `ip`) VALUES (NULL, '" << targetip << "')";
								accipbanquery.exec();

								int32_t affects = static_cast<int32_t>(accipbanquery.affected_rows());
								if (affects != 0) {
									string banmsg = targetname + " has been IP banned" + getBanString(reason);
									PlayerPacket::showMessageChannel(banmsg, PlayerPacket::NoticeTypes::Notice);
									ChannelServer::Instance()->log(LogTypes::GmCommand, "GM IP banned a character with reason " + lexical_cast<string>((int16_t)reason) + ". GM: " + player->getName() + ", Character: " + targetname);
								}
								else {
									PlayerPacket::showMessage(player, "Couldn't IP ban " + string(args) + ". Character not found.", PlayerPacket::NoticeTypes::Red);
								}
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdTempBan:
						re = "(\\w+) (\\d+) (\\d+)";
						if (regex_match(args.c_str(), matches, re)) {
							string targetname = matches[1];
							if (Player *target = PlayerDataProvider::Instance()->getPlayer(targetname))
								target->getSession()->disconnect();

							string reasonstring = matches[2];
							string length = matches[3];
							int8_t reason = reasonstring.length() > 0 ? atoi(reasonstring.c_str()) : 1;

							mysqlpp::Query accbanquery = Database::getCharDB().query();
							accbanquery << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_reason = " << (int16_t) reason << ", users.ban_expire = DATE_ADD(NOW(), INTERVAL " << length << " DAY) WHERE characters.name = '" << targetname << "'";
							accbanquery.exec();

							int32_t affects = static_cast<int32_t>(accbanquery.affected_rows());
							if (affects != 0) {
								string banmsg = targetname + " has been banned" + getBanString(reason);
								PlayerPacket::showMessageChannel(banmsg, PlayerPacket::NoticeTypes::Notice);
								ChannelServer::Instance()->log(LogTypes::GmCommand, "GM temporary banned a character with reason " + lexical_cast<string>((int16_t)reason) + " for " + length + " days. GM: " + player->getName() + ", Character: " + targetname);
							}
							else {
								PlayerPacket::showMessage(player, "Couldn't temporary ban " + string(args) + ". Character not found.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdUnban:
						if (args.length() != 0) {
							mysqlpp::Query accbanquery = Database::getCharDB().query();
							accbanquery << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.ban_expire = '0000-00-00 00:00:00' WHERE characters.name = '" << args << "'";
							accbanquery.exec();

							int32_t affects = static_cast<int32_t>(accbanquery.affected_rows());
							if (affects != 0) {
								PlayerPacket::showMessage(player, string(args) + " has been unbanned.", PlayerPacket::NoticeTypes::Blue);
								ChannelServer::Instance()->log(LogTypes::GmCommand, "GM unbanned a character. GM: " + player->getName() + ", Character: " + args);
							}
							else {
								PlayerPacket::showMessage(player, "Couldn't unban " + string(args) + ". Character not found.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdShutdown:
						PlayerPacket::showMessage(player, "Shutting down the server.", PlayerPacket::NoticeTypes::Blue);
						ChannelServer::Instance()->log(LogTypes::GmCommand, "GM shutdown the server. GM: " + player->getName());
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
							int32_t time = atoi(args.c_str());
							string msg = "Stopped map timer.";
							if (time > 0) {
								int32_t seconds = time /= 60;
								int32_t minutes = time /= 60;
								int32_t hours = time /= 60;
								msg = "Started map timer. Counting down from ";
								if (hours > 0) {
									msg += lexical_cast<string>(hours) + " hours";
								}
								if (minutes > 0) {
									if (hours > 0) {
										msg += ", ";
									}
									msg += lexical_cast<string>(minutes) + " minutes";
								}
								if (seconds > 0) {
									if (hours > 0 || minutes > 0) {
										msg += " and ";
									}
									msg += lexical_cast<string>(seconds) + " seconds";
								}
								msg += "!";
							}
							PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
							Maps::getMap(player->getMap())->setMapTimer(time);
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
							PlayerPacket::showMessage(player, "Showing instruction bubble to everyone on the map.", PlayerPacket::NoticeTypes::Blue);
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdAddNpc:
						if (args.length() != 0) {
							NpcSpawnInfo npc;
							npc.id = atoi(args.c_str());
							npc.foothold = 0;
							npc.pos = player->getPos();
							npc.rx0 = npc.pos.x - 50;
							npc.rx1 = npc.pos.x + 50;
							int32_t id = Maps::getMap(player->getMap())->addNpc(npc);
							PlayerPacket::showMessage(player, "Spawned NPC with Object ID " + lexical_cast<string>(id) , PlayerPacket::NoticeTypes::Blue);
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdMe:
						if (args.length() != 0) {
							string msg = player->getName() + " : " + args;
							MeFunctor func = {msg};
							PlayerDataProvider::Instance()->run(func);
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdKick:
						if (args.length() != 0) {
							if (Player *target = PlayerDataProvider::Instance()->getPlayer(args)) {
								if (player->getGmLevel() > target->getGmLevel()) {
									target->getSession()->disconnect();
									PlayerPacket::showMessage(player, "Kicked " + args + " from the server.", PlayerPacket::NoticeTypes::Blue);
								}
								else {
									PlayerPacket::showMessage(player, "Player outranks you.", PlayerPacket::NoticeTypes::Red);
								}
							}
							else {
								PlayerPacket::showMessage(player, "Invalid player or player is offline.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdWarp:
						re = "(\\w+) ?(\\d*)?";
						if (regex_match(args.c_str(), matches, re)) {
							if (Player *warpee = PlayerDataProvider::Instance()->getPlayer(matches[1])) {
								string mapstring = matches[2];
								int32_t mapid;
								if (mapstring.length() > 0) {
									mapid = getMap(mapstring, player);
								}
								else {
									mapid = player->getMap();
								}

								if (Maps::getMap(mapid)) {
									warpee->setMap(mapid);
									PlayerPacket::showMessage(player, "Warped " + string(matches[1]) + " to mapid " + lexical_cast<string>(mapid) + ".", PlayerPacket::NoticeTypes::Blue);
								}
								else {
									PlayerPacket::showMessage(player, "Cannot warp player; invalid Map ID given.", PlayerPacket::NoticeTypes::Red);
								}
							}
							else {
								PlayerPacket::showMessage(player, string(matches[1]) + " not found for warping.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdWarpAll: {
						int32_t mapid;
						if (args.length() > 0) {
							mapid = getMap(args, player);
						}
						else {
							mapid = player->getMap();
						}

						if (Maps::getMap(mapid)) {
							WarpFunctor func = {mapid, player};
							PlayerDataProvider::Instance()->run(func);
							if (args.length() > 0) {
								PlayerPacket::showMessage(player, "Warped everyone in the server to mapid " + lexical_cast<string>(mapid) + ".", PlayerPacket::NoticeTypes::Blue);
							}
							else {
								PlayerPacket::showMessage(player, "Warped everyone in the server to yourself.", PlayerPacket::NoticeTypes::Blue);
							}
						}
						else {
							PlayerPacket::showMessage(player, "Cannot warp players; invalid Map ID given (map not found).", PlayerPacket::NoticeTypes::Red);
						}
						break;
					}
					case CmdWarpMap: {
						int32_t mapid;
						if (args.length() > 0) {
							mapid = getMap(args, player);
						}
						else {
							mapid = player->getMap();
						}

						if (Map *map = Maps::getMap(mapid)) {
							WarpFunctor func = {mapid, player};
							Maps::getMap(player->getMap())->runFunctionPlayers(func);
							if (args.length() > 0) {
								PlayerPacket::showMessage(player, "Warped everyone in the map to mapid " + lexical_cast<string>(mapid) + ".", PlayerPacket::NoticeTypes::Blue);
							}
							else {
								PlayerPacket::showMessage(player, "Warped everyone in the map to yourself.", PlayerPacket::NoticeTypes::Blue);
							}
						}
						else {
							PlayerPacket::showMessage(player, "Cannot warp players; invalid Map ID given (map not found).", PlayerPacket::NoticeTypes::Red);
						}
						break;
					}
					case CmdKillAll: {
						int32_t killed = Maps::getMap(player->getMap())->killMobs(player);
						PlayerPacket::showMessage(player, "Killed " + lexical_cast<string>(killed) + " mobs!", PlayerPacket::NoticeTypes::Blue);
						break;
					}
					case CmdClearDrops:
						Maps::getMap(player->getMap())->clearDrops();
						break;
					case CmdKill:
						if (player->getGmLevel() == 1) {
							player->getStats()->setHp(0);
							PlayerPacket::showMessage(player, "Killed yourself.", PlayerPacket::NoticeTypes::Blue);
						}
						else {
							if (args == "all") {
								int32_t kills = 0;
								for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
									Player *killpsa = Maps::getMap(player->getMap())->getPlayer(i);
									if (killpsa != player) {
										killpsa->getStats()->setHp(0);
										kills++;
									}
								}
								PlayerPacket::showMessage(player, "Killed " + lexical_cast<string>(kills) + " players in the current map!", PlayerPacket::NoticeTypes::Blue);
							}
							else if (args == "gm") {
								int32_t kills = 0;
								for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
									Player *killpsa = Maps::getMap(player->getMap())->getPlayer(i);
									if (killpsa != player) {
										if (killpsa->isGm()) {
											killpsa->getStats()->setHp(0);
											kills++;
										}
									}
								}
								PlayerPacket::showMessage(player, "Killed " + lexical_cast<string>(kills) + "  GMs in the current map!", PlayerPacket::NoticeTypes::Blue);
							}
							else if (args == "players") {
								int32_t kills = 0;
								for (size_t i = 0; i < Maps::getMap(player->getMap())->getNumPlayers(); i++) {
									Player *killpsa = Maps::getMap(player->getMap())->getPlayer(i);
									if (killpsa != player) {
										if (!killpsa->isGm()) {
											killpsa->getStats()->setHp(0);
											kills++;
										}
									}
								}
								PlayerPacket::showMessage(player, "Killed " + lexical_cast<string>(kills) + "  players in the current map!", PlayerPacket::NoticeTypes::Blue);
							}
							else if (args == "me") {
								player->getStats()->setHp(0);
								PlayerPacket::showMessage(player, "Killed yourself.", PlayerPacket::NoticeTypes::Blue);
							}
							else if (Player *killpsa = PlayerDataProvider::Instance()->getPlayer(args)) {
								// Kill by name
								killpsa->getStats()->setHp(0);
								PlayerPacket::showMessage(player, "Killed " + args, PlayerPacket::NoticeTypes::Blue);
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
							else if (matches[1] == "scriptbyname") type = 300;
							else if (matches[1] == "scriptbyid") type = 400;

							if (type != 0) {
								mysqlpp::Query query = Database::getDataDB().query();
								mysqlpp::StoreQueryResult res;
								if (type < 200) {
									if (type == 100) {
										query << "SELECT objectid, `label` FROM strings WHERE objectid = " << mysqlpp::quote << (string)matches[2];
									}
									else {
										query << "SELECT objectid, `label` FROM strings WHERE object_type = " << type << " AND label LIKE '%" << (string)matches[2] << "%'";
									}

									res = query.store();

									if (res.num_rows() == 0) {
										PlayerPacket::showMessage(player, "No results.", PlayerPacket::NoticeTypes::Red);
									}
									else {
										for (size_t i = 0; i < res.num_rows(); i++) {
											string msg = (string) res[i][0] + " : " + (string) res[i][1];
											PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
										}
									}
								}
								else if (type == 200) {
									int32_t mapid = getMap(matches[2], player);
									if (Maps::getMap(mapid) != nullptr) {
										string message = lexical_cast<string>(mapid) + " : " + lexical_cast<string>((int32_t)(MapDataProvider::Instance()->getContinent(mapid)));
										PlayerPacket::showMessage(player, message, PlayerPacket::NoticeTypes::Blue);
									}
									else {
										PlayerPacket::showMessage(player, "Invalid map", PlayerPacket::NoticeTypes::Red);
									}
								}
								else if (type > 200) {
									if (type == 300) {
										query << "SELECT script_type, objectid, script FROM scripts WHERE script LIKE '%" << (string)matches[2] << "%'";
									}
									else if (type == 400) {
										query << "SELECT script_type, objectid, script FROM scripts WHERE objectid = " << mysqlpp::quote << (string)matches[2];
									}
									res = query.store();

									if (res.num_rows() == 0) {
										PlayerPacket::showMessage(player, "No results.", PlayerPacket::NoticeTypes::Red);
									}
									else {
										for (size_t i = 0; i < res.num_rows(); i++) {
											string msg = (string) res[i][1] + " (" + (string) res[i][0] + "): " + (string) res[i][2];
											PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
										}
									}
								}
							}
							else {
								PlayerPacket::showMessage(player, "Invalid search type - valid options are: {item, skill, map, mob, npc, quest, continent, id, scriptbyname, scriptbyid}", PlayerPacket::NoticeTypes::Red);
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
								PlayerPacket::showMessage(player, "Map not found.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							PlayerPacket::showMessage(player, "Current Map: " + lexical_cast<string>(player->getMap()), PlayerPacket::NoticeTypes::Blue);
						}
						break;
					}
					case CmdNpc: {
						if (args.length() != 0) {
							int32_t npcid = atoi(args.c_str());
							Npc *npc = new Npc(npcid, player);
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
							if (SkillDataProvider::Instance()->isSkill(skillid)) {
								// Don't allow skills that do not exist to be added
								string countstring = matches[2];
								uint8_t count = countstring.length() > 0 ? atoi(countstring.c_str()) : 1;

								player->getSkills()->addSkillLevel(skillid, count);
								if (count < 0) {
									PlayerPacket::showMessage(player, "Removed " + lexical_cast<string>((int16_t) count) + " skill points from skillid " + lexical_cast<string>(skillid), PlayerPacket::NoticeTypes::Blue);
								}
								else {
									PlayerPacket::showMessage(player, "Added " + lexical_cast<string>((int16_t) count) + " skill points to skillid " + lexical_cast<string>(skillid), PlayerPacket::NoticeTypes::Blue);
								}
							}
							else {
								PlayerPacket::showMessage(player, "Invalid Skill ID.", PlayerPacket::NoticeTypes::Red);
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
								if (count != 0) {
									int32_t objectId = Maps::getMap(player->getMap())->spawnMob(mobid, player->getPos());
									for (int32_t i = 1; i < count && i < 100; i++) {
										Maps::getMap(player->getMap())->spawnMob(mobid, player->getPos());
									}

									string msg;
									if (count > 1) {
										msg = "Spawned " + lexical_cast<string>(count) + " mobs with ID " + lexical_cast<string>(mobid) + ".";
										msg += " The spawned mobs Object ID range is " + lexical_cast<string>(objectId) + " - " + lexical_cast<string>(objectId + count) + ".";
									}
									else {
										msg = "Spawned 1 mob with ID " + lexical_cast<string>(mobid) + " and Object ID " + lexical_cast<string>(objectId) + ".";
									}
									PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
								}
								else {
									PlayerPacket::showMessage(player, "No mobs spawned.", PlayerPacket::NoticeTypes::Red);
								}
							}
							else {
								PlayerPacket::showMessage(player, "Invalid Mob ID.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					}
					case CmdNotice:
						if (args.length() != 0) {
							PlayerPacket::showMessageChannel(args, PlayerPacket::NoticeTypes::Notice);
						}
						break;
					case CmdMaxStats:
						player->getStats()->setFame(Stats::MaxFame);
						player->getStats()->setMaxHp(Stats::MaxMaxHp);
						player->getStats()->setMaxMp(Stats::MaxMaxMp);
						player->getStats()->setStr(32767);
						player->getStats()->setDex(32767);
						player->getStats()->setInt(32767);
						player->getStats()->setLuk(32767);
						PlayerPacket::showMessage(player, "Maxed every stat for this character.", PlayerPacket::NoticeTypes::Blue);
						break;
					case CmdStr:
						if (args.length() != 0) {
							player->getStats()->setStr(atoi(args.c_str()));
						}
						break;
					case CmdDex:
						if (args.length() != 0) {
							player->getStats()->setDex(atoi(args.c_str()));
						}
						break;
					case CmdLuk:
						if (args.length() != 0) {
							player->getStats()->setLuk(atoi(args.c_str()));
						}
						break;
					case CmdInt:
						if (args.length() != 0) {
							player->getStats()->setInt(atoi(args.c_str()));
						}
						break;
					case CmdHp:
						if (args.length() != 0) {
							uint16_t amount = atoi(args.c_str());
							player->getStats()->setMaxHp(amount);
							if (player->getStats()->getHp() > amount) {
								player->getStats()->setHp(player->getStats()->getMaxHp());
							}
						}
						break;
					case CmdMp:
						if (args.length() != 0) {
							uint16_t amount = atoi(args.c_str());
							player->getStats()->setMaxMp(amount);
							if (player->getStats()->getMp() > amount) {
								player->getStats()->setMp(player->getStats()->getMaxMp());
							}
						}
						break;
					case CmdFame:
						if (args.length() != 0) {
							player->getStats()->setFame(atoi(args.c_str()));
						}
						break;
					case CmdReload:
						if (args.length() != 0) {
							if (args == "items" || args == "drops" || args == "shops" ||
								args == "mobs" || args == "beauty" || args == "scripts" ||
								args == "skills" || args == "reactors" || args == "pets" ||
								args == "quests" || args == "all") {
								WorldServerConnectPacket::reloadMcdb(ChannelServer::Instance()->getWorldConnection(), args);
								PlayerPacket::showMessage(player, "Reloading message for " + args + " sent to all channels.", PlayerPacket::NoticeTypes::Blue);
							}
							else if (args == "cashshop") {
								WorldServerConnectPacket::reloadCashServers(ChannelServer::Instance()->getWorldConnection());
								PlayerPacket::showMessage(player, "Reloading message sent to all cash servers.", PlayerPacket::NoticeTypes::Blue);
							}
							else {
								PlayerPacket::showMessage(player, "Invalid reload type.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdShop:
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

							if (!NpcHandler::showShop(player, shopid)) {
								showSyntax(player, command);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdPos: {
						Pos p = player->getPos();
						string msg = "(FH, X, Y): (";
						msg += lexical_cast<string>(player->getFh()) + ", ";
						msg += lexical_cast<string>(p.x) + ", ";
						msg += lexical_cast<string>(p.y) + ")";
						PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
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
								PlayerPacket::showMessage(player, "Invalid Item ID.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					}
					case CmdLevel:
						if (args.length() != 0) {
							player->getStats()->setLevel(atoi(args.c_str()));
						}
						break;
					case CmdJob: {
						if (args.length() != 0) {
							int16_t job = getJob(args);
							if (job >= 0) {
								player->getStats()->setJob(job);
							}
						}
						else {
							string msg = "Current Job: " + lexical_cast<string>(player->getStats()->getJob());
							PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);
						}
						break;
					}
					case CmdAp:
						if (args.length() != 0) {
							player->getStats()->setAp(atoi(args.c_str()));
						}
						break;
					case CmdSp:
						if (args.length() != 0) {
							player->getStats()->setSp(atoi(args.c_str()));
						}
						break;
					case CmdKillNpc:
						player->setNpc(nullptr);
						break;
					case CmdHorntail:
						Maps::getMap(player->getMap())->spawnMob(Mobs::SummonHorntail, player->getPos());
						ChannelServer::Instance()->log(LogTypes::GmCommand, "Player spawned horntail on map " + lexical_cast<string>(player->getMap()) + ". Name: " + player->getName());
						break;
					case CmdHeal:
						player->getStats()->setHp(player->getStats()->getMaxHp());
						player->getStats()->setMp(player->getStats()->getMaxMp());
						break;
					case CmdMesos:
						if (args.length() != 0) {
							player->getInventory()->setMesos(atoi(args.c_str()));
						}
						break;
					case CmdRelog:
						player->changeChannel((int8_t)ChannelServer::Instance()->getChannel());
						break;
					case CmdSave:
						player->saveAll();
						PlayerPacket::showMessage(player, "Your progress has been saved.", PlayerPacket::NoticeTypes::Blue);
						break;
					case CmdWarpTo:
						if (args.length() > 0) {
							if (Player *warptoee = PlayerDataProvider::Instance()->getPlayer(args)) {
								player->setMap(warptoee->getMap());
							}
							else {
								PlayerPacket::showMessage(player, "Player not found: " + args, PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdZakum:
						Maps::getMap(player->getMap())->spawnZakum(player->getPos());
						ChannelServer::Instance()->log(LogTypes::GmCommand, "Player spawned zakum on map " + lexical_cast<string>(player->getMap()) + ". Name: " + player->getName());
						break;
					case CmdMusic:
						Maps::getMap(player->getMap())->setMusic(args);
						PlayerPacket::showMessage(player, "Set music on the map to: " + args, PlayerPacket::NoticeTypes::Blue);
						break;
					case CmdDisconnect:
						player->getSession()->disconnect();
						break;
					case CmdEventInstruction:
						MapPacket::showEventInstructions(player->getMap());
						break;
					case CmdStorage:
						NpcHandler::showStorage(player, 1012009);
						break;
					case CmdRankingCalc:
						WorldServerConnectPacket::rankingCalculation(ChannelServer::Instance()->getWorldConnection());
						PlayerPacket::showMessage(player, "Sent a signal to force the calculation of rankings.", PlayerPacket::NoticeTypes::Blue);
						break;
					case CmdWorldMessage:
						re = "(\\w+) (.+)";
						if (regex_match(args.c_str(), matches, re)) {
							int8_t type = getMessageType((string) matches[1]);
							if (type != -1) {
								PlayerPacket::showMessageWorld((string) matches[2], type);
							}
							else {
								PlayerPacket::showMessage(player, "Invalid message type - valid options are: {notice, box, red, blue}", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdGlobalMessage:
						re = "(\\w+) (.+)";
						if (regex_match(args.c_str(), matches, re)) {
							int8_t type = getMessageType((string) matches[1]);
							if (type != -1) {
								PlayerPacket::showMessageGlobal((string) matches[2], type);
							}
							else {
								PlayerPacket::showMessage(player, "Invalid message type - valid options are: {notice, box, red, blue}", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
					case CmdListMobs: {
						string message = "No mobs on the current map.";
						if (Maps::getMap(player->getMap())->countMobs(0) > 0) {
							typedef unordered_map<int32_t, Mob *> mobmap;
							mobmap mobs = Maps::getMap(player->getMap())->getMobs();
							for (mobmap::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
								message = "Mob ";
								message += lexical_cast<string>(iter->first);
								message += " (ID: ";
								message += lexical_cast<string>(iter->second->getMobId());
								message += ", HP: ";
								message += lexical_cast<string>(iter->second->getHp());
								message += "/";
								message += lexical_cast<string>(iter->second->getMaxHp());
								message += ")";
								PlayerPacket::showMessage(player, message, PlayerPacket::NoticeTypes::Blue);
							}
						}
						else {
							PlayerPacket::showMessage(player, message, PlayerPacket::NoticeTypes::Red);
						}
						break;
					}
					case CmdGetMobHp: {
						string message = "Mob does not exist.";
						if (args.length() != 0) {
							int32_t mobid = atoi(args.c_str());
							Mob *mob = Maps::getMap(player->getMap())->getMob(mobid);
							if (mob != nullptr) {
								message = "Mob ";
								message += lexical_cast<string>(mobid);
								message += " HP: ";
								message += lexical_cast<string>(mob->getHp());
								message += "/";
								message += lexical_cast<string>(mob->getMaxHp());
								message += " (";
								message += lexical_cast<string>(static_cast<int64_t>(mob->getHp()) * 100 / mob->getMaxHp());
								message += "%)";
							}
							PlayerPacket::showMessage(player, message, PlayerPacket::NoticeTypes::Blue);
						}
						else {
							PlayerPacket::showMessage(player, message, PlayerPacket::NoticeTypes::Red);
						}
						break;
					}
					case CmdKillMob:
						if (args.length() != 0) {
							int32_t mobid = atoi(args.c_str());
							Mob *mob = Maps::getMap(player->getMap())->getMob(mobid);
							if (mob != nullptr) {
								PlayerPacket::showMessage(player, "Killed mob with Object ID " + args + ". HP Left till death: " + lexical_cast<string>(mob->getMaxHp() - mob->getHp()) + ".", PlayerPacket::NoticeTypes::Blue);
								mob->applyDamage(player->getId(), mob->getHp());
							}
							else {
								PlayerPacket::showMessage(player, "Couldn't kill mob. Mob with Object ID " + args + " not found.", PlayerPacket::NoticeTypes::Red);
							}
						}
						else {
							showSyntax(player, command);
						}
						break;
				}
			}
		}
		return true;
	}
	return false;
}

int32_t ChatHandler::getMap(const string &query, Player *player) {
	int32_t mapid = -1;
	// Special
	if (query == "here") mapid = player->getMap();
	else if (query == "town") mapid = Maps::getMap(player->getMap())->getReturnMap();
	else if (query == "gm") mapid = Maps::GmMap;
	else if (query == "fm") mapid = 910000000;
	else if (query == "4th") mapid = 240010501;
	else if (query == "3rd") mapid = 211000001;
	else if (query == "grendel") mapid = 101000003;
	else if (query == "athena") mapid = 100000201;
	else if (query == "darklord") mapid = 103000003;
	else if (query == "danceswb") mapid = 102000003;
	else if (query == "kyrin") mapid = 120000101;
	else if (query == "happyville") mapid = 209000000;
	// Maple Island
	else if (query == "southperry") mapid = 60000;
	else if (query == "amherst") mapid = 1010000;
	// Victoria
	else if (query == "henesys") mapid = 100000000;
	else if (query == "perion") mapid = 102000000;
	else if (query == "sharenian") mapid = 101030104;
	else if (query == "ellinia") mapid = 101000000;
	else if (query == "sleepywood") mapid = 105040300;
	else if (query == "lith") mapid = 104000000;
	else if (query == "florina") mapid = 110000000;
	else if (query == "kerning") mapid = 103000000;
	else if (query == "port") mapid = 120000000;
	// Ossyria
	else if (query == "orbis") mapid = 200000000;
	else if (query == "nath") mapid = 211000000;
	else if (query == "leafre") mapid = 240000000;
	else if (query == "mulung") mapid = 250000000;
	else if (query == "herbtown") mapid = 251000000;
	else if (query == "ariant") mapid = 260000000;
	else if (query == "magatia") mapid = 261000000;
	// Ludus Lake area
	else if (query == "ludi") mapid = 220000000;
	else if (query == "altaire") mapid = 300000000;
	else if (query == "kft") mapid = 222000000;
	else if (query == "aqua") mapid = 230000000;
	else if (query == "omega") mapid = 221000000;
	// Floating areas/not officially a part of other continents
	else if (query == "temple") mapid = 270000000;
	else if (query == "ereve") mapid = 130000200;
	else if (query == "rien") mapid = 140000000;
	// Masteria
	else if (query == "nlc") mapid = 600000000;
	else if (query == "amoria") mapid = 680000000;
	else if (query == "crimsonwood") mapid = 610020006;
	// Dungeon areas
	else if (query == "armory") mapid = 801040004;
	else if (query == "mansion") mapid = 682000000;
	else if (query == "dungeon") mapid = 105090200;
	else if (query == "mine") mapid = 211041400;
	// World Tour
	else if (query == "singapore") mapid = 540000000;
	else if (query == "quay") mapid = 541000000;
	else if (query == "malaysia") mapid = 550000000;
	else if (query == "kampung") mapid = 551000000;
	else if (query == "shrine") mapid = 800000000;
	else if (query == "showa") mapid = 801000000;
	// Area boss maps
	else if (query == "manon") mapid = 240020401;
	else if (query == "griffey") mapid = 240020101;
	else if (query == "jrbalrog") mapid = 105090900;
	else if (query == "anego") mapid = 801040003;
	else if (query == "tengu") mapid = 800020130;
	else if (query == "lilynouch") mapid = 270020500;
	else if (query == "dodo") mapid = 270010500;
	else if (query == "lyka") mapid = 270030500;
	// PQ boss maps
	else if (query == "ergoth") mapid = 990000900;
	else if (query == "lordpirate") mapid = 925100500;
	else if (query == "alishar") mapid = 922010900;
	else if (query == "papapixie") mapid = 920010800;
	else if (query == "kingslime") mapid = 103000804;
	else if (query == "dunes") mapid = 926010000;
	// Boss maps
	else if (query == "pap") mapid = 220080001;
	else if (query == "zakum") mapid = 280030000;
	else if (query == "horntail") mapid = 240060200;
	else if (query == "pianus") mapid = 230040420;
	else if (query == "grandpa") mapid = 801040100;
	else if (query == "bean") mapid = 270050100;
	else {
		char *endptr;
		mapid = strtol(query.c_str(), &endptr, 0);
		if (strlen(endptr) != 0) mapid = -1;
	}
	return mapid;
}

int16_t ChatHandler::getJob(const string &query) {
	int16_t job = -1;
	if (query == "beginner") job = Jobs::JobIds::Beginner;
	else if (query == "warrior") job = Jobs::JobIds::Swordsman;
	else if (query == "fighter") job = Jobs::JobIds::Fighter;
	else if (query == "sader") job = Jobs::JobIds::Crusader;
	else if (query == "hero") job = Jobs::JobIds::Hero;
	else if (query == "page") job = Jobs::JobIds::Page;
	else if (query == "wk") job = Jobs::JobIds::WhiteKnight;
	else if (query == "paladin") job = Jobs::JobIds::Paladin;
	else if (query == "spearman") job = Jobs::JobIds::Spearman;
	else if (query == "dk") job  = Jobs::JobIds::DragonKnight;
	else if (query == "drk") job = Jobs::JobIds::DarkKnight;
	else if (query == "magician") job = Jobs::JobIds::Magician;
	else if (query == "fpwiz") job = Jobs::JobIds::FPWizard;
	else if (query == "fpmage") job = Jobs::JobIds::FPMage;
	else if (query == "fparch") job = Jobs::JobIds::FPArchMage;
	else if (query == "ilwiz") job = Jobs::JobIds::ILWizard;
	else if (query == "ilmage") job = Jobs::JobIds::ILMage;
	else if (query == "ilarch") job = Jobs::JobIds::ILArchMage;
	else if (query == "cleric") job = Jobs::JobIds::Cleric;
	else if (query == "priest") job = Jobs::JobIds::Priest;
	else if (query == "bishop") job = Jobs::JobIds::Bishop;
	else if (query == "bowman") job = Jobs::JobIds::Archer;
	else if (query == "hunter") job = Jobs::JobIds::Hunter;
	else if (query == "ranger") job = Jobs::JobIds::Ranger;
	else if (query == "bm") job = Jobs::JobIds::Bowmaster;
	else if (query == "xbowman") job = Jobs::JobIds::Crossbowman;
	else if (query == "sniper") job = Jobs::JobIds::Sniper;
	else if (query == "marksman") job = Jobs::JobIds::Marksman;
	else if (query == "thief") job = Jobs::JobIds::Rogue;
	else if (query == "sin") job = Jobs::JobIds::Assassin;
	else if (query == "hermit") job = Jobs::JobIds::Hermit;
	else if (query == "nl") job = Jobs::JobIds::NightLord;
	else if (query == "dit") job = Jobs::JobIds::Bandit;
	else if (query == "cb") job = Jobs::JobIds::ChiefBandit;
	else if (query == "shadower") job = Jobs::JobIds::Shadower;
	else if (query == "pirate") job = Jobs::JobIds::Pirate;
	else if (query == "brawler") job = Jobs::JobIds::Brawler;
	else if (query == "marauder") job = Jobs::JobIds::Marauder;
	else if (query == "buccaneer") job = Jobs::JobIds::Buccaneer;
	else if (query == "gunslinger") job = Jobs::JobIds::Gunslinger;
	else if (query == "outlaw") job = Jobs::JobIds::Outlaw;
	else if (query == "corsair") job = Jobs::JobIds::Corsair;
	else if (query == "gm") job = Jobs::JobIds::Gm;
	else if (query == "sgm") job = Jobs::JobIds::SuperGm;
	else if (query == "noblesse") job = Jobs::JobIds::Noblesse;
	else if (query == "dawn1") job = Jobs::JobIds::DawnWarrior1;
	else if (query == "dawn2") job = Jobs::JobIds::DawnWarrior2;
	else if (query == "dawn3") job = Jobs::JobIds::DawnWarrior3;
	else if (query == "dawn4") job = Jobs::JobIds::DawnWarrior4;
	else if (query == "blaze1") job = Jobs::JobIds::BlazeWizard1;
	else if (query == "blaze2") job = Jobs::JobIds::BlazeWizard2;
	else if (query == "blaze3") job = Jobs::JobIds::BlazeWizard3;
	else if (query == "blaze4") job = Jobs::JobIds::BlazeWizard4;
	else if (query == "wind1") job = Jobs::JobIds::WindArcher1;
	else if (query == "wind2") job = Jobs::JobIds::WindArcher2;
	else if (query == "wind3") job = Jobs::JobIds::WindArcher3;
	else if (query == "wind4") job = Jobs::JobIds::WindArcher4;
	else if (query == "night1") job = Jobs::JobIds::NightWalker1;
	else if (query == "night2") job = Jobs::JobIds::NightWalker2;
	else if (query == "night3") job = Jobs::JobIds::NightWalker3;
	else if (query == "night4") job = Jobs::JobIds::NightWalker4;
	else if (query == "thunder1") job = Jobs::JobIds::ThunderBreaker1;
	else if (query == "thunder2") job = Jobs::JobIds::ThunderBreaker2;
	else if (query == "thunder3") job = Jobs::JobIds::ThunderBreaker3;
	else if (query == "thunder4") job = Jobs::JobIds::ThunderBreaker4;
	else job = atoi(query.c_str());
	return job;
}

string ChatHandler::getBanString(int8_t reason) {
	string banmsg = ".";
	switch (reason) {
		case 0x01: banmsg = " for hacking."; break;
		case 0x02: banmsg = " for using macro/auto-keyboard."; break;
		case 0x03: banmsg = " for illicit promotion or advertising."; break;
		case 0x04: banmsg = " for harassment."; break;
		case 0x05: banmsg = " for using profane language."; break;
		case 0x06: banmsg = " for scamming."; break;
		case 0x07: banmsg = " for misconduct."; break;
		case 0x08: banmsg = " for illegal cash transaction."; break;
		case 0x09: banmsg = " for illegal charging/funding."; break;
		case 0x0A: banmsg = " for temporary request."; break;
		case 0x0B: banmsg = " for impersonating GM."; break;
		case 0x0C: banmsg = " for using illegal programs or violating the game policy."; break;
		case 0x0D: banmsg = " for one of cursing, scamming, or illegal trading via Megaphones."; break;
	}
	return banmsg;
}

int8_t ChatHandler::getMessageType(const string &query) {
	int8_t ret = -1;
	if (query == "notice") ret = PlayerPacket::NoticeTypes::Notice;
	else if (query == "box") ret = PlayerPacket::NoticeTypes::Box;
	else if (query == "red") ret = PlayerPacket::NoticeTypes::Red;
	else if (query == "blue") ret = PlayerPacket::NoticeTypes::Blue;
	return ret;
}

void ChatHandler::handleGroupChat(Player *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	uint8_t amount = packet.get<uint8_t>();
	vector<int32_t> receivers = packet.getVector<int32_t>(amount);
	string chat = packet.getString();

	if (!ChatHandler::handleCommand(player, chat)) {
		WorldServerConnectPacket::groupChat(ChannelServer::Instance()->getWorldConnection(), type, player->getId(), receivers, chat);
	}
}
