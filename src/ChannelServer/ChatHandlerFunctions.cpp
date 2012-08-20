/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "ChatHandlerFunctions.h"
#include "CustomFunctions.h"
#include "InfoFunctions.h"
#include "JobConstants.h"
#include "MapFunctions.h"
#include "Maps.h"
#include "ManagementFunctions.h"
#include "MessageFunctions.h"
#include "Player.h"
#include "PlayerModFunctions.h"
#include "PlayerPacket.h"
#include "WarpFunctions.h"

CommandListType ChatHandlerFunctions::CommandList;

void ChatHandlerFunctions::initialize() {
	// Set up commands and appropriate GM levels
	ChatCommand command;

	// Notes:
	// Don't add syntax to things that have no parameters
	// Every command needs at least one line of notes that describes what the command does

	#pragma region GM Level 3
	command.level = 3;

	command.command = &ManagementFunctions::ban;
	command.syntax = "<$player name> [#reason]";
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
	CommandList["ban"] = command.addToMap();

	command.command = &ManagementFunctions::ipBan;
	command.syntax = "<$player name> [#reason]";
	command.notes.push_back("Permanently bans a player's IP based on their name. Does not ban the account for various reasons.");
	command.notes.push_back("Use !help ban to see the applicable reason codes.");
	CommandList["ipban"] = command.addToMap();

	command.command = &ManagementFunctions::tempBan;
	command.syntax = "<$player name> <#reason> <#length in days>";
	command.notes.push_back("Temporarily bans a player by name.");
	command.notes.push_back("Use !help ban to see the applicable reason codes.");
	CommandList["tempban"] = command.addToMap();

	command.command = &ManagementFunctions::unban;
	command.syntax = "<$player name>";
	command.notes.push_back("Removes a ban from the database.");
	CommandList["unban"] = command.addToMap();

	command.command = &ManagementFunctions::header;
	command.syntax = "[$message]";
	command.notes.push_back("Changes the scrolling message at the top of the screen.");
	CommandList["header"] = command.addToMap();

	command.command = &ManagementFunctions::shutdown;
	command.notes.push_back("Stops the current ChannelServer.");
	CommandList["shutdown"] = command.addToMap();

	command.command = &MapFunctions::timer;
	command.syntax = "<#time in seconds>";
	command.notes.push_back("Displays a timer at the top of the map.");
	CommandList["timer"] = command.addToMap();

	command.command = &MapFunctions::instruction;
	command.syntax = "<$bubble text>";
	command.notes.push_back("Displays a bubble. With shiny text. Somewhat useless for managing players.");
	CommandList["instruction"] = command.addToMap();

	command.command = &ManagementFunctions::addNpc;
	command.syntax = "<#npc ID>";
	command.notes.push_back("Permanently adds an NPC to a map.");
	CommandList["addnpc"] = command.addToMap();

	command.command = &ManagementFunctions::calculateRanks;
	command.notes.push_back("Forces ranking recalculation.");
	CommandList["dorankings"] = command.addToMap();

	command.command = &MessageFunctions::globalMessage;
	command.syntax = "<${notice | box | red | blue}> <$message>";
	command.notes.push_back("Displays a message to every channel on every world.");
	CommandList["globalmessage"] = command.addToMap();
	#pragma endregion

	#pragma region GM Level 2
	command.level = 2;

	command.command = &MessageFunctions::gmMessage;
	command.syntax = "<$message>";
	command.notes.push_back("Displays a message to all other online GMs.");
	CommandList["me"] = command.addToMap();

	command.command = &ManagementFunctions::kick;
	command.syntax = "<$player name>";
	command.notes.push_back("Forcibly disconnects a player, cannot be used on players that outrank you in GM level.");
	CommandList["kick"] = command.addToMap();

	command.command = &WarpFunctions::warp;
	command.syntax = "<$player name> [#map ID]";
	command.notes.push_back("Warps the specified player to your map or the map you specify.");
	CommandList["warp"] = command.addToMap();

	command.command = &WarpFunctions::warpAll;
	command.syntax = "[#map ID]";
	command.notes.push_back("Warps all players to your map or the map you specify.");
	CommandList["warpall"] = command.addToMap();

	command.command = &WarpFunctions::warpMap;
	command.syntax = "[#map ID]";
	command.notes.push_back("Warps all players in your current map to your map or the map you specify.");
	CommandList["warpmap"] = command.addToMap();

	command.command = &MapFunctions::killAllMobs;
	command.notes.push_back("Kills all mobs on the current map.");
	CommandList["killall"] = command.addToMap();

	command.command = &MapFunctions::clearDrops;
	command.notes.push_back("Clears all drops from the current map.");
	CommandList["cleardrops"] = command.addToMap();

	command.command = &MessageFunctions::worldMessage;
	command.syntax = "<${notice | box | red | blue}> <$message>";
	command.notes.push_back("Displays a message to every channel on the current world.");
	CommandList["worldmessage"] = command.addToMap();
	#pragma endregion

	#pragma region GM Level 1
	command.level = 1;

	command.command = &ManagementFunctions::kill;
	command.syntax = "<${players | gm | all | me} | $player name>";
	command.notes.push_back("If you are GM level 1, you can only kill yourself with this.");
	command.notes.push_back("If you are above GM level 1, you may kill GMs, players, everyone on a map, yourself, or the specified player.");
	CommandList["kill"] = command.addToMap();

	command.command = &InfoFunctions::lookup;
	command.syntax = "<${item | skill | map | mob | npc | quest | continent | id | scriptbyname | scriptbyid}> <$search | #id>";
	command.notes.push_back("Uses the database to give you the string values for an ID or the IDs for a given string value.");
	command.notes.push_back("Use !help map to see valid string values for continent lookup.");
	CommandList["lookup"] = command.addToMap();

	command.command = &ManagementFunctions::map;
	command.syntax = "<${town | map string | boss map string} | #map ID>";
	command.notes.push_back("Warps you to a desired map.");
	command.notes.push_back("-------------");
	command.notes.push_back("Valid maps");
	command.notes.push_back("-------------");
	command.notes.push_back("Special: gm | fm | happyville | town | here | back | 3rd | 4th | grendel | athena | darklord | danceswb | kyrin");
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
	CommandList["map"] = command.addToMap();

	command.command = &PlayerModFunctions::job;
	command.syntax = "<${job string} | #job ID>";
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
	CommandList["job"] = command.addToMap();

	command.command = &PlayerModFunctions::level;
	command.syntax = "<#level>";
	command.notes.push_back("Sets your player's level to the specified amount.");
	CommandList["level"] = command.addToMap();

	command.command = &PlayerModFunctions::hp;
	command.syntax = "<#hp>";
	command.notes.push_back("Sets your player's HP to the specified amount.");
	CommandList["hp"] = command.addToMap();

	command.command = &PlayerModFunctions::mp;
	command.syntax = "<#mp>";
	command.notes.push_back("Sets your player's MP to the specified amount.");
	CommandList["mp"] = command.addToMap();

	command.command = &PlayerModFunctions::ap;
	command.syntax = "<#ap>";
	command.notes.push_back("Sets your player's AP to the specified amount.");
	CommandList["ap"] = command.addToMap();

	command.command = &PlayerModFunctions::sp;
	command.syntax = "<#sp>";
	command.notes.push_back("Sets your player's SP to the specified amount.");
	CommandList["sp"] = command.addToMap();

	command.command = &PlayerModFunctions::addSp;
	command.syntax = "<#skill ID> [#skill points]";
	command.notes.push_back("Adds SP to the desired skill.");
	CommandList["addsp"] = command.addToMap();

	command.command = &PlayerModFunctions::maxSp;
	command.syntax = "<#skill ID> [#skill points]";
	command.notes.push_back("Sets the skill's max SP to the desired level.");
	CommandList["maxsp"] = command.addToMap();

	command.command = &PlayerModFunctions::modInt;
	command.syntax = "<#int>";
	command.notes.push_back("Sets your player's INT to the specified amount.");
	CommandList["int"] = command.addToMap();

	command.command = &PlayerModFunctions::modLuk;
	command.syntax = "<#luk>";
	command.notes.push_back("Sets your player's LUK to the specified amount.");
	CommandList["luk"] = command.addToMap();

	command.command = &PlayerModFunctions::modDex;
	command.syntax = "<#dex>";
	command.notes.push_back("Sets your player's DEX to the specified amount.");
	CommandList["dex"] = command.addToMap();

	command.command = &PlayerModFunctions::modStr;
	command.syntax = "<#str>";
	command.notes.push_back("Sets your player's STR to the specified amount.");
	CommandList["str"] = command.addToMap();

	command.command = &PlayerModFunctions::fame;
	command.syntax = "<#fame>";
	command.notes.push_back("Sets your player's fame to the specified amount.");
	CommandList["fame"] = command.addToMap();

	command.command = &PlayerModFunctions::maxStats;
	command.notes.push_back("Sets all your core stats to their maximum values.");
	CommandList["maxstats"] = command.addToMap();

	command.command = &ManagementFunctions::npc;
	command.syntax = "<#npc ID>";
	command.notes.push_back("Runs the NPC script of the NPC you specify.");
	CommandList["npc"] = command.addToMap();

	command.command = &ManagementFunctions::item;
	command.syntax = "<#item ID> [#amount]";
	command.notes.push_back("Gives you an item.");
	CommandList["item"] = command.addToMap();

	command.command = &MapFunctions::summon;
	command.syntax = "<#mob ID> [#amount]";
	command.notes.push_back("Spawns monsters.");
	CommandList["summon"] = command;
	CommandList["spawn"] = command.addToMap();

	command.command = &MessageFunctions::channelMessage;
	command.syntax = "<$message>";
	command.notes.push_back("Displays a blue GM notice.");
	CommandList["notice"] = command.addToMap();

	command.command = &ManagementFunctions::shop;
	command.syntax = "<${gear, scrolls, nx, face, ring, chair, mega, pet} | #shop ID>";
	command.notes.push_back("Shows you the desired shop.");
	CommandList["shop"] = command.addToMap();

	command.command = &InfoFunctions::pos;
	command.notes.push_back("Displays your current position and foothold on the map.");
	CommandList["pos"] = command.addToMap();

	command.command = &MapFunctions::zakum;
	command.notes.push_back("Spawns Zakum.");
	CommandList["zakum"] = command.addToMap();

	command.command = &MapFunctions::horntail;
	command.notes.push_back("Spawns Horntail.");
	CommandList["horntail"] = command.addToMap();

	command.command = &PlayerModFunctions::heal;
	command.notes.push_back("Sets your HP and MP to 100%.");
	CommandList["heal"] = command.addToMap();

	command.command = &PlayerModFunctions::modMesos;
	command.syntax = "<#meso amount>";
	command.notes.push_back("Sets your mesos to the specified amount.");
	CommandList["mesos"] = command.addToMap();

	command.command = &PlayerModFunctions::disconnect;
	command.notes.push_back("Disconnects yourself.");
	CommandList["dc"] = command.addToMap();

	command.command = &MapFunctions::music;
	command.syntax = "[$music name]";
	command.notes.push_back("Sets the music for a given map.");
	command.notes.push_back("Using \"default\" will reset the map to default music.");
	CommandList["music"] = command.addToMap();

	command.command = &ManagementFunctions::storage;
	command.notes.push_back("Shows your storage.");
	CommandList["storage"] = command.addToMap();

	command.command = &MapFunctions::eventInstruction;
	command.notes.push_back("Shows event instructions for Ola Ola, etc.");
	CommandList["eventinstruct"] = command.addToMap();

	command.command = &ManagementFunctions::relog;
	command.notes.push_back("Logs you back in to the current channel.");
	CommandList["relog"] = command.addToMap();

	command.command = &PlayerModFunctions::save;
	command.notes.push_back("Saves your stats.");
	CommandList["save"] = command.addToMap();

	command.command = &WarpFunctions::warpTo;
	command.syntax = "<$player name>";
	command.notes.push_back("Warps you to the specified player.");
	CommandList["warpto"] = command.addToMap();

	command.command = &ManagementFunctions::killNpc;
	command.notes.push_back("Used when scripts leave an NPC hanging. This command will clear the NPC and allow you to use other NPCs.");
	CommandList["killnpc"] = command.addToMap();

	command.command = &MapFunctions::listMobs;
	command.notes.push_back("Lists all the mobs on the map.");
	CommandList["listmobs"] = command.addToMap();

	command.command = &MapFunctions::getMobHp;
	command.syntax = "<#map mob ID>";
	command.notes.push_back("Gets the HP of a specific mob based on the map mob ID that you can get from !listmobs.");
	CommandList["getmobhp"] = command.addToMap();

	command.command = &MapFunctions::killMob;
	command.syntax = "<#map mob ID>";
	command.notes.push_back("Kills a specific mob based on the map mob ID that you can get from !listmobs.");
	CommandList["killmob"] = command.addToMap();

	command.command = &ManagementFunctions::reload;
	command.syntax = "<${all | items | drops | mobs | beauty | shops | scripts | reactors | pets | quests | skills}>";
	command.notes.push_back("Reloads data from the database.");
	CommandList["reload"] = command.addToMap();

	command.command = &ManagementFunctions::changeChannel;
	command.syntax = "<#channel>";
	command.notes.push_back("Allows you to change channels on any map.");
	CommandList["cc"] = command.addToMap();

	command.command = &InfoFunctions::online;
	command.notes.push_back("Allows you to see up to 100 players on the current channel.");
	CommandList["online"] = command.addToMap();

	command.command = &ManagementFunctions::lag;
	command.syntax = "<$player>";
	command.notes.push_back("Allows you to view the lag of any player.");
	CommandList["lag"] = command.addToMap();
	#pragma endregion

	#pragma region GM Level 0
	command.level = 0;

	command.command = &InfoFunctions::help;
	command.syntax = "[$command]";
	command.notes.push_back("I wonder what it does?");
	command.notes.push_back("Syntax for help display:");
	command.notes.push_back("$ = string");
	command.notes.push_back("# = number");
	command.notes.push_back("${hi | bye} = specific choices, in this case, strings of hi or bye");
	command.notes.push_back("<#time in seconds> = required parameter");
	command.notes.push_back("[#time in seconds] = optional parameter");
	CommandList["help"] = command.addToMap();
	#pragma endregion

	CustomFunctions::initialize(CommandList);
}

int32_t ChatHandlerFunctions::getMap(const string &query, Player *player) {
	int32_t mapId = -1;
	// Special
	if (query == "here") mapId = player->getMap();
	else if (query == "back") mapId = player->getLastMap();
	else if (query == "town") mapId = Maps::getMap(player->getMap())->getReturnMap();
	else if (query == "gm") mapId = Maps::GmMap;
	else if (query == "fm") mapId = 910000000;
	else if (query == "4th") mapId = 240010501;
	else if (query == "3rd") mapId = 211000001;
	else if (query == "grendel") mapId = 101000003;
	else if (query == "athena") mapId = 100000201;
	else if (query == "darklord") mapId = 103000003;
	else if (query == "danceswb") mapId = 102000003;
	else if (query == "kyrin") mapId = 120000101;
	else if (query == "happyville") mapId = 209000000;
	// Maple Island
	else if (query == "southperry") mapId = 60000;
	else if (query == "amherst") mapId = 1010000;
	// Victoria
	else if (query == "henesys") mapId = 100000000;
	else if (query == "perion") mapId = 102000000;
	else if (query == "sharenian") mapId = 101030104;
	else if (query == "ellinia") mapId = 101000000;
	else if (query == "sleepywood") mapId = 105040300;
	else if (query == "lith") mapId = 104000000;
	else if (query == "florina") mapId = 110000000;
	else if (query == "kerning") mapId = 103000000;
	else if (query == "port") mapId = 120000000;
	// Ossyria
	else if (query == "orbis") mapId = 200000000;
	else if (query == "nath") mapId = 211000000;
	else if (query == "leafre") mapId = 240000000;
	else if (query == "mulung") mapId = 250000000;
	else if (query == "herbtown") mapId = 251000000;
	else if (query == "ariant") mapId = 260000000;
	else if (query == "magatia") mapId = 261000000;
	// Ludus Lake area
	else if (query == "ludi") mapId = 220000000;
	else if (query == "altaire") mapId = 300000000;
	else if (query == "kft") mapId = 222000000;
	else if (query == "aqua") mapId = 230000000;
	else if (query == "omega") mapId = 221000000;
	// Floating areas/not officially a part of other continents
	else if (query == "temple") mapId = 270000000;
	else if (query == "ereve") mapId = 130000200;
	else if (query == "rien") mapId = 140000000;
	// Masteria
	else if (query == "nlc") mapId = 600000000;
	else if (query == "amoria") mapId = 680000000;
	else if (query == "crimsonwood") mapId = 610020006;
	// Dungeon areas
	else if (query == "armory") mapId = 801040004;
	else if (query == "mansion") mapId = 682000000;
	else if (query == "dungeon") mapId = 105090200;
	else if (query == "mine") mapId = 211041400;
	// World Tour
	else if (query == "singapore") mapId = 540000000;
	else if (query == "quay") mapId = 541000000;
	else if (query == "malaysia") mapId = 550000000;
	else if (query == "kampung") mapId = 551000000;
	else if (query == "shrine") mapId = 800000000;
	else if (query == "showa") mapId = 801000000;
	// Area boss maps
	else if (query == "manon") mapId = 240020401;
	else if (query == "griffey") mapId = 240020101;
	else if (query == "jrbalrog") mapId = 105090900;
	else if (query == "anego") mapId = 801040003;
	else if (query == "tengu") mapId = 800020130;
	else if (query == "lilynouch") mapId = 270020500;
	else if (query == "dodo") mapId = 270010500;
	else if (query == "lyka") mapId = 270030500;
	// PQ boss maps
	else if (query == "ergoth") mapId = 990000900;
	else if (query == "lordpirate") mapId = 925100500;
	else if (query == "alishar") mapId = 922010900;
	else if (query == "papapixie") mapId = 920010800;
	else if (query == "kingslime") mapId = 103000804;
	else if (query == "dunes") mapId = 926010000;
	// Boss maps
	else if (query == "pap") mapId = 220080001;
	else if (query == "zakum") mapId = 280030000;
	else if (query == "horntail") mapId = 240060200;
	else if (query == "pianus") mapId = 230040420;
	else if (query == "grandpa") mapId = 801040100;
	else if (query == "bean") mapId = 270050100;
	else {
		char *endptr;
		mapId = strtol(query.c_str(), &endptr, 0);
		if (strlen(endptr) != 0) mapId = -1;
	}
	return mapId;
}

int16_t ChatHandlerFunctions::getJob(const string &query) {
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
	else if (query == "fpwiz") job = Jobs::JobIds::FpWizard;
	else if (query == "fpmage") job = Jobs::JobIds::FpMage;
	else if (query == "fparch") job = Jobs::JobIds::FpArchMage;
	else if (query == "ilwiz") job = Jobs::JobIds::IlWizard;
	else if (query == "ilmage") job = Jobs::JobIds::IlMage;
	else if (query == "ilarch") job = Jobs::JobIds::IlArchMage;
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

string ChatHandlerFunctions::getBanString(int8_t reason) {
	string banMessage = ".";
	switch (reason) {
		case 0x01: banMessage = " for hacking."; break;
		case 0x02: banMessage = " for using macro/auto-keyboard."; break;
		case 0x03: banMessage = " for illicit promotion or advertising."; break;
		case 0x04: banMessage = " for harassment."; break;
		case 0x05: banMessage = " for using profane language."; break;
		case 0x06: banMessage = " for scamming."; break;
		case 0x07: banMessage = " for misconduct."; break;
		case 0x08: banMessage = " for illegal cash transaction."; break;
		case 0x09: banMessage = " for illegal charging/funding."; break;
		case 0x0A: banMessage = " for temporary request."; break;
		case 0x0B: banMessage = " for impersonating GM."; break;
		case 0x0C: banMessage = " for using illegal programs or violating the game policy."; break;
		case 0x0D: banMessage = " for one of cursing, scamming, or illegal trading via Megaphones."; break;
	}
	return banMessage;
}

int8_t ChatHandlerFunctions::getMessageType(const string &query) {
	int8_t ret = -1;
	if (query == "notice") ret = PlayerPacket::NoticeTypes::Notice;
	else if (query == "box") ret = PlayerPacket::NoticeTypes::Box;
	else if (query == "red") ret = PlayerPacket::NoticeTypes::Red;
	else if (query == "blue") ret = PlayerPacket::NoticeTypes::Blue;
	return ret;
}

bool ChatHandlerFunctions::runRegexPattern(const string &args, const string &pattern, cmatch &matches) {
	regex re;
	re = pattern; // Why, C++, why?
	// Compiles matches if successful and will return true
	// Otherwise returns false
	return std::regex_match(args.c_str(), matches, re);
}

void ChatHandlerFunctions::showSyntax(Player *player, const string &command, bool fromHelp) {
	if (CommandList.find(command) != CommandList.end()) {
		ChatCommand &cmd = CommandList[command];
		const string &msg = "Usage: !" + command + " " + cmd.syntax;
		PlayerPacket::showMessage(player, msg, PlayerPacket::NoticeTypes::Blue);

		if (fromHelp) {
			PlayerPacket::showMessage(player, "Notes: " + cmd.notes[0], PlayerPacket::NoticeTypes::Blue);
			for (size_t i = 1; i < cmd.notes.size(); i++) {
				PlayerPacket::showMessage(player, cmd.notes[i], PlayerPacket::NoticeTypes::Blue);
			}
		}
	}
}