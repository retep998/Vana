/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "ChatHandlerFunctions.hpp"
#include "CustomFunctions.hpp"
#include "InfoFunctions.hpp"
#include "JobConstants.hpp"
#include "MapFunctions.hpp"
#include "Maps.hpp"
#include "ManagementFunctions.hpp"
#include "MessageFunctions.hpp"
#include "Player.hpp"
#include "PlayerModFunctions.hpp"
#include "PlayerPacket.hpp"

case_insensitive_hash_map_t<ChatCommand, chat_t> ChatHandlerFunctions::sCommandList;

const case_insensitive_hash_map_t<MapPair, chat_t> ChatHandlerFunctions::sMapAssociations = {
	// These first maps are here purely for documentation purposes - they are computed by other means
	{"town", {Maps::NoMap, "Special"}},
	{"return", {Maps::NoMap, "Special"}},
	{"here", {Maps::NoMap, "Special"}},
	{"back", {Maps::NoMap, "Special"}},
	// Actual maps
	{"gm", {Maps::GmMap, "Special"}},
	{"fm", {910000000, "Special"}},
	{"happyville", {209000000, "Special"}},
	// Job-Related
	{"3rd", {211000001, "Job-Related"}},
	{"4th", {240010501, "Job-Related"}},
	{"stone", {211040401, "Job-Related"}},
	{"grendel", {101000003, "Job-Related"}},
	{"athena", {100000201, "Job-Related"}},
	{"darklord", {103000003, "Job-Related"}},
	{"danceswithbalrog", {102000003, "Job-Related"}},
	{"kyrin", {120000101, "Job-Related"}},
	// NPC
	{"spiruna", {200050001, "NPC"}},
	{"vega", {221022000, "NPC"}},
	{"huckle", {200081201, "NPC"}},
	{"hughes", {200082301, "NPC"}},
	{"kenta", {230000003, "NPC"}},
	{"carta", {230040001, "NPC"}},
	// Maple Island
	{"southperry", {60000, "Maple Island"}},
	{"amherst", {1010000, "Maple Island"}},
	// Victoria
	{"henesys", {100000000, "Victoria"}},
	{"perion", {102000000, "Victoria"}},
	{"ellinia", {101000000, "Victoria"}},
	{"sleepywood", {105040300, "Victoria"}},
	{"lith", {104000000, "Victoria"}},
	{"florina", {110000000, "Victoria"}},
	{"kerning", {103000000, "Victoria"}},
	{"port", {120000000, "Victoria"}},
	// Misc Victoria
	{"gamezone", {100000203, "Misc Victoria"}},
	{"hhg", {104040000, "Misc Victoria"}},
	{"anttunnel", {105070001, "Misc Victoria"}},
	{"dungeon", {105090200, "Misc Victoria"}},
	{"subway", {103000100, "Misc Victoria"}},
	{"pigbeach", {104010001, "Misc Victoria"}},
	{"hotsand", {110040000, "Misc Victoria"}},
	{"slimetree", {101010100, "Misc Victoria"}},
	{"lorangx3", {110020001, "Misc Victoria"}},
	{"sd3", {105040303, "Misc Victoria"}},
	{"fog", {105040306, "Misc Victoria"}},
	{"sanc2", {105090600, "Misc Victoria"}},
	{"boars1", {101040001, "Misc Victoria"}},
	{"boars2", {101030001, "Misc Victoria"}},
	{"dangervalley", {106000002, "Misc Victoria"}},
	{"wildkargo", {105090301, "Misc Victoria"}},
	{"drakearea", {103000100, "Misc Victoria"}},
	{"drakehunting", {105080000, "Misc Victoria"}},
	{"drakemeal", {105090300, "Misc Victoria"}},
	{"burnzone2", {106000110, "Misc Victoria"}},
	{"golemtemple", {106010102, "Misc Victoria"}},
	{"crokos", {107000300, "Misc Victoria"}},
	// Ossyria
	{"orbis", {200000000, "Ossyria"}},
	{"nath", {211000000, "Ossyria"}},
	{"leafre", {240000000, "Ossyria"}},
	{"mulung", {250000000, "Ossyria"}},
	{"herbtown", {251000000, "Ossyria"}},
	{"ariant", {260000000, "Ossyria"}},
	{"magatia", {261000000, "Ossyria"}},
	// Misc Ossyria
	{"guildhq", {200000301, "Misc Ossyria"}},
	{"sharpcliff", {211040300, "Misc Ossyria"}},
	{"mine", {211041400, "Misc Ossyria"}},
	{"caveoftrial", {211042000, "Misc Ossyria"}},
	{"canyon", {240040000, "Misc Ossyria"}},
	{"newties", {240040520, "Misc Ossyria"}},
	{"skeles", {240040511, "Misc Ossyria"}},
	{"strollpath", {200030000, "Misc Ossyria"}},
	{"strollpath2", {200060000, "Misc Ossyria"}},
	{"garden", {200010100, "Misc Ossyria"}},
	{"skystairway", {200010200, "Misc Ossyria"}},
	{"skystairway2", {200010300, "Misc Ossyria"}},
	{"werewolves", {211040500, "Misc Ossyria"}},
	{"lycans", {211040800, "Misc Ossyria"}},
	{"dragonforest", {240030100, "Misc Ossyria"}},
	{"bluewyverns", {240040210, "Misc Ossyria"}},
	{"redwyverns", {240040310, "Misc Ossyria"}},
	{"darkwyverns", {240040500, "Misc Ossyria"}},
	{"pirateden", {251010401, "Misc Ossyria"}},
	{"foggyforest", {250010501, "Misc Ossyria"}},
	{"wildbear", {250010301, "Misc Ossyria"}},
	// Ludus Lake
	{"ludi", {220000000, "Ludus Lake"}},
	{"kft", {222000000, "Ludus Lake"}},
	{"aqua", {230000000, "Ludus Lake"}},
	{"omega", {221000000, "Ludus Lake"}},
	{"altaire", {300000000, "Ludus Lake"}},
	// Misc Ludus Lake
	{"clocktower", {220050300, "Misc Ludus Lake"}},
	{"kulan5", {221040400, "Misc Ludus Lake"}},
	{"sharks", {230040400, "Misc Ludus Lake"}},
	{"squids", {230040300, "Misc Ludus Lake"}},
	{"gobies", {230040100, "Misc Ludus Lake"}},
	{"duals", {220060201, "Misc Ludus Lake"}},
	{"vikings", {220060300, "Misc Ludus Lake"}},
	{"mdt", {220070201, "Misc Ludus Lake"}},
	{"grims", {220070301, "Misc Ludus Lake"}},
	{"forestkft", {222010102, "Misc Ludus Lake"}},
	{"ridgekft", {222010200, "Misc Ludus Lake"}},
	{"hauntedkft", {222010401, "Misc Ludus Lake"}},
	// Masteria
	{"nlc", {600000000, "Masteria"}},
	{"amoria", {680000000, "Masteria"}},
	{"crimsonwood", {610020006, "Masteria"}},
	// Misc Masteria
	{"mansion", {682000000, "Misc Masteria"}},
	{"gorge", {610010004, "Misc Masteria"}},
	{"forgottenpath", {610010005, "Misc Masteria"}},
	{"lowerascent", {610020002, "Misc Masteria"}},
	{"spiders", {600020300, "Misc Masteria"}},
	{"gryphons", {600010500, "Misc Masteria"}},
	// Landmasses
	{"temple", {270000000, "Landmasses"}},
	{"ereve", {130000200, "Landmasses"}},
	{"rien", {140000000, "Landmasses"}},
	// World Tour
	{"showa", {801000000, "World Tour"}},
	{"shrine", {800000000, "World Tour"}},
	{"singapore", {540000000, "World Tour"}},
	{"quay", {541000000, "World Tour"}},
	{"malaysia", {550000000, "World Tour"}},
	{"kampung", {551000000, "World Tour"}},
	// Misc World Tour
	{"armory", {801040004, "Misc World Tour"}},
	{"path3", {541000300, "Misc World Tour"}},
	{"ghost6", {541010050, "Misc World Tour"}},
	// PQ
	{"ergoth", {990000900, "PQ"}},
	{"lordpirate", {925100500, "PQ"}},
	{"kingslime", {103000804, "PQ"}},
	{"alishar", {922010900, "PQ"}},
	{"papapixie", {920010800, "PQ"}},
	{"dunes", {926010000, "PQ"}},
	{"orbispq", {200080101, "PQ"}},
	{"ludipq", {221024500, "PQ"}},
	{"guildpq", {101030104, "PQ"}},
	{"piratepq", {251010404, "PQ"}},
	{"amoriapq", {670010000, "PQ"}},
	// Area Boss
	{"mushmom", {100000005, "Area Boss"}},
	{"bluemushmom", {800010100, "Area Boss"}},
	{"zombiemushmom", {105070002, "Area Boss"}},
	{"manon", {240020401, "Area Boss"}},
	{"griffey", {240020101, "Area Boss"}},
	{"jrbalrog", {105090900, "Area Boss"}},
	{"anego", {801040003, "Area Boss"}},
	{"rombot", {221020701, "Area Boss"}},
	{"mt09", {221030601, "Area Boss"}},
	{"tengu", {800020130, "Area Boss"}},
	{"lilynouch", {270020500, "Area Boss"}},
	{"dodo", {270010500, "Area Boss"}},
	{"lyka", {270030500, "Area Boss"}},
	// Boss
	{"pap", {220080001, "Boss"}},
	{"zakum", {280030000, "Boss"}},
	{"horntail", {240060200, "Boss"}},
	{"pianus", {230040420, "Boss"}},
	{"grandpa", {801040100, "Boss"}},
	{"bean", {270050100, "Boss"}},
	// Misc Boss
	{"papdoor", {220080000, "Misc Boss"}},
	{"zakumdoor", {211042300, "Misc Boss"}},
	{"caveoflifedoor", {240040700, "Misc Boss"}},
	{"caveoflife", {240050000, "Misc Boss"}},
	{"zakumsignup", {211042400, "Misc Boss"}},
	{"horntailsignup", {240050400, "Misc Boss"}},
	{"beansignup", {270050000, "Misc Boss"}},
};

auto ChatHandlerFunctions::initialize() -> void {
	// Set up commands and appropriate GM levels
	ChatCommand command;

	// Notes:
	// Don't add syntax to things that have no parameters
	// Every command needs at least one line of notes that describes what the command does

	#pragma region GM Level 3
	command.level = 3;

	command.command = &ManagementFunctions::ban;
	command.syntax = "<$player name> [#reason]";
	command.notes.push_back("Permanently bans a player by name");
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
	sCommandList["ban"] = command.addToMap();

	command.command = &ManagementFunctions::ipBan;
	command.syntax = "<$player name> [#reason]";
	command.notes.push_back("Permanently bans a player's IP based on their name. Does not ban the account for various reasons");
	command.notes.push_back("Use !help ban to see the applicable reason codes");
	sCommandList["ipban"] = command.addToMap();

	command.command = &ManagementFunctions::tempBan;
	command.syntax = "<$player name> <#reason> <#length in days>";
	command.notes.push_back("Temporarily bans a player by name");
	command.notes.push_back("Use !help ban to see the applicable reason codes");
	sCommandList["tempban"] = command.addToMap();

	command.command = &ManagementFunctions::unban;
	command.syntax = "<$player name>";
	command.notes.push_back("Removes a ban from the database");
	sCommandList["unban"] = command.addToMap();

	command.command = &ManagementFunctions::header;
	command.syntax = "[$message]";
	command.notes.push_back("Changes the scrolling message at the top of the screen");
	sCommandList["header"] = command.addToMap();

	command.command = &ManagementFunctions::shutdown;
	command.notes.push_back("Stops the current ChannelServer");
	sCommandList["shutdown"] = command.addToMap();

	command.command = &MapFunctions::timer;
	command.syntax = "<#time in seconds>";
	command.notes.push_back("Displays a timer at the top of the map");
	sCommandList["timer"] = command.addToMap();

	command.command = &MapFunctions::instruction;
	command.syntax = "<$bubble text>";
	command.notes.push_back("Displays a bubble. With shiny text. Somewhat useless for managing players");
	sCommandList["instruction"] = command.addToMap();

	command.command = &ManagementFunctions::addNpc;
	command.syntax = "<#npc ID>";
	command.notes.push_back("Permanently adds an NPC to a map");
	sCommandList["addnpc"] = command.addToMap();

	command.command = &ManagementFunctions::calculateRanks;
	command.notes.push_back("Forces ranking recalculation");
	sCommandList["dorankings"] = command.addToMap();

	command.command = &MessageFunctions::globalMessage;
	command.syntax = "<${notice | box | red | blue}> <$message>";
	command.notes.push_back("Displays a message to every channel on every world");
	sCommandList["globalmessage"] = command.addToMap();
	#pragma endregion

	#pragma region GM Level 2
	command.level = 2;

	command.command = &ManagementFunctions::kick;
	command.syntax = "<$player name>";
	command.notes.push_back("Forcibly disconnects a player, cannot be used on players that outrank you in GM level");
	sCommandList["kick"] = command.addToMap();

	command.command = &ManagementFunctions::warp;
	command.syntax = "<${map | player | self | current | channel}> <${map | player | self | current}> <{$map string | #map ID | $player name}> [{$map string | #map ID | $player name}]";
	command.notes.push_back("Warps the source argument to the destination argument");
	command.notes.push_back("This is a complicated command, but here are some examples with explanations:");
	command.notes.push_back("1: !warp player player Bill James");
	command.notes.push_back("2: !warp map map gm 4th");
	command.notes.push_back("3: !warp self player James");
	command.notes.push_back("4: !warp current player James");
	command.notes.push_back("5: !warp channel map henesys");
	command.notes.push_back("#1 warps player Bill to player James");
	command.notes.push_back("#2 warps all players on the GM map to the 4th job map");
	command.notes.push_back("#3 warps the user to player James");
	command.notes.push_back("#4 warps all players on the user's map to player James");
	command.notes.push_back("#5 warps all players on the channel to Henesys");
	command.notes.push_back("Not all combinations of arguments work (e.g. self self)");
	sCommandList["warp"] = command.addToMap();

	command.command = &MapFunctions::killAllMobs;
	command.notes.push_back("Kills all mobs on the current map");
	sCommandList["killall"] = command.addToMap();

	command.command = &MapFunctions::clearDrops;
	command.notes.push_back("Clears all drops from the current map");
	sCommandList["cleardrops"] = command.addToMap();

	command.command = &MessageFunctions::worldMessage;
	command.syntax = "<${notice | box | red | blue}> <$message>";
	command.notes.push_back("Displays a message to every channel on the current world");
	sCommandList["worldmessage"] = command.addToMap();
	#pragma endregion

	#pragma region GM Level 1
	command.level = 1;

	command.command = &ManagementFunctions::kill;
	command.syntax = "<${players | gm | all | me} | $player name>";
	command.notes.push_back("If you are GM level 1, you can only kill yourself with this");
	command.notes.push_back("If you are above GM level 1, you may kill GMs, players, everyone on a map, yourself, or the specified player");
	sCommandList["kill"] = command.addToMap();

	command.command = &InfoFunctions::lookup;
	command.syntax = "<${item | equip | use | setup | etc | cash | skill | map | mob | npc | quest | continent | id | scriptbyname | scriptbyid | whatdrops | whatmaps | music | drops}> <$search | #id>";
	command.notes.push_back("Uses the database to give you the string values for an ID or the IDs for a given string value");
	command.notes.push_back("Use !help map to see valid string values for continent lookup");
	command.notes.push_back("Searches that are based on ID: continent, id, scriptbyid, whatdrops, drops");
	command.notes.push_back("Searches that are based on search string: item, equip, use, etc, cash, skill, map, mob, npc, quest, scriptbyname, music");
	command.notes.push_back("A whatmaps search is special; you must specify a type (portal, mob, reactor, npc) when you do a whatmaps search");
	command.notes.push_back("A whatmaps portal search requires a script name");
	command.notes.push_back("The other whatmaps searches require IDs");
	sCommandList["lookup"] = command.addToMap();

	command.command = &InfoFunctions::variable;
	command.syntax = "<$variable name>";
	command.notes.push_back("Displays the value for a given player variable (or an error if the value is blank/doesn't exist)");
	sCommandList["variable"] = command.addToMap();

	command.command = &InfoFunctions::questData;
	command.syntax = "<#quest ID> <$data>";
	command.notes.push_back("Sets the quest data for the specified quest to the specified data.");
	sCommandList["questdata"] = command.addToMap();

	command.command = &InfoFunctions::questKills;
	command.syntax = "<#mob ID> <#count>";
	command.notes.push_back("Mocks killing a specified number of the specified mob.");
	sCommandList["questkills"] = command.addToMap();

	command.command = &ManagementFunctions::map;
	command.syntax = "<{$map string | #map ID}> [{$portal name | $position string | $foothold string}]";
	command.notes.push_back("Warps you to a desired map");
	command.notes.push_back("Position strings have the following format: {xPosition, yPosition}");
	command.notes.push_back("For example: {-500, 1300}");
	command.notes.push_back("Foothold strings have the following format: [foothold ID]");
	command.notes.push_back("For example: [150]");
	command.notes.push_back("-------------------");
	command.notes.push_back("Available Maps");
	command.notes.push_back("-------------------");
	ord_map_t<string_t, ord_set_t<string_t>> byCategory;
	for (const auto &kvp : sMapAssociations) {
		auto existing = byCategory.find(kvp.second.category);
		if (existing == std::end(byCategory)) {
			existing = byCategory.emplace(kvp.second.category, ord_set_t<string_t>{}).first;
		}
		existing->second.insert(kvp.first);
	}
	for (const auto &kvp : byCategory) {
		chat_stream_t category;
		category << " >>> " << kvp.first + ": ";
		bool separate = false;
		for (const auto &map : kvp.second) {
			if (separate) {
				category << " | ";
			}
			category << map;
			separate = true;
		}
		command.notes.push_back(category.str());
	}
	sCommandList["map"] = command.addToMap();

	command.command = &ManagementFunctions::follow;
	command.syntax = "[$player name]";
	command.notes.push_back("Follows a player through his/her channel changes and map changes");
	command.notes.push_back("You can stop following by using !follow with no arguments");
	sCommandList["follow"] = command.addToMap();

	command.command = &PlayerModFunctions::job;
	command.syntax = "<{$job string | #job ID}>";
	command.notes.push_back("Sets your job");
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
	sCommandList["job"] = command.addToMap();

	command.command = &PlayerModFunctions::level;
	command.syntax = "<#level>";
	command.notes.push_back("Sets your player's level to the specified amount");
	sCommandList["level"] = command.addToMap();

	command.command = &PlayerModFunctions::hp;
	command.syntax = "<#hp>";
	command.notes.push_back("Sets your player's HP to the specified amount");
	sCommandList["hp"] = command.addToMap();

	command.command = &PlayerModFunctions::mp;
	command.syntax = "<#mp>";
	command.notes.push_back("Sets your player's MP to the specified amount");
	sCommandList["mp"] = command.addToMap();

	command.command = &PlayerModFunctions::ap;
	command.syntax = "<#ap>";
	command.notes.push_back("Sets your player's AP to the specified amount");
	sCommandList["ap"] = command.addToMap();

	command.command = &PlayerModFunctions::sp;
	command.syntax = "<#sp>";
	command.notes.push_back("Sets your player's SP to the specified amount");
	sCommandList["sp"] = command.addToMap();

	command.command = &PlayerModFunctions::addSp;
	command.syntax = "<#skill ID> [#skill points]";
	command.notes.push_back("Adds SP to the desired skill");
	sCommandList["addsp"] = command.addToMap();

	command.command = &PlayerModFunctions::maxSp;
	command.syntax = "<#skill ID> [#skill points]";
	command.notes.push_back("Sets the skill's max SP to the desired level");
	sCommandList["maxsp"] = command.addToMap();

	command.command = &PlayerModFunctions::modInt;
	command.syntax = "<#int>";
	command.notes.push_back("Sets your player's INT to the specified amount");
	sCommandList["int"] = command.addToMap();

	command.command = &PlayerModFunctions::modLuk;
	command.syntax = "<#luk>";
	command.notes.push_back("Sets your player's LUK to the specified amount");
	sCommandList["luk"] = command.addToMap();

	command.command = &PlayerModFunctions::modDex;
	command.syntax = "<#dex>";
	command.notes.push_back("Sets your player's DEX to the specified amount");
	sCommandList["dex"] = command.addToMap();

	command.command = &PlayerModFunctions::modStr;
	command.syntax = "<#str>";
	command.notes.push_back("Sets your player's STR to the specified amount");
	sCommandList["str"] = command.addToMap();

	command.command = &PlayerModFunctions::fame;
	command.syntax = "<#fame>";
	command.notes.push_back("Sets your player's fame to the specified amount");
	sCommandList["fame"] = command.addToMap();

	command.command = &PlayerModFunctions::maxStats;
	command.notes.push_back("Sets all your core stats to their maximum values");
	sCommandList["maxstats"] = command.addToMap();

	command.command = &ManagementFunctions::npc;
	command.syntax = "<#npc ID>";
	command.notes.push_back("Runs the NPC script of the NPC you specify");
	sCommandList["npc"] = command.addToMap();

	command.command = &ManagementFunctions::item;
	command.syntax = "<#item ID> [#amount]";
	command.notes.push_back("Gives you an item");
	sCommandList["item"] = command.addToMap();

	command.command = &MapFunctions::summon;
	command.syntax = "<#mob ID> [#amount]";
	command.notes.push_back("Spawns monsters");
	sCommandList["summon"] = command;
	sCommandList["spawn"] = command.addToMap();

	command.command = &MessageFunctions::channelMessage;
	command.syntax = "<${notice | box | red | blue}> <$message>";
	command.notes.push_back("Displays a message to every player on the current channel");
	sCommandList["notice"] = command.addToMap();

	command.command = &MessageFunctions::gmChatMode;
	command.notes.push_back("Toggles whether your all chat is displayed only to other GMs or functions like normal");
	sCommandList["gmchat"] = command.addToMap();

	command.command = &ManagementFunctions::shop;
	command.syntax = "<{${gear, scrolls, nx, face, ring, chair, mega, pet} | #shop ID}>";
	command.notes.push_back("Shows you the desired shop");
	sCommandList["shop"] = command.addToMap();

	command.command = &InfoFunctions::pos;
	command.notes.push_back("Displays your current position and foothold on the map. The format is {X, Y} [Foothold]");
	sCommandList["pos"] = command.addToMap();

	command.command = &MapFunctions::mapDimensions;
	command.syntax = "[{$map string | #map ID}]";
	command.notes.push_back("Displays the left top and right bottom dimensions of the map");
	sCommandList["dimensions"] = command.addToMap();

	command.command = &MapFunctions::zakum;
	command.notes.push_back("Spawns Zakum");
	sCommandList["zakum"] = command.addToMap();

	command.command = &MapFunctions::horntail;
	command.notes.push_back("Spawns Horntail");
	sCommandList["horntail"] = command.addToMap();

	command.command = &PlayerModFunctions::heal;
	command.notes.push_back("Sets your HP and MP to 100%");
	sCommandList["heal"] = command.addToMap();

	command.command = &PlayerModFunctions::modMesos;
	command.syntax = "<#meso amount>";
	command.notes.push_back("Sets your mesos to the specified amount");
	sCommandList["mesos"] = command.addToMap();

	command.command = &PlayerModFunctions::disconnect;
	command.notes.push_back("Disconnects yourself");
	sCommandList["dc"] = command.addToMap();

	command.command = &MapFunctions::music;
	command.syntax = "[$music name]";
	command.notes.push_back("Sets the music for a given map");
	command.notes.push_back("Using \"default\" will reset the map to default music");
	sCommandList["music"] = command.addToMap();

	command.command = &ManagementFunctions::storage;
	command.notes.push_back("Shows your storage");
	sCommandList["storage"] = command.addToMap();

	command.command = &MapFunctions::eventInstruction;
	command.notes.push_back("Shows event instructions for Ola Ola, etc");
	sCommandList["eventinstruct"] = command.addToMap();

	command.command = &ManagementFunctions::relog;
	command.notes.push_back("Logs you back in to the current channel");
	sCommandList["relog"] = command.addToMap();

	command.command = &PlayerModFunctions::save;
	command.notes.push_back("Saves your stats");
	sCommandList["save"] = command.addToMap();

	command.command = &ManagementFunctions::killNpc;
	command.notes.push_back("Used when scripts leave an NPC hanging. This command will clear the NPC and allow you to use other NPCs");
	sCommandList["killnpc"] = command.addToMap();

	command.command = &MapFunctions::listMobs;
	command.syntax = "[{$map string | #map ID}]";
	command.notes.push_back("Lists all the mobs on the map");
	sCommandList["listmobs"] = command.addToMap();

	command.command = &MapFunctions::listPortals;
	command.syntax = "[{$map string | #map ID}] [{$portal filter|tp|sp}]";
	command.notes.push_back("Lists all the non-spawn/non-Mystic Door portals on the map unless the filter argument is specified - otherwise, it will give all portals matching the specified portal label");
	sCommandList["listportals"] = command.addToMap();

	command.command = &MapFunctions::listPlayers;
	command.syntax = "[{$map string | #map ID}]";
	command.notes.push_back("Lists all players on the map");
	sCommandList["listplayers"] = command.addToMap();

	command.command = &MapFunctions::listReactors;
	command.syntax = "[{$map string | #map ID}]";
	command.notes.push_back("Lists all the reactors on the map");
	sCommandList["listreactors"] = command.addToMap();

	command.command = &MapFunctions::listNpcs;
	command.syntax = "[{$map string | #map ID}]";
	command.notes.push_back("Lists all the NPCs and potentially their scripts on the map");
	sCommandList["listnpcs"] = command.addToMap();

	command.command = &MapFunctions::getMobHp;
	command.syntax = "<#map mob ID>";
	command.notes.push_back("Gets the HP of a specific mob based on the map mob ID that you can get from !listmobs");
	sCommandList["getmobhp"] = command.addToMap();

	command.command = &MapFunctions::killMob;
	command.syntax = "<#map mob ID>";
	command.notes.push_back("Kills a specific mob based on the map mob ID that you can get from !listmobs");
	sCommandList["killmob"] = command.addToMap();

	command.command = &ManagementFunctions::reload;
	command.syntax = "<${all | items | drops | mobs | beauty | shops | scripts | reactors | pets | quests | skills}>";
	command.notes.push_back("Reloads data from the database");
	sCommandList["reload"] = command.addToMap();

	command.command = &ManagementFunctions::changeChannel;
	command.syntax = "<#channel>";
	command.notes.push_back("Allows you to change channels on any map");
	sCommandList["cc"] = command.addToMap();

	command.command = &InfoFunctions::online;
	command.notes.push_back("Allows you to see up to 100 players on the current channel");
	sCommandList["online"] = command.addToMap();

	command.command = &ManagementFunctions::lag;
	command.syntax = "<$player>";
	command.notes.push_back("Allows you to view the lag of any player");
	sCommandList["lag"] = command.addToMap();

	command.command = &ManagementFunctions::rehash;
	command.notes.push_back("Rehashes world configurations after modification");
	sCommandList["rehash"] = command.addToMap();

	command.command = &ManagementFunctions::rates;
	command.syntax = "<${view | mobexp | mobmeso | questexp | drop}> [#new rate]";
	command.notes.push_back("Allows the viewing or setting of rates on the current world");
	sCommandList["rates"] = command.addToMap();
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
	command.notes.push_back("{${hi | bye} | #time in seconds} = specific choices, in this case, strings of hi or bye or the time in seconds");
	command.notes.push_back("<#time in seconds> = required parameter");
	command.notes.push_back("[#time in seconds] = optional parameter");
	sCommandList["help"] = command.addToMap();

	command.command = &InfoFunctions::gmLevel;
	command.notes.push_back("Displays your current GM level");
	sCommandList["gmlevel"] = command.addToMap();
	#pragma endregion

	CustomFunctions::initialize(sCommandList);
}

auto ChatHandlerFunctions::getMap(const chat_t &query, Player *player) -> int32_t {
	map_id_t mapId = -1;
	// Special
	chat_t lowercaseQuery = StringUtilities::toLower(query);
	if (lowercaseQuery == "here") mapId = player->getMapId();
	else if (lowercaseQuery == "back") mapId = player->getLastMapId();
	else if (lowercaseQuery == "town") mapId = player->getMap()->getReturnMap();
	else if (lowercaseQuery == "return") {
		mapId = player->getMap()->getForcedReturn();
		if (mapId == Maps::NoMap) mapId = player->getMap()->getReturnMap();
	}
	else {
		auto kvp = sMapAssociations.find(lowercaseQuery);
		if (kvp != std::end(sMapAssociations)) mapId = kvp->second.mapId;
		else {
			char *endptr;
			mapId = strtol(lowercaseQuery.c_str(), &endptr, 0);
			if (strlen(endptr) != 0) mapId = -1;
		}
	}
	return mapId;
}

auto ChatHandlerFunctions::getJob(const chat_t &query) -> job_id_t {
	job_id_t job = -1;
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
	else {
		char *endptr;
		job = static_cast<job_id_t>(strtol(query.c_str(), &endptr, 0));
		if (strlen(endptr) != 0) job = -1;
	}
	return job;
}

auto ChatHandlerFunctions::getBanString(int8_t reason) -> chat_t {
	chat_t banMessage = ".";
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

auto ChatHandlerFunctions::getMessageType(const chat_t &query) -> int8_t {
	int8_t ret = -1;
	if (query == "notice") ret = PlayerPacket::NoticeTypes::Notice;
	else if (query == "box") ret = PlayerPacket::NoticeTypes::Box;
	else if (query == "red") ret = PlayerPacket::NoticeTypes::Red;
	else if (query == "blue") ret = PlayerPacket::NoticeTypes::Blue;
	return ret;
}

auto ChatHandlerFunctions::runRegexPattern(const chat_t &args, const chat_t &pattern, match_t &matches) -> MatchResult {
	std::regex re;
	re = pattern; // Why, C++, why?
	return std::regex_match(args, matches, re) ? MatchResult::AnyMatches : MatchResult::NoMatches;
}

auto ChatHandlerFunctions::showSyntax(Player *player, const chat_t &command, bool fromHelp) -> void {
	auto kvp = sCommandList.find(command);
	if (kvp != std::end(sCommandList)) {
		auto &cmd = kvp->second;

		using overload_t = void (*)(Player *, const chat_t &);
		auto displayStyle = fromHelp ?
			static_cast<overload_t>(showInfo) :
			static_cast<overload_t>(showError);

		displayStyle(player, "Usage: !" + command + " " + cmd.syntax);

		if (fromHelp) {
			displayStyle(player, "Notes: " + cmd.notes[0]);
			for (size_t i = 1; i < cmd.notes.size(); i++) {
				displayStyle(player, cmd.notes[i]);
			}
		}
	}
}

auto ChatHandlerFunctions::showError(Player *player, const chat_t &message) -> void {
	player->send(PlayerPacket::showMessage(message, PlayerPacket::NoticeTypes::Red));
}

auto ChatHandlerFunctions::showInfo(Player *player, const chat_t &message) -> void {
	player->send(PlayerPacket::showMessage(message, PlayerPacket::NoticeTypes::Blue));
}

auto ChatHandlerFunctions::showError(Player *player, function_t<void(chat_stream_t &)> produceMessage) -> void {
	chat_stream_t error;
	produceMessage(error);
	showError(player, error.str());
}

auto ChatHandlerFunctions::showInfo(Player *player, function_t<void(chat_stream_t &)> produceMessage) -> void {
	chat_stream_t info;
	produceMessage(info);
	showInfo(player, info.str());
}

auto ChatHandlerFunctions::showError(Player *player, const char *message) -> void {
	showError(player, chat_t{message});
}

auto ChatHandlerFunctions::showInfo(Player *player, const char *message) -> void {
	showInfo(player, chat_t{message});
}