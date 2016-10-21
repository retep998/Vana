/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "chat_handler_functions.hpp"
#include "channel_server/custom_functions.hpp"
#include "channel_server/info_functions.hpp"
#include "channel_server/map_functions.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/management_functions.hpp"
#include "channel_server/message_functions.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_mod_functions.hpp"
#include "channel_server/player_packet.hpp"

namespace vana {
namespace channel_server {

case_insensitive_hash_map<chat_command, game_chat> chat_handler_functions::g_command_list;

const case_insensitive_hash_map<map_pair, game_chat> chat_handler_functions::g_map_associations = {
	// These first maps are here purely for documentation purposes - they are computed by other means
	{"town", {constant::map::no_map, "Special"}},
	{"return", {constant::map::no_map, "Special"}},
	{"here", {constant::map::no_map, "Special"}},
	{"back", {constant::map::no_map, "Special"}},
	// Actual maps
	{"gm", {constant::map::gm_map, "Special"}},
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
	{"kulan3", {221040200, "Misc Ludus Lake"}},
	{"kulan5", {221040400, "Misc Ludus Lake"}},
	{"boswell3", {221030300, "Misc Ludus Lake"}},
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

auto chat_handler_functions::initialize() -> void {
	// Set up commands and appropriate GM levels
	chat_command command;

	// Notes:
	// Don't add syntax to things that have no parameters
	// Every command needs at least one line of notes that describes what the command does

	#pragma region GM Level 3
	command.level = 3;

	command.command = &management_functions::ban;
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
	g_command_list["ban"] = command.add_to_map();

	command.command = &management_functions::ip_ban;
	command.syntax = "<$player name> [#reason]";
	command.notes.push_back("Permanently bans a player's IP based on their name. Does not ban the account for various reasons");
	command.notes.push_back("Use !help ban to see the applicable reason codes");
	g_command_list["ipban"] = command.add_to_map();

	command.command = &management_functions::temp_ban;
	command.syntax = "<$player name> <#reason> <#length in days>";
	command.notes.push_back("Temporarily bans a player by name");
	command.notes.push_back("Use !help ban to see the applicable reason codes");
	g_command_list["tempban"] = command.add_to_map();

	command.command = &management_functions::unban;
	command.syntax = "<$player name>";
	command.notes.push_back("Removes a ban from the database");
	g_command_list["unban"] = command.add_to_map();

	command.command = &management_functions::header;
	command.syntax = "[$message]";
	command.notes.push_back("Changes the scrolling message at the top of the screen");
	g_command_list["header"] = command.add_to_map();

	command.command = &management_functions::shutdown;
	command.notes.push_back("Stops the current ChannelServer");
	g_command_list["shutdown"] = command.add_to_map();

	command.command = &map_functions::timer;
	command.syntax = "<#time in seconds>";
	command.notes.push_back("Displays a timer at the top of the map");
	g_command_list["timer"] = command.add_to_map();

	command.command = &map_functions::instruction;
	command.syntax = "<$bubble text>";
	command.notes.push_back("Displays a bubble. With shiny text. Somewhat useless for managing players");
	g_command_list["instruction"] = command.add_to_map();

	command.command = &management_functions::add_npc;
	command.syntax = "<#npc ID>";
	command.notes.push_back("Permanently adds an NPC to a map");
	g_command_list["addnpc"] = command.add_to_map();

	command.command = &management_functions::calculate_ranks;
	command.notes.push_back("Forces ranking recalculation");
	g_command_list["dorankings"] = command.add_to_map();

	command.command = &message_functions::global_message;
	command.syntax = "<${notice | box | red | blue}> <$message>";
	command.notes.push_back("Displays a message to every channel on every world");
	g_command_list["globalmessage"] = command.add_to_map();
	#pragma endregion

	#pragma region GM Level 2
	command.level = 2;

	command.command = &management_functions::kick;
	command.syntax = "<$player name>";
	command.notes.push_back("Forcibly disconnects a player, cannot be used on players that outrank you in GM level");
	g_command_list["kick"] = command.add_to_map();

	command.command = &management_functions::warp;
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
	g_command_list["warp"] = command.add_to_map();

	command.command = &map_functions::kill_all_mobs;
	command.notes.push_back("Kills all mobs on the current map");
	g_command_list["killall"] = command.add_to_map();

	command.command = &map_functions::clear_drops;
	command.notes.push_back("Clears all drops from the current map");
	g_command_list["cleardrops"] = command.add_to_map();

	command.command = &message_functions::world_message;
	command.syntax = "<${notice | box | red | blue}> <$message>";
	command.notes.push_back("Displays a message to every channel on the current world");
	g_command_list["worldmessage"] = command.add_to_map();
	#pragma endregion

	#pragma region GM Level 1
	command.level = 1;

	command.command = &management_functions::kill;
	command.syntax = "<${players | gm | all | me} | $player name>";
	command.notes.push_back("If you are GM level 1, you can only kill yourself with this");
	command.notes.push_back("If you are above GM level 1, you may kill GMs, players, everyone on a map, yourself, or the specified player");
	g_command_list["kill"] = command.add_to_map();

	command.command = &info_functions::lookup;
	command.syntax = "<${item | equip | use | setup | etc | cash | skill | map | mob | npc | quest | continent | id | scriptbyname | scriptbyid | whatdrops | whatmaps | music | drops}> <$search | #id>";
	command.notes.push_back("Uses the database to give you the string values for an ID or the IDs for a given string value");
	command.notes.push_back("Use !help map to see valid string values for continent lookup");
	command.notes.push_back("Searches that are based on ID: continent, id, scriptbyid, whatdrops, drops");
	command.notes.push_back("Searches that are based on search string: item, equip, use, etc, cash, skill, map, mob, npc, quest, scriptbyname, music");
	command.notes.push_back("A whatmaps search is special; you must specify a type (portal, mob, reactor, npc) when you do a whatmaps search");
	command.notes.push_back("A whatmaps portal search requires a script name");
	command.notes.push_back("The other whatmaps searches require IDs");
	g_command_list["lookup"] = command.add_to_map();

	command.command = &info_functions::variable;
	command.syntax = "<$variable name>";
	command.notes.push_back("Displays the value for a given player variable (or an error if the value is blank/doesn't exist)");
	g_command_list["variable"] = command.add_to_map();

	command.command = &info_functions::quest_data;
	command.syntax = "<#quest ID> <$data>";
	command.notes.push_back("Sets the quest data for the specified quest to the specified data.");
	g_command_list["questdata"] = command.add_to_map();

	command.command = &info_functions::quest_kills;
	command.syntax = "<#mob ID> <#count>";
	command.notes.push_back("Mocks killing a specified number of the specified mob.");
	g_command_list["questkills"] = command.add_to_map();

	command.command = &management_functions::user_warp;
	command.syntax = "<{$map string | #map ID}> [{$portal name | $position string | $foothold string}]";
	command.notes.push_back("Warps you to a desired map");
	command.notes.push_back("Position strings have the following format: {xPosition, yPosition}");
	command.notes.push_back("For example: {-500, 1300}");
	command.notes.push_back("Foothold strings have the following format: [foothold ID]");
	command.notes.push_back("For example: [150]");
	command.notes.push_back("-------------------");
	command.notes.push_back("Available Maps");
	command.notes.push_back("-------------------");
	ord_map<string, ord_set<string>> by_category;
	for (const auto &kvp : g_map_associations) {
		auto existing = by_category.find(kvp.second.category);
		if (existing == std::end(by_category)) {
			existing = by_category.emplace(kvp.second.category, ord_set<string>{}).first;
		}
		existing->second.insert(kvp.first);
	}
	for (const auto &kvp : by_category) {
		game_chat_stream category;
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
	g_command_list["map"] = command.add_to_map();

	command.command = &management_functions::follow;
	command.syntax = "[$player name]";
	command.notes.push_back("Follows a player through his/her channel changes and map changes");
	command.notes.push_back("You can stop following by using !follow with no arguments");
	g_command_list["follow"] = command.add_to_map();

	command.command = &player_mod_functions::job;
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
	g_command_list["job"] = command.add_to_map();

	command.command = &player_mod_functions::level;
	command.syntax = "<#level>";
	command.notes.push_back("Sets your player's level to the specified amount");
	g_command_list["level"] = command.add_to_map();

	command.command = &player_mod_functions::hp;
	command.syntax = "<#hp>";
	command.notes.push_back("Sets your player's HP to the specified amount");
	g_command_list["hp"] = command.add_to_map();

	command.command = &player_mod_functions::mp;
	command.syntax = "<#mp>";
	command.notes.push_back("Sets your player's MP to the specified amount");
	g_command_list["mp"] = command.add_to_map();

	command.command = &player_mod_functions::ap;
	command.syntax = "<#ap>";
	command.notes.push_back("Sets your player's AP to the specified amount");
	g_command_list["ap"] = command.add_to_map();

	command.command = &player_mod_functions::sp;
	command.syntax = "<#sp>";
	command.notes.push_back("Sets your player's SP to the specified amount");
	g_command_list["sp"] = command.add_to_map();

	command.command = &player_mod_functions::add_sp;
	command.syntax = "<#skill ID> [#skill points]";
	command.notes.push_back("Adds SP to the desired skill");
	g_command_list["addsp"] = command.add_to_map();

	command.command = &player_mod_functions::max_sp;
	command.syntax = "<#skill ID> [#skill points]";
	command.notes.push_back("Sets the skill's max SP to the desired level");
	g_command_list["maxsp"] = command.add_to_map();

	command.command = &player_mod_functions::mod_int;
	command.syntax = "<#int>";
	command.notes.push_back("Sets your player's INT to the specified amount");
	g_command_list["int"] = command.add_to_map();

	command.command = &player_mod_functions::mod_luk;
	command.syntax = "<#luk>";
	command.notes.push_back("Sets your player's LUK to the specified amount");
	g_command_list["luk"] = command.add_to_map();

	command.command = &player_mod_functions::mod_dex;
	command.syntax = "<#dex>";
	command.notes.push_back("Sets your player's DEX to the specified amount");
	g_command_list["dex"] = command.add_to_map();

	command.command = &player_mod_functions::mod_str;
	command.syntax = "<#str>";
	command.notes.push_back("Sets your player's STR to the specified amount");
	g_command_list["str"] = command.add_to_map();

	command.command = &player_mod_functions::fame;
	command.syntax = "<#fame>";
	command.notes.push_back("Sets your player's fame to the specified amount");
	g_command_list["fame"] = command.add_to_map();

	command.command = &player_mod_functions::max_stats;
	command.notes.push_back("Sets all your core stats to their maximum values");
	g_command_list["maxstats"] = command.add_to_map();

	command.command = &management_functions::run_npc;
	command.syntax = "<#npc ID>";
	command.notes.push_back("Runs the NPC script of the NPC you specify");
	g_command_list["npc"] = command.add_to_map();

	command.command = &management_functions::item;
	command.syntax = "<#item ID> [#amount]";
	command.notes.push_back("Gives you an item");
	g_command_list["item"] = command.add_to_map();

	command.command = &map_functions::summon;
	command.syntax = "<#mob ID> [#amount]";
	command.notes.push_back("Spawns monsters");
	g_command_list["summon"] = command;
	g_command_list["spawn"] = command.add_to_map();

	command.command = &message_functions::channel_message;
	command.syntax = "<${notice | box | red | blue}> <$message>";
	command.notes.push_back("Displays a message to every player on the current channel");
	g_command_list["notice"] = command.add_to_map();

	command.command = &message_functions::gm_chat_mode;
	command.notes.push_back("Toggles whether your all chat is displayed only to other GMs or functions like normal");
	g_command_list["gmchat"] = command.add_to_map();

	command.command = &management_functions::shop;
	command.syntax = "<{${gear, scrolls, nx, face, ring, chair, mega, pet} | #shop ID}>";
	command.notes.push_back("Shows you the desired shop");
	g_command_list["shop"] = command.add_to_map();

	command.command = &info_functions::pos;
	command.notes.push_back("Displays your current position and foothold on the map. The format is {X, Y} [Foothold]");
	g_command_list["pos"] = command.add_to_map();

	command.command = &map_functions::map_dimensions;
	command.syntax = "[{$map string | #map ID}]";
	command.notes.push_back("Displays the left top and right bottom dimensions of the map");
	g_command_list["dimensions"] = command.add_to_map();

	command.command = &map_functions::zakum;
	command.notes.push_back("Spawns Zakum");
	g_command_list["zakum"] = command.add_to_map();

	command.command = &map_functions::horntail;
	command.notes.push_back("Spawns Horntail");
	g_command_list["horntail"] = command.add_to_map();

	command.command = &player_mod_functions::heal;
	command.notes.push_back("Sets your HP and MP to 100%");
	g_command_list["heal"] = command.add_to_map();

	command.command = &player_mod_functions::mod_mesos;
	command.syntax = "<#meso amount>";
	command.notes.push_back("Sets your mesos to the specified amount");
	g_command_list["mesos"] = command.add_to_map();

	command.command = &player_mod_functions::disconnect;
	command.notes.push_back("Disconnects yourself");
	g_command_list["dc"] = command.add_to_map();

	command.command = &map_functions::music;
	command.syntax = "[$music name]";
	command.notes.push_back("Sets the music for a given map");
	command.notes.push_back("Using \"default\" will reset the map to default music");
	g_command_list["music"] = command.add_to_map();

	command.command = &management_functions::storage;
	command.notes.push_back("Shows your storage");
	g_command_list["storage"] = command.add_to_map();

	command.command = &map_functions::event_instruction;
	command.notes.push_back("Shows event instructions for Ola Ola, etc");
	g_command_list["eventinstruct"] = command.add_to_map();

	command.command = &management_functions::relog;
	command.notes.push_back("Logs you back in to the current channel");
	g_command_list["relog"] = command.add_to_map();

	command.command = &player_mod_functions::save;
	command.notes.push_back("Saves your stats");
	g_command_list["save"] = command.add_to_map();

	command.command = &management_functions::kill_npc;
	command.notes.push_back("Used when scripts leave an NPC hanging. This command will clear the NPC and allow you to use other NPCs");
	g_command_list["killnpc"] = command.add_to_map();

	command.command = &map_functions::list_mobs;
	command.syntax = "[{$map string | #map ID}]";
	command.notes.push_back("Lists all the mobs on the map");
	g_command_list["listmobs"] = command.add_to_map();

	command.command = &map_functions::list_portals;
	command.syntax = "[{$map string | #map ID}] [{$portal filter|tp|sp}]";
	command.notes.push_back("Lists all the non-spawn/non-Mystic Door portals on the map unless the filter argument is specified - otherwise, it will give all portals matching the specified portal label");
	g_command_list["listportals"] = command.add_to_map();

	command.command = &map_functions::list_players;
	command.syntax = "[{$map string | #map ID}]";
	command.notes.push_back("Lists all players on the map");
	g_command_list["listplayers"] = command.add_to_map();

	command.command = &map_functions::list_reactors;
	command.syntax = "[{$map string | #map ID}]";
	command.notes.push_back("Lists all the reactors on the map");
	g_command_list["listreactors"] = command.add_to_map();

	command.command = &map_functions::list_npcs;
	command.syntax = "[{$map string | #map ID}]";
	command.notes.push_back("Lists all the NPCs and potentially their scripts on the map");
	g_command_list["listnpcs"] = command.add_to_map();

	command.command = &map_functions::get_mob_hp;
	command.syntax = "<#map mob ID>";
	command.notes.push_back("Gets the HP of a specific mob based on the map mob ID that you can get from !listmobs");
	g_command_list["getmobhp"] = command.add_to_map();

	command.command = &map_functions::kill_mob;
	command.syntax = "<#map mob ID>";
	command.notes.push_back("Kills a specific mob based on the map mob ID that you can get from !listmobs");
	g_command_list["killmob"] = command.add_to_map();

	command.command = &management_functions::reload;
	command.syntax = "<${all | items | drops | mobs | beauty | shops | scripts | reactors | pets | quests | maps | skills}>";
	command.notes.push_back("Reloads data from the database");
	g_command_list["reload"] = command.add_to_map();

	command.command = &management_functions::change_channel;
	command.syntax = "<#channel>";
	command.notes.push_back("Allows you to change channels on any map");
	g_command_list["cc"] = command.add_to_map();

	command.command = &info_functions::online;
	command.notes.push_back("Allows you to see up to 100 players on the current channel");
	g_command_list["online"] = command.add_to_map();

	command.command = &management_functions::lag;
	command.syntax = "<$player>";
	command.notes.push_back("Allows you to view the lag of any player");
	g_command_list["lag"] = command.add_to_map();

	command.command = &management_functions::rehash;
	command.notes.push_back("Rehashes world configurations after modification");
	g_command_list["rehash"] = command.add_to_map();

	command.command = &management_functions::rates;
	command.syntax = "<${view | reset | set}> <${mobexp | dropmeso | questexp | drop | globaldrop | globaldropmeso}> [#new rate]";
	command.notes.push_back("Allows the viewing or setting of rates on the current world");
	g_command_list["rates"] = command.add_to_map();

	command.command = &management_functions::packet;
	command.syntax = "<hex string and/or special placeholders>";
	command.notes.push_back("Send a packet to yourself");
	command.notes.push_back("This command also supports placeholders for making sending packets easier.");
	command.notes.push_back("Packets of 0 or 1 byte will not be sent.");
	command.notes.push_back("If you want to send B0 - B9, you need to write it with a capital.");
	command.notes.push_back("Supported placeholders:");
	command.notes.push_back(" \"text\": write 'text' as a string (including size)");
	command.notes.push_back(" lNNN: write an int64 (long), where NNN = number from " + utilities::str::lexical_cast<string>(INT64_MIN) + " to " + utilities::str::lexical_cast<string>(INT64_MAX) + " inclusive");
	command.notes.push_back(" iNNN: write an uint32 (unsigned int), where NNN = number from 0 to " + utilities::str::lexical_cast<string>(UINT32_MAX) + " inclusive");
	command.notes.push_back(" sNNN: write an uint16 (unsigned short), where NNN = number from 0 to " + utilities::str::lexical_cast<string>(UINT16_MAX) + " inclusive");
	command.notes.push_back(" bNNN: write an uint8 (unsigned byte), where NNN = number from 0 to " + utilities::str::lexical_cast<string>(UINT8_MAX) + " inclusive");
	command.notes.push_back("Example:");
	command.notes.push_back(" !packet 7E00 b3 \"dojang/end/clear\"");
	command.notes.push_back(" !packet 9400 i0 b1 \"This is a message from a gm, probably\" b0 \"Vana\"");
	g_command_list["packet"] = command.add_to_map();
	#pragma endregion

	#pragma region GM Level 0
	command.level = 0;

	command.command = &info_functions::help;
	command.syntax = "[$command]";
	command.notes.push_back("I wonder what it does?");
	command.notes.push_back("Syntax for help display:");
	command.notes.push_back("$ = string");
	command.notes.push_back("# = number");
	command.notes.push_back("${hi | bye} = specific choices, in this case, strings of hi or bye");
	command.notes.push_back("{${hi | bye} | #time in seconds} = specific choices, in this case, strings of hi or bye or the time in seconds");
	command.notes.push_back("<#time in seconds> = required parameter");
	command.notes.push_back("[#time in seconds] = optional parameter");
	g_command_list["help"] = command.add_to_map();

	command.command = &info_functions::gm_level;
	command.notes.push_back("Displays your current GM level");
	g_command_list["gmlevel"] = command.add_to_map();
	#pragma endregion

	custom_functions::initialize(g_command_list);
}

auto chat_handler_functions::get_map(const game_chat &query, ref_ptr<player> player) -> int32_t {
	game_map_id map_id = -1;
	// Special
	game_chat lowercase_query = utilities::str::to_lower(query);
	if (lowercase_query == "here") map_id = player->get_map_id();
	else if (lowercase_query == "back") map_id = player->get_last_map_id();
	else if (lowercase_query == "town") map_id = player->get_map()->get_return_map();
	else if (lowercase_query == "return") {
		map_id = player->get_map()->get_forced_return();
		if (map_id == constant::map::no_map) map_id = player->get_map()->get_return_map();
	}
	else {
		auto kvp = g_map_associations.find(lowercase_query);
		if (kvp != std::end(g_map_associations)) map_id = kvp->second.map_id;
		else {
			char *endptr;
			map_id = strtol(lowercase_query.c_str(), &endptr, 0);
			if (strlen(endptr) != 0) map_id = -1;
		}
	}
	return map_id;
}

auto chat_handler_functions::get_job(const game_chat &query) -> game_job_id {
	game_job_id job = -1;
	if (query == "beginner") job = constant::job::id::beginner;
	else if (query == "warrior") job = constant::job::id::swordsman;
	else if (query == "fighter") job = constant::job::id::fighter;
	else if (query == "sader") job = constant::job::id::crusader;
	else if (query == "hero") job = constant::job::id::hero;
	else if (query == "page") job = constant::job::id::page;
	else if (query == "wk") job = constant::job::id::white_knight;
	else if (query == "paladin") job = constant::job::id::paladin;
	else if (query == "spearman") job = constant::job::id::spearman;
	else if (query == "dk") job  = constant::job::id::dragon_knight;
	else if (query == "drk") job = constant::job::id::dark_knight;
	else if (query == "magician") job = constant::job::id::magician;
	else if (query == "fpwiz") job = constant::job::id::fp_wizard;
	else if (query == "fpmage") job = constant::job::id::fp_mage;
	else if (query == "fparch") job = constant::job::id::fp_arch_mage;
	else if (query == "ilwiz") job = constant::job::id::il_wizard;
	else if (query == "ilmage") job = constant::job::id::il_mage;
	else if (query == "ilarch") job = constant::job::id::il_arch_mage;
	else if (query == "cleric") job = constant::job::id::cleric;
	else if (query == "priest") job = constant::job::id::priest;
	else if (query == "bishop") job = constant::job::id::bishop;
	else if (query == "bowman") job = constant::job::id::archer;
	else if (query == "hunter") job = constant::job::id::hunter;
	else if (query == "ranger") job = constant::job::id::ranger;
	else if (query == "bm") job = constant::job::id::bowmaster;
	else if (query == "xbowman") job = constant::job::id::crossbowman;
	else if (query == "sniper") job = constant::job::id::sniper;
	else if (query == "marksman") job = constant::job::id::marksman;
	else if (query == "thief") job = constant::job::id::rogue;
	else if (query == "sin") job = constant::job::id::assassin;
	else if (query == "hermit") job = constant::job::id::hermit;
	else if (query == "nl") job = constant::job::id::night_lord;
	else if (query == "dit") job = constant::job::id::bandit;
	else if (query == "cb") job = constant::job::id::chief_bandit;
	else if (query == "shadower") job = constant::job::id::shadower;
	else if (query == "pirate") job = constant::job::id::pirate;
	else if (query == "brawler") job = constant::job::id::brawler;
	else if (query == "marauder") job = constant::job::id::marauder;
	else if (query == "buccaneer") job = constant::job::id::buccaneer;
	else if (query == "gunslinger") job = constant::job::id::gunslinger;
	else if (query == "outlaw") job = constant::job::id::outlaw;
	else if (query == "corsair") job = constant::job::id::corsair;
	else if (query == "gm") job = constant::job::id::gm;
	else if (query == "sgm") job = constant::job::id::super_gm;
	else if (query == "noblesse") job = constant::job::id::noblesse;
	else if (query == "dawn1") job = constant::job::id::dawn_warrior1;
	else if (query == "dawn2") job = constant::job::id::dawn_warrior2;
	else if (query == "dawn3") job = constant::job::id::dawn_warrior3;
	else if (query == "dawn4") job = constant::job::id::dawn_warrior4;
	else if (query == "blaze1") job = constant::job::id::blaze_wizard1;
	else if (query == "blaze2") job = constant::job::id::blaze_wizard2;
	else if (query == "blaze3") job = constant::job::id::blaze_wizard3;
	else if (query == "blaze4") job = constant::job::id::blaze_wizard4;
	else if (query == "wind1") job = constant::job::id::wind_archer1;
	else if (query == "wind2") job = constant::job::id::wind_archer2;
	else if (query == "wind3") job = constant::job::id::wind_archer3;
	else if (query == "wind4") job = constant::job::id::wind_archer4;
	else if (query == "night1") job = constant::job::id::night_walker1;
	else if (query == "night2") job = constant::job::id::night_walker2;
	else if (query == "night3") job = constant::job::id::night_walker3;
	else if (query == "night4") job = constant::job::id::night_walker4;
	else if (query == "thunder1") job = constant::job::id::thunder_breaker1;
	else if (query == "thunder2") job = constant::job::id::thunder_breaker2;
	else if (query == "thunder3") job = constant::job::id::thunder_breaker3;
	else if (query == "thunder4") job = constant::job::id::thunder_breaker4;
	else {
		char *endptr;
		job = static_cast<game_job_id>(strtol(query.c_str(), &endptr, 0));
		if (strlen(endptr) != 0) job = -1;
	}
	return job;
}

auto chat_handler_functions::get_ban_string(int8_t reason) -> game_chat {
	game_chat ban_message = ".";
	switch (reason) {
		case 0x01: ban_message = " for hacking."; break;
		case 0x02: ban_message = " for using macro/auto-keyboard."; break;
		case 0x03: ban_message = " for illicit promotion or advertising."; break;
		case 0x04: ban_message = " for harassment."; break;
		case 0x05: ban_message = " for using profane language."; break;
		case 0x06: ban_message = " for scamming."; break;
		case 0x07: ban_message = " for misconduct."; break;
		case 0x08: ban_message = " for illegal cash transaction."; break;
		case 0x09: ban_message = " for illegal charging/funding."; break;
		case 0x0A: ban_message = " for temporary request."; break;
		case 0x0B: ban_message = " for impersonating GM."; break;
		case 0x0C: ban_message = " for using illegal programs or violating the game policy."; break;
		case 0x0D: ban_message = " for one of cursing, scamming, or illegal trading via Megaphones."; break;
	}
	return ban_message;
}

auto chat_handler_functions::get_message_type(const game_chat &query) -> int8_t {
	int8_t ret = -1;
	if (query == "notice") ret = packets::player::notice_types::notice;
	else if (query == "box") ret = packets::player::notice_types::box;
	else if (query == "red") ret = packets::player::notice_types::red;
	else if (query == "blue") ret = packets::player::notice_types::blue;
	return ret;
}

auto chat_handler_functions::run_regex_pattern(const game_chat &args, const game_chat &pattern, match &matches) -> match_result {
	std::regex re;
	re = pattern; // Why, C++, why?
	return std::regex_match(args, matches, re) ? match_result::any_matches : match_result::no_matches;
}

auto chat_handler_functions::show_syntax(ref_ptr<player> player_value, const game_chat &command, bool from_help) -> void {
	auto kvp = g_command_list.find(command);
	if (kvp != std::end(g_command_list)) {
		auto &cmd = kvp->second;

		using overload_t = void (*)(ref_ptr<player>, const game_chat &);
		auto display_style = from_help ?
			static_cast<overload_t>(show_info) :
			static_cast<overload_t>(show_error);

		display_style(player_value, "Usage: !" + command + " " + cmd.syntax);

		if (from_help) {
			display_style(player_value, "Notes: " + cmd.notes[0]);
			for (size_t i = 1; i < cmd.notes.size(); i++) {
				display_style(player_value, cmd.notes[i]);
			}
		}
	}
}

auto chat_handler_functions::show_error(ref_ptr<player> player, const game_chat &message) -> void {
	player->send(packets::player::show_message(message, packets::player::notice_types::red));
}

auto chat_handler_functions::show_info(ref_ptr<player> player, const game_chat &message) -> void {
	player->send(packets::player::show_message(message, packets::player::notice_types::blue));
}

auto chat_handler_functions::show_error(ref_ptr<player> player, function<void(game_chat_stream &)> produce_message) -> void {
	game_chat_stream error;
	produce_message(error);
	show_error(player, error.str());
}

auto chat_handler_functions::show_info(ref_ptr<player> player, function<void(game_chat_stream &)> produce_message) -> void {
	game_chat_stream info;
	produce_message(info);
	show_info(player, info.str());
}

auto chat_handler_functions::show_error(ref_ptr<player> player, const char *message) -> void {
	show_error(player, game_chat{message});
}

auto chat_handler_functions::show_info(ref_ptr<player> player, const char *message) -> void {
	show_info(player, game_chat{message});
}

}
}