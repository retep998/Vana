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
#include "LuaScriptable.hpp"
#include "common/algorithm_temp.hpp"
#include "common/beauty_data_provider.hpp"
#include "common/game_constants.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/inter_header.hpp"
#include "common/item_constants.hpp"
#include "common/item_data_provider.hpp"
#include "common/mcdb_version.hpp"
#include "common/packet_wrapper.hpp"
#include "common/randomizer_temp.hpp"
#include "common/script_data_provider.hpp"
#include "common/shop_data_provider.hpp"
#include "common/string_utilities.hpp"
#include "common/time_utilities.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/Drop.hpp"
#include "channel_server/EffectPacket.hpp"
#include "channel_server/EventDataProvider.hpp"
#include "channel_server/Instance.hpp"
#include "channel_server/Instances.hpp"
#include "channel_server/Inventory.hpp"
#include "channel_server/InventoryPacket.hpp"
#include "channel_server/MapPacket.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/Mob.hpp"
#include "channel_server/Npc.hpp"
#include "channel_server/NpcHandler.hpp"
#include "channel_server/Party.hpp"
#include "channel_server/PartyHandler.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerPacket.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/PlayersPacket.hpp"
#include "channel_server/Quests.hpp"
#include "channel_server/Reactor.hpp"
#include <iostream>
#include <vector>

namespace vana {
namespace channel_server {

// TODO FIXME msvc
// Remove this when MSVC supports static init
string lua_scriptable::s_api_version = "1.0.0";

lua_scriptable::lua_scriptable(const string &filename, game_player_id player_id) :
	lua_environment{filename},
	m_player_id{player_id}
{
	initialize();
}

lua_scriptable::lua_scriptable(const string &filename, game_player_id player_id, bool use_thread) :
	lua_environment{filename, use_thread},
	m_player_id{player_id}
{
	initialize();
}

auto lua_scriptable::initialize() -> void {
	set<game_player_id>("system_player_id", m_player_id); // Pushing ID for reference from static functions
	set<string>("system_script", get_script_name());
	set<vector<string>>("system_path", get_script_path());
	set_environment_variables();

	auto player = channel_server::get_instance().get_player_data_provider().get_player(m_player_id);
	if (player != nullptr && player->get_instance() != nullptr) {
		set<string>("system_instance_name", player->get_instance()->get_name());
	}
	else {
		set<string>("system_instance_name", "");
	}

	// Miscellanous
	expose("consoleOutput", &lua_exports::console_output);
	expose("getRandomNumber", &lua_exports::get_random_number);
	expose("log", &lua_exports::log);
	expose("selectDiscrete", &lua_exports::select_discrete);
	expose("showGlobalMessage", &lua_exports::show_global_message);
	expose("showWorldMessage", &lua_exports::show_world_message);
	expose("testExport", &lua_exports::test_export);

	// Channel
	expose("deleteChannelVariable", &lua_exports::delete_channel_variable);
	expose("getChannel", &lua_exports::get_channel);
	expose("getChannelVariable", &lua_exports::get_channel_variable);
	expose("getWorld", &lua_exports::get_world);
	expose("setChannelVariable", &lua_exports::set_channel_variable);
	expose("showChannelMessage", &lua_exports::show_channel_message);

	// Bosses
	expose("getHorntailChannels", &lua_exports::get_horntail_channels);
	expose("getMaxHorntailBattles", &lua_exports::get_max_horntail_battles);
	expose("getMaxPapulatusBattles", &lua_exports::get_max_papulatus_battles);
	expose("getMaxPianusBattles", &lua_exports::get_max_pianus_battles);
	expose("getMaxPinkBeanBattles", &lua_exports::get_max_pink_bean_battles);
	expose("getMaxZakumBattles", &lua_exports::get_max_zakum_battles);
	expose("getPapulatusChannels", &lua_exports::get_papulatus_channels);
	expose("getPianusChannels", &lua_exports::get_pianus_channels);
	expose("getPinkBeanChannels", &lua_exports::get_pink_bean_channels);
	expose("getZakumChannels", &lua_exports::get_zakum_channels);
	expose("isHorntailChannel", &lua_exports::is_horntail_channel);
	expose("isPapulatusChannel", &lua_exports::is_papulatus_channel);
	expose("isPianusChannel", &lua_exports::is_pianus_channel);
	expose("isPinkBeanChannel", &lua_exports::is_pink_bean_channel);
	expose("isZakumChannel", &lua_exports::is_zakum_channel);

	// NPC
	expose("isBusy", &lua_exports::is_busy);
	expose("removeNpc", &lua_exports::remove_npc);
	expose("runNpc", &lua_exports::run_npc);
	expose("showShop", &lua_exports::show_shop);
	expose("spawnNpc", &lua_exports::spawn_npc);

	// Beauty
	expose("getAllFaces", &lua_exports::get_all_faces);
	expose("getAllHairs", &lua_exports::get_all_hairs);
	expose("getAllSkins", &lua_exports::get_all_skins);
	expose("getRandomFace", &lua_exports::get_random_face);
	expose("getRandomHair", &lua_exports::get_random_hair);
	expose("getRandomSkin", &lua_exports::get_random_skin);
	expose("isValidFace", &lua_exports::is_valid_face);
	expose("isValidHair", &lua_exports::is_valid_hair);
	expose("isValidSkin", &lua_exports::is_valid_skin);

	// Buddy
	expose("addBuddySlots", &lua_exports::add_buddy_slots);
	expose("getBuddySlots", &lua_exports::get_buddy_slots);

	// Skill
	expose("addSkillLevel", &lua_exports::add_skill_level);
	expose("getSkillLevel", &lua_exports::get_skill_level);
	expose("getMaxSkillLevel", &lua_exports::get_max_skill_level);
	expose("setMaxSkillLevel", &lua_exports::set_max_skill_level);

	// Quest
	expose("getQuestData", &lua_exports::get_quest_data);
	expose("isQuestActive", &lua_exports::is_quest_active);
	expose("isQuestInactive", &lua_exports::is_quest_inactive);
	expose("isQuestCompleted", &lua_exports::is_quest_completed);
	expose("setQuestData", &lua_exports::set_quest_data);

	// Inventory
	expose("addSlots", &lua_exports::add_slots);
	expose("addStorageSlots", &lua_exports::add_storage_slots);
	expose("destroyEquippedItem", &lua_exports::destroy_equipped_item);
	expose("getEquippedItemInSlot", &lua_exports::get_equipped_item_in_slot);
	expose("getItemAmount", &lua_exports::get_item_amount);
	expose("getMaxStackSize", &lua_exports::get_max_stack_size);
	expose("getMesos", &lua_exports::get_mesos);
	expose("getOpenSlots", &lua_exports::get_open_slots);
	expose("giveItem", &lua_exports::give_item);
	expose("giveItemGachapon", &lua_exports::give_item_gachapon);
	expose("giveMesos", &lua_exports::give_mesos);
	expose("hasOpenSlotsFor", &lua_exports::has_open_slots_for);
	expose("isEquippedItem", &lua_exports::is_equipped_item);
	expose("isValidItem", &lua_exports::is_valid_item);
	expose("useItem", &lua_exports::use_item);

	// Player
	expose("deletePlayerVariable", &lua_exports::delete_player_variable);
	expose("endMorph", &lua_exports::end_morph);
	expose("getAp", &lua_exports::get_ap);
	expose("getDex", &lua_exports::get_dex);
	expose("getExp", &lua_exports::get_exp);
	expose("getFace", &lua_exports::get_face);
	expose("getFame", &lua_exports::get_fame);
	expose("getFh", &lua_exports::get_fh);
	expose("getGender", &lua_exports::get_gender);
	expose("getGmLevel", &lua_exports::get_gm_level);
	expose("getHair", &lua_exports::get_hair);
	expose("getHp", &lua_exports::get_hp);
	expose("getHpMpAp", &lua_exports::get_hp_mp_ap);
	expose("getId", &lua_exports::get_id);
	expose("getInt", &lua_exports::get_int);
	expose("getJob", &lua_exports::get_job);
	expose("getLevel", &lua_exports::get_level);
	expose("getLuk", &lua_exports::get_luk);
	expose("get_map", &lua_exports::get_map);
	expose("getMaxHp", &lua_exports::get_max_hp);
	expose("getMaxMp", &lua_exports::get_max_mp);
	expose("getMp", &lua_exports::get_mp);
	expose("getName", &lua_exports::get_name);
	expose("getPlayerVariable", &lua_exports::get_player_variable);
	expose("getPosX", &lua_exports::get_pos_x);
	expose("getPosY", &lua_exports::get_pos_y);
	expose("getRealMaxHp", &lua_exports::get_real_max_hp);
	expose("getRealMaxMp", &lua_exports::get_real_max_mp);
	expose("getSkin", &lua_exports::get_skin);
	expose("getSp", &lua_exports::get_sp);
	expose("getStr", &lua_exports::get_str);
	expose("giveAp", &lua_exports::give_ap);
	expose("giveExp", &lua_exports::give_exp);
	expose("giveFame", &lua_exports::give_fame);
	expose("giveSp", &lua_exports::give_sp);
	expose("isActiveItem", &lua_exports::is_active_item);
	expose("isActiveSkill", &lua_exports::is_active_skill);
	expose("isGm", &lua_exports::is_gm);
	expose("isOnline", &lua_exports::is_online);
	expose("revertPlayer", &lua_exports::revert_player);
	expose("setAp", &lua_exports::set_ap);
	expose("setDex", &lua_exports::set_dex);
	expose("setExp", &lua_exports::set_exp);
	expose("setHp", &lua_exports::set_hp);
	expose("setInt", &lua_exports::set_int);
	expose("setJob", &lua_exports::set_job);
	expose("setLevel", &lua_exports::set_level);
	expose("setLuk", &lua_exports::set_luk);
	expose("setMap", &lua_exports::set_map);
	expose("setMaxHp", &lua_exports::set_max_hp);
	expose("setMaxMp", &lua_exports::set_max_mp);
	expose("setMp", &lua_exports::set_mp);
	expose("setPlayer", &lua_exports::set_player);
	expose("setPlayerVariable", &lua_exports::set_player_variable);
	expose("setSp", &lua_exports::set_sp);
	expose("setStr", &lua_exports::set_str);
	expose("setStyle", &lua_exports::set_style);
	expose("showInstructionBubble", &lua_exports::show_instruction_bubble);
	expose("showMessage", &lua_exports::show_message);

	// Effects
	expose("playFieldSound", &lua_exports::play_field_sound);
	expose("playMinigameSound", &lua_exports::play_minigame_sound);
	expose("setMusic", &lua_exports::set_music);
	expose("showMapEffect", &lua_exports::show_map_effect);
	expose("showMapEvent", &lua_exports::show_map_event);

	// Map
	expose("clearDrops", &lua_exports::clear_drops);
	expose("clearMobs", &lua_exports::clear_mobs);
	expose("countMobs", &lua_exports::count_mobs);
	expose("setPortalState", &lua_exports::set_portal_state);
	expose("getAllMapPlayerIds", &lua_exports::get_all_map_player_ids);
	expose("getNumPlayers", &lua_exports::get_num_players);
	expose("getReactorState", &lua_exports::get_reactor_state);
	expose("killMobs", &lua_exports::kill_mobs);
	expose("setBoatDocked", &lua_exports::set_boat_docked);
	expose("setMapSpawn", &lua_exports::set_map_spawn);
	expose("setReactorState", &lua_exports::set_reactor_state);
	expose("showMapMessage", &lua_exports::show_map_message);
	expose("showMapTimer", &lua_exports::show_map_timer);
	expose("spawnMob", &lua_exports::spawn_mob);
	expose("spawnMobPos", &lua_exports::spawn_mob_pos);

	// Mob
	expose("getMobFh", &lua_exports::get_mob_fh);
	expose("getMobHp", &lua_exports::get_mob_hp);
	expose("getMobMaxHp", &lua_exports::get_mob_max_hp);
	expose("getMobMaxMp", &lua_exports::get_mob_max_mp);
	expose("getMobMp", &lua_exports::get_mob_mp);
	expose("getMobPosX", &lua_exports::get_mob_pos_x);
	expose("getMobPosY", &lua_exports::get_mob_pos_y);
	expose("getRealMobId", &lua_exports::get_real_mob_id);
	expose("killMob", &lua_exports::kill_mob);
	expose("mobDropItem", &lua_exports::mob_drop_item);

	// Time
	expose("getDate", &lua_exports::get_date);
	expose("getDay", &lua_exports::get_day);
	expose("getHour", &lua_exports::get_hour);
	expose("getMinute", &lua_exports::get_minute);
	expose("getMonth", &lua_exports::get_month);
	expose("getNearestMinute", &lua_exports::get_nearest_minute);
	expose("getSecond", &lua_exports::get_second);
	expose("getTime", &lua_exports::get_time);
	expose("getTimeZoneOffset", &lua_exports::get_time_zone_offset);
	expose("getWeek", &lua_exports::get_week);
	expose("getYear", &lua_exports::get_year);
	expose("isDst", &lua_exports::is_dst);

	// Rates
	expose("getDropMeso", &lua_exports::get_drop_meso);
	expose("getDropRate", &lua_exports::get_drop_rate);
	expose("getExpRate", &lua_exports::get_exp_rate);
	expose("getGlobalDropMeso", &lua_exports::get_global_drop_meso);
	expose("getGlobalDropRate", &lua_exports::get_global_drop_rate);
	expose("getQuestExpRate", &lua_exports::get_quest_exp_rate);

	// Party
	expose("checkPartyFootholds", &lua_exports::check_party_footholds);
	expose("getAllPartyPlayerIds", &lua_exports::get_all_party_player_ids);
	expose("getPartyCount", &lua_exports::get_party_count);
	expose("getPartyId", &lua_exports::get_party_id);
	expose("getPartyMapCount", &lua_exports::get_party_map_count);
	expose("isPartyInLevelRange", &lua_exports::is_party_in_level_range);
	expose("isPartyLeader", &lua_exports::is_party_leader);
	expose("verifyPartyFootholds", &lua_exports::verify_party_footholds);
	expose("warpParty", &lua_exports::warp_party);

	// Instance
	expose("addInstanceMap", &lua_exports::add_instance_map);
	expose("addInstanceParty", &lua_exports::add_instance_party);
	expose("addInstancePlayer", &lua_exports::add_instance_player);
	expose("checkInstanceTimer", &lua_exports::check_instance_timer);
	expose("createInstance", &lua_exports::create_instance);
	expose("deleteInstanceVariable", &lua_exports::delete_instance_variable);
	expose("getAllInstancePlayerIds", &lua_exports::get_all_instance_player_ids);
	expose("getInstancePlayerCount", &lua_exports::get_instance_player_count);
	expose("getInstancePlayerId", &lua_exports::get_instance_player_id);
	expose("getInstanceVariable", &lua_exports::get_instance_variable);
	expose("isInstance", &lua_exports::is_instance);
	expose("isInstanceMap", &lua_exports::is_instance_map);
	expose("isInstancePersistent", &lua_exports::is_instance_persistent);
	expose("markForDelete", &lua_exports::mark_for_delete);
	expose("moveAllPlayers", &lua_exports::move_all_players);
	expose("passPlayersBetweenInstances", &lua_exports::pass_players_between_instances);
	expose("removeAllInstancePlayers", &lua_exports::remove_all_instance_players);
	expose("removeInstancePlayer", &lua_exports::remove_instance_player);
	expose("respawnInstanceMobs", &lua_exports::respawn_instance_mobs);
	expose("respawnInstanceReactors", &lua_exports::respawn_instance_reactors);
	expose("revertInstance", &lua_exports::revert_instance);
	expose("setInstance", &lua_exports::set_instance);
	expose("setInstancePersistence", &lua_exports::set_instance_persistence);
	expose("setInstanceReset", &lua_exports::set_instance_reset);
	expose("setInstanceVariable", &lua_exports::set_instance_variable);
	expose("showInstanceTime", &lua_exports::show_instance_time);
	expose("startInstanceFutureTimer", &lua_exports::start_instance_future_timer);
	expose("startInstanceSecondOfHourTimer", &lua_exports::start_instance_second_of_hour_timer);
	expose("stopAllInstanceTimers", &lua_exports::stop_all_instance_timers);
	expose("stopInstanceTimer", &lua_exports::stop_instance_timer);
}

auto lua_scriptable::set_environment_variables() -> void {
	set<string>("instance_timer", "instance");

	set<int32_t>("msg_blue", packets::player::notice_types::blue);
	set<int32_t>("msg_red", packets::player::notice_types::red);
	set<int32_t>("msg_notice", packets::player::notice_types::notice);
	set<int32_t>("msg_box", packets::player::notice_types::box);

	set<int32_t>("gender_male", gender::male);
	set<int32_t>("gender_female", gender::female);

	set<bool>("boat_docked", true);
	set<bool>("boat_undocked", false);

	set<int32_t>("type_bool", variable_type::boolean);
	set<int32_t>("type_int", variable_type::integer);
	set<int32_t>("type_num", variable_type::number);
	set<int32_t>("type_str", variable_type::string);

	set<bool>("portal_enabled", true);
	set<bool>("portal_disabled", false);

	set<int8_t>("line_beginner", jobs::job_lines::beginner);
	set<int8_t>("line_warrior", jobs::job_lines::warrior);
	set<int8_t>("line_magician", jobs::job_lines::magician);
	set<int8_t>("line_bowman", jobs::job_lines::bowman);
	set<int8_t>("line_thief", jobs::job_lines::thief);
	set<int8_t>("line_pirate", jobs::job_lines::pirate);

	set<int8_t>("progression_second", 0);
	set<int8_t>("progression_third", 1);
	set<int8_t>("progression_fourth", 2);

	set<game_item_id>("item_mesos", items::sack_of_money);

	set<string>("locale_global", mcdb::locales::global);
	set<string>("locale_korea", mcdb::locales::korea);
	set<string>("locale_japan", mcdb::locales::japan);
	set<string>("locale_china", mcdb::locales::china);
	set<string>("locale_europe", mcdb::locales::europe);
	set<string>("locale_thailand", mcdb::locales::thailand);
	set<string>("locale_tawian", mcdb::locales::taiwan);
	set<string>("locale_sea", mcdb::locales::sea);
	set<string>("locale_brazil", mcdb::locales::brazil);

	set<game_version>("env_version", maple_version::version);
	set<string>("env_subversion", maple_version::login_subversion);
	set<bool>("env_is_test_server", mcdb::is_test_server);
	set<string>("env_locale", mcdb::locale);
	set<string>("env_api_version", s_api_version);
}

auto lua_scriptable::handle_error(const string &filename, const string &error) -> void {
	auto &channel = channel_server::get_instance();
	auto player = channel.get_player_data_provider().get_player(m_player_id);

	channel.log(log_type::script_log, "script error in " + filename + ": " + error);
	if (player == nullptr) {
		return;
	}

	if (player->is_gm()) {
		player->send(packets::player::show_message(error, packets::player::notice_types::red));
	}
	else {
		player->send(packets::player::show_message("There is a script error; please contact an administrator", packets::player::notice_types::red));
	}
}

// Lua Exports
auto lua_exports::get_environment(lua_State *lua_vm) -> lua_environment & {
	return lua_environment::get_environment(lua_vm);
}

auto lua_exports::get_player(lua_State *lua_vm, lua_environment &env) -> ref_ptr<player> {
	game_player_id player_id = env.get<game_player_id>(lua_vm, "system_player_id");
	return channel_server::get_instance().get_player_data_provider().get_player(player_id);
}

auto lua_exports::get_player_deduced(int parameter, lua_State *lua_vm, lua_environment &env) -> ref_ptr<player> {
	ref_ptr<player> player = nullptr;
	if (env.is(lua_vm, parameter, lua::lua_type::string)) {
		player = channel_server::get_instance().get_player_data_provider().get_player(env.get<string>(lua_vm, parameter));
	}
	else {
		player = channel_server::get_instance().get_player_data_provider().get_player(env.get<game_player_id>(lua_vm, parameter));
	}
	return player;
}

auto lua_exports::get_instance(lua_State *lua_vm, lua_environment &env) -> instance * {
	string instance_name = env.get<string>(lua_vm, "system_instance_name");
	return channel_server::get_instance().get_instances().get_instance(instance_name);
}

auto lua_exports::obtain_set_variable_pair(lua_State *lua_vm, lua_environment &env) -> pair<string, string> {
	pair<string, string> ret;
	ret.first = env.get<string>(lua_vm, 1);
	switch (env.type_of(lua_vm, 2)) {
		case lua::lua_type::nil: ret.second = "nil"; break;
		case lua::lua_type::boolean: ret.second = env.get<bool>(lua_vm, 2) ? "true" : "false"; break;
		case lua::lua_type::number: ret.second = utilities::str::lexical_cast<string>(env.get<int32_t>(lua_vm, 2)); break;
		case lua::lua_type::string: ret.second = env.get<string>(lua_vm, 2); break;
		default: throw not_implemented_exception{"lua datatype"};
	}
	return ret;
}

auto lua_exports::push_get_variable_data(lua_State *lua_vm, lua_environment &env, const string &value, variable_type::type return_type) -> lua::lua_return {
	if (value == "nil") {
		env.push_nil(lua_vm);
	}
	else {
		switch (return_type) {
			case variable_type::boolean: env.push<bool>(lua_vm, value == "true"); break;
			case variable_type::number:
				if (value.empty()) {
					env.push_nil(lua_vm);
				}
				else {
					env.push<double>(lua_vm, utilities::str::lexical_cast<double>(value));
				}
				break;
			case variable_type::integer:
				if (value.empty()) {
					env.push_nil(lua_vm);
				}
				else {
					env.push<int32_t>(lua_vm, utilities::str::lexical_cast<int32_t>(value));
				}
				break;
			case variable_type::string: env.push<string>(lua_vm, value); break;
			default: throw std::invalid_argument{"return_type must be a valid type_ constant"};
		}
	}
	return 1;
}

auto lua_exports::is_boss_channel(lua_State *lua_vm, const vector<game_channel_id> &elements) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_channel_id channel = channel_server::get_instance().get_channel_id() + 1;
	env.push<bool>(lua_vm, ext::any_of(elements, [channel](game_channel_id test_channel) -> bool { return test_channel == channel; }));
	return 1;
}

// Miscellaneous
auto lua_exports::console_output(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	switch (env.type_of(lua_vm, 1)) {
		case lua::lua_type::none: std::cout << "i_nV_aL_iD__aR_gU_mE_nT" << std::endl; break;
		case lua::lua_type::nil: std::cout << "nil" << std::endl; break;
		case lua::lua_type::table: std::cout << "table" << std::endl; break;
		case lua::lua_type::boolean: std::cout << (env.get<bool>(lua_vm, 1) ? "true" : "false") << std::endl; break;
		case lua::lua_type::light_userdata: std::cout << "lightuserdata" << std::endl; break;
		case lua::lua_type::userdata: std::cout << "userdata" << std::endl; break;
		case lua::lua_type::thread: std::cout << "thread" << std::endl; break;
		case lua::lua_type::function: std::cout << "function" << std::endl; break;
		default: std::cout << env.get<string>(lua_vm, 1) << std::endl; break;
	}

	return 0;
}

auto lua_exports::get_random_number(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, randomizer::rand<int32_t>(env.get<int32_t>(lua_vm, 1), 1));
	return 1;
}

auto lua_exports::log(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	channel_server::get_instance().log(log_type::script_log, env.get<string>(lua_vm, 1));
	return 0;
}

auto lua_exports::select_discrete(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	vector<double> relative_chances = env.get<vector<double>>(lua_vm, 1);
	if (relative_chances.size() == 0) {
		env.push_nil(lua_vm);
	}
	else {
		std::discrete_distribution<> dist{relative_chances.begin(), relative_chances.end()};
		// Account for Lua array start
		env.push<int32_t>(lua_vm, randomizer::rand(dist) + 1);
	}
	return 1;
}

auto lua_exports::show_global_message(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string msg = env.get<string>(lua_vm, 1);
	int8_t type = env.get<int8_t>(lua_vm, 2);
	channel_server::get_instance().send_world(
		vana::packets::prepend(packets::player::show_message(msg, type), [](packet_builder &builder) {
			builder.add<packet_header>(IMSG_TO_LOGIN);
			builder.add<packet_header>(IMSG_TO_ALL_WORLDS);
			builder.add<packet_header>(IMSG_TO_ALL_CHANNELS);
			builder.add<packet_header>(IMSG_TO_ALL_PLAYERS);
		}));
	return 0;
}

auto lua_exports::show_world_message(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string msg = env.get<string>(lua_vm, 1);
	int8_t type = env.get<int8_t>(lua_vm, 2);
	channel_server::get_instance().send_world(
		vana::packets::prepend(packets::player::show_message(msg, type), [](packet_builder &builder) {
			builder.add<packet_header>(IMSG_TO_ALL_CHANNELS);
			builder.add<packet_header>(IMSG_TO_ALL_PLAYERS);
		}));
	return 0;
}

auto lua_exports::test_export(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	return 0;
}

// Channel
auto lua_exports::delete_channel_variable(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string key = env.get<string>(lua_vm, 1);
	channel_server::get_instance().get_event_data_provider().get_variables()->delete_variable(key);
	return 0;
}

auto lua_exports::get_channel(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_channel_id>(lua_vm, channel_server::get_instance().get_channel_id() + 1);
	return 1;
}

auto lua_exports::get_channel_variable(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	variable_type::type return_type = variable_type::string;
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		return_type = (variable_type::type)env.get<int32_t>(lua_vm, 2);
	}
	string val = channel_server::get_instance().get_event_data_provider().get_variables()->get_variable(env.get<string>(lua_vm, 1));
	push_get_variable_data(lua_vm, env, val, return_type);
	return 1;
}

auto lua_exports::get_world(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_world_id>(lua_vm, channel_server::get_instance().get_world_id() + 1);
	return 1;
}

auto lua_exports::set_channel_variable(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto kvp = obtain_set_variable_pair(lua_vm, env);
	channel_server::get_instance().get_event_data_provider().get_variables()->set_variable(kvp.first, kvp.second);
	return 0;
}

auto lua_exports::show_channel_message(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string msg = env.get<string>(lua_vm, 1);
	int8_t type = env.get<int8_t>(lua_vm, 2);
	channel_server::get_instance().get_player_data_provider().send(packets::player::show_message(msg, type));
	return 0;
}

// Bosses
auto lua_exports::get_horntail_channels(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<vector<game_channel_id>>(lua_vm, channel_server::get_instance().get_config().horntail.channels);
	return 1;
}

auto lua_exports::get_max_horntail_battles(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int16_t>(lua_vm, channel_server::get_instance().get_config().horntail.attempts);
	return 1;
}

auto lua_exports::get_max_papulatus_battles(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int16_t>(lua_vm, channel_server::get_instance().get_config().papulatus.attempts);
	return 1;
}

auto lua_exports::get_max_pianus_battles(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int16_t>(lua_vm, channel_server::get_instance().get_config().pianus.attempts);
	return 1;
}

auto lua_exports::get_max_pink_bean_battles(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int16_t>(lua_vm, channel_server::get_instance().get_config().pinkbean.attempts);
	return 1;
}

auto lua_exports::get_max_zakum_battles(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int16_t>(lua_vm, channel_server::get_instance().get_config().zakum.attempts);
	return 1;
}

auto lua_exports::get_papulatus_channels(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<vector<game_channel_id>>(lua_vm, channel_server::get_instance().get_config().papulatus.channels);
	return 1;
}

auto lua_exports::get_pianus_channels(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<vector<game_channel_id>>(lua_vm, channel_server::get_instance().get_config().pianus.channels);
	return 1;
}

auto lua_exports::get_pink_bean_channels(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<vector<game_channel_id>>(lua_vm, channel_server::get_instance().get_config().pinkbean.channels);
	return 1;
}

auto lua_exports::get_zakum_channels(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<vector<game_channel_id>>(lua_vm, channel_server::get_instance().get_config().zakum.channels);
	return 1;
}

auto lua_exports::is_horntail_channel(lua_State *lua_vm) -> lua::lua_return {
	return is_boss_channel(lua_vm, channel_server::get_instance().get_config().horntail.channels);
}

auto lua_exports::is_papulatus_channel(lua_State *lua_vm) -> lua::lua_return {
	return is_boss_channel(lua_vm, channel_server::get_instance().get_config().papulatus.channels);
}

auto lua_exports::is_pianus_channel(lua_State *lua_vm) -> lua::lua_return {
	return is_boss_channel(lua_vm, channel_server::get_instance().get_config().pianus.channels);
}

auto lua_exports::is_pink_bean_channel(lua_State *lua_vm) -> lua::lua_return {
	return is_boss_channel(lua_vm, channel_server::get_instance().get_config().pinkbean.channels);
}

auto lua_exports::is_zakum_channel(lua_State *lua_vm) -> lua::lua_return {
	return is_boss_channel(lua_vm, channel_server::get_instance().get_config().zakum.channels);
}

// NPC
auto lua_exports::is_busy(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<bool>(lua_vm, get_player(lua_vm, env)->get_npc() != nullptr);
	return 1;
}

auto lua_exports::remove_npc(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	uint32_t index = env.get<uint32_t>(lua_vm, 2);
	maps::get_map(map_id)->remove_npc(index);
	return 0;
}

auto lua_exports::run_npc(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_npc_id npc_id = env.get<game_npc_id>(lua_vm, 1);
	string script;
	auto &channel = channel_server::get_instance();
	if (env.is(lua_vm, 2, lua::lua_type::string)) {
		// We already have our script name
		string specified = env.get<string>(lua_vm, 2);
		script = channel.get_script_data_provider().build_script_path(script_types::npc, specified);
	}
	else {
		script = channel.get_script_data_provider().get_script(&channel, npc_id, script_types::npc);
	}
	npc *value = new npc{npc_id, get_player(lua_vm, env), script};
	value->run();
	return 0;
}

auto lua_exports::show_shop(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_shop_id shop_id = env.get<game_shop_id>(lua_vm, 1);
	npc_handler::show_shop(get_player(lua_vm, env), shop_id);
	return 0;
}

auto lua_exports::spawn_npc(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_npc_id npc_id = env.get<game_npc_id>(lua_vm, 2);
	game_coord x = env.get<game_coord>(lua_vm, 3);
	game_coord y = env.get<game_coord>(lua_vm, 4);

	npc_spawn_info npc;
	npc.id = npc_id;
	npc.foothold = 0;
	npc.pos = point{x, y};
	npc.rx0 = x - 50;
	npc.rx1 = x + 50;

	env.push<game_map_object>(lua_vm, maps::get_map(map_id)->add_npc(npc));
	return 1;
}

// Beauty
auto lua_exports::get_all_faces(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<vector<game_face_id>>(lua_vm, channel_server::get_instance().get_beauty_data_provider().get_faces(get_player(lua_vm, env)->get_gender()));
	return 1;
}

auto lua_exports::get_all_hairs(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<vector<game_hair_id>>(lua_vm, channel_server::get_instance().get_beauty_data_provider().get_hair(get_player(lua_vm, env)->get_gender()));
	return 1;
}

auto lua_exports::get_all_skins(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<vector<game_skin_id>>(lua_vm, channel_server::get_instance().get_beauty_data_provider().get_skins());
	return 1;
}

auto lua_exports::get_random_face(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_face_id>(lua_vm, channel_server::get_instance().get_beauty_data_provider().get_random_face(get_player(lua_vm, env)->get_gender()));
	return 1;
}

auto lua_exports::get_random_hair(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_hair_id>(lua_vm, channel_server::get_instance().get_beauty_data_provider().get_random_hair(get_player(lua_vm, env)->get_gender()));
	return 1;
}

auto lua_exports::get_random_skin(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_skin_id>(lua_vm, channel_server::get_instance().get_beauty_data_provider().get_random_skin());
	return 1;
}

auto lua_exports::is_valid_face(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<bool>(lua_vm, channel_server::get_instance().get_beauty_data_provider().is_valid_face(get_player(lua_vm, env)->get_gender(), env.get<game_face_id>(lua_vm, 1)));
	return 1;
}

auto lua_exports::is_valid_hair(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<bool>(lua_vm, channel_server::get_instance().get_beauty_data_provider().is_valid_hair(get_player(lua_vm, env)->get_gender(), env.get<game_hair_id>(lua_vm, 1)));
	return 1;
}

auto lua_exports::is_valid_skin(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<bool>(lua_vm, channel_server::get_instance().get_beauty_data_provider().is_valid_skin(env.get<game_skin_id>(lua_vm, 1)));
	return 1;
}

// Buddy
auto lua_exports::add_buddy_slots(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto p = get_player(lua_vm, env);
	uint8_t csize = p->get_buddy_list_size();
	int8_t mod = env.get<int8_t>(lua_vm, 1);
	p->set_buddy_list_size(csize + mod);
	return 0;
}

auto lua_exports::get_buddy_slots(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<uint8_t>(lua_vm, get_player(lua_vm, env)->get_buddy_list_size());
	return 1;
}

// Skill
auto lua_exports::add_skill_level(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_skill_id skill_id = env.get<game_skill_id>(lua_vm, 1);
	game_skill_level level = env.get<game_skill_level>(lua_vm, 2);

	if (env.is(lua_vm, 3, lua::lua_type::number)) {
		// Optional argument of increasing a skill's max level
		get_player(lua_vm, env)->get_skills()->set_max_skill_level(skill_id, env.get<game_skill_level>(lua_vm, 3));
	}

	get_player(lua_vm, env)->get_skills()->add_skill_level(skill_id, level);
	return 0;
}

auto lua_exports::get_skill_level(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_skill_id skill_id = env.get<game_skill_id>(lua_vm, 1);
	env.push<game_skill_level>(lua_vm, get_player(lua_vm, env)->get_skills()->get_skill_level(skill_id));
	return 1;
}

auto lua_exports::get_max_skill_level(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_skill_id skill_id = env.get<game_skill_id>(lua_vm, 1);
	env.push<game_skill_level>(lua_vm, get_player(lua_vm, env)->get_skills()->get_max_skill_level(skill_id));
	return 1;
}

auto lua_exports::set_max_skill_level(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_skill_id skill_id = env.get<game_skill_id>(lua_vm, 1);
	game_skill_level level = env.get<game_skill_level>(lua_vm, 2);
	get_player(lua_vm, env)->get_skills()->set_max_skill_level(skill_id, level);
	return 0;
}

// Quest
auto lua_exports::get_quest_data(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_quest_id quest_id = env.get<game_quest_id>(lua_vm, 1);
	env.push<string>(lua_vm, get_player(lua_vm, env)->get_quests()->get_quest_data(quest_id));
	return 1;
}

auto lua_exports::is_quest_active(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_quest_id quest_id = env.get<game_quest_id>(lua_vm, 1);
	env.push<bool>(lua_vm, get_player(lua_vm, env)->get_quests()->is_quest_active(quest_id));
	return 1;
}

auto lua_exports::is_quest_inactive(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_quest_id quest_id = env.get<game_quest_id>(lua_vm, 1);
	auto player = get_player(lua_vm, env);
	bool active = !(player->get_quests()->is_quest_active(quest_id) || player->get_quests()->is_quest_complete(quest_id));
	env.push<bool>(lua_vm, active);
	return 1;
}

auto lua_exports::is_quest_completed(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_quest_id quest_id = env.get<game_quest_id>(lua_vm, 1);
	env.push<bool>(lua_vm, get_player(lua_vm, env)->get_quests()->is_quest_complete(quest_id));
	return 1;
}

auto lua_exports::set_quest_data(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_quest_id quest_id = env.get<game_quest_id>(lua_vm, 1);
	string data = env.get<string>(lua_vm, 2);
	get_player(lua_vm, env)->get_quests()->set_quest_data(quest_id, data);
	return 0;
}

// Inventory
auto lua_exports::add_slots(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_inventory inventory = env.get<game_inventory>(lua_vm, 1);
	game_inventory_slot_count rows = env.get<game_inventory_slot_count>(lua_vm, 2);
	get_player(lua_vm, env)->get_inventory()->add_max_slots(inventory, rows);
	return 0;
}

auto lua_exports::add_storage_slots(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_storage_slot slots = env.get<game_storage_slot>(lua_vm, 1);
	get_player(lua_vm, env)->get_storage()->set_slots(get_player(lua_vm, env)->get_storage()->get_slots() + slots);
	return 0;
}

auto lua_exports::destroy_equipped_item(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 1);
	auto player = get_player(lua_vm, env);
	bool destroyed = player->get_inventory()->is_equipped_item(item_id);
	if (destroyed) {
		player->get_inventory()->destroy_equipped_item(item_id);
	}
	env.push<bool>(lua_vm, destroyed);
	return 1;
}

auto lua_exports::get_equipped_item_in_slot(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_inventory_slot slot = env.get<game_inventory_slot>(lua_vm, 1);
	env.push<game_item_id>(lua_vm, get_player(lua_vm, env)->get_inventory()->get_equipped_id(slot));
	return 1;
}

auto lua_exports::get_item_amount(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 1);
	env.push<game_slot_qty>(lua_vm, get_player(lua_vm, env)->get_inventory()->get_item_amount(item_id));
	return 1;
}

auto lua_exports::get_max_stack_size(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 1);
	env.push<game_slot_qty>(lua_vm, channel_server::get_instance().get_item_data_provider().get_item_info(item_id)->max_slot);
	return 1;
}

auto lua_exports::get_mesos(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_mesos>(lua_vm, get_player(lua_vm, env)->get_inventory()->get_mesos());
	return 1;
}

auto lua_exports::get_open_slots(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_inventory inv = env.get<game_inventory>(lua_vm, 1);
	env.push<game_inventory_slot_count>(lua_vm, get_player(lua_vm, env)->get_inventory()->get_open_slots_num(inv));
	return 1;
}

auto lua_exports::give_item(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 1);
	game_slot_qty amount = 1;
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		amount = env.get<game_slot_qty>(lua_vm, 2);
	}
	bool success = quests::give_item(get_player(lua_vm, env), item_id, amount) == result::successful;
	env.push<bool>(lua_vm, success);
	return 1;
}

auto lua_exports::give_item_gachapon(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 1);
	game_slot_qty amount = 1;
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		amount = env.get<game_slot_qty>(lua_vm, 2);
	}
	bool success = quests::give_item(get_player(lua_vm, env), item_id, amount, items::stat_variance::gachapon) == result::successful;
	env.push<bool>(lua_vm, success);
	return 1;
}

auto lua_exports::give_mesos(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_mesos mesos = env.get<game_mesos>(lua_vm, 1);
	bool success = quests::give_mesos(get_player(lua_vm, env), mesos) == result::successful;
	env.push<bool>(lua_vm, success);
	return 1;
}

auto lua_exports::has_open_slots_for(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 1);
	game_slot_qty amount = 1;
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		amount = env.get<game_slot_qty>(lua_vm, 2);
	}
	env.push<bool>(lua_vm, get_player(lua_vm, env)->get_inventory()->has_open_slots_for(item_id, amount));
	return 1;
}

auto lua_exports::is_equipped_item(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 1);
	env.push<bool>(lua_vm, get_player(lua_vm, env)->get_inventory()->is_equipped_item(item_id));
	return 1;
}

auto lua_exports::is_valid_item(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 1);
	env.push<bool>(lua_vm, channel_server::get_instance().get_item_data_provider().get_item_info(item_id) != nullptr);
	return 1;
}

auto lua_exports::use_item(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 1);
	inventory::use_item(get_player(lua_vm, env), item_id);
	return 0;
}

// Player
auto lua_exports::delete_player_variable(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string key = env.get<string>(lua_vm, 1);
	get_player(lua_vm, env)->get_variables()->delete_variable(key);
	return 0;
}

auto lua_exports::end_morph(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_player(lua_vm, env)->get_active_buffs()->end_morph();
	return 0;
}

auto lua_exports::get_ap(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_stat>(lua_vm, get_player(lua_vm, env)->get_stats()->get_ap());
	return 1;
}

auto lua_exports::get_dex(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_stat>(lua_vm, get_player(lua_vm, env)->get_stats()->get_dex());
	return 1;
}

auto lua_exports::get_exp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_experience>(lua_vm, get_player(lua_vm, env)->get_stats()->get_exp());
	return 1;
}

auto lua_exports::get_face(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_face_id>(lua_vm, get_player(lua_vm, env)->get_face());
	return 1;
}

auto lua_exports::get_fame(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_fame>(lua_vm, get_player(lua_vm, env)->get_stats()->get_fame());
	return 1;
}

auto lua_exports::get_fh(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_foothold_id>(lua_vm, get_player(lua_vm, env)->get_foothold());
	return 1;
}

auto lua_exports::get_gender(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_gender_id>(lua_vm, get_player(lua_vm, env)->get_gender());
	return 1;
}

auto lua_exports::get_gm_level(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, get_player(lua_vm, env)->get_gm_level());
	return 1;
}

auto lua_exports::get_hair(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_hair_id>(lua_vm, get_player(lua_vm, env)->get_hair());
	return 1;
}

auto lua_exports::get_hp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_health>(lua_vm, get_player(lua_vm, env)->get_stats()->get_hp());
	return 1;
}

auto lua_exports::get_hp_mp_ap(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_health_ap>(lua_vm, get_player(lua_vm, env)->get_stats()->get_hp_mp_ap());
	return 1;
}

auto lua_exports::get_id(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_player_id>(lua_vm, get_player(lua_vm, env)->get_id());
	return 1;
}

auto lua_exports::get_int(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_stat>(lua_vm, get_player(lua_vm, env)->get_stats()->get_int());
	return 1;
}

auto lua_exports::get_job(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_job_id>(lua_vm, get_player(lua_vm, env)->get_stats()->get_job());
	return 1;
}

auto lua_exports::get_level(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_player_level>(lua_vm, get_player(lua_vm, env)->get_stats()->get_level());
	return 1;
}

auto lua_exports::get_luk(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_stat>(lua_vm, get_player(lua_vm, env)->get_stats()->get_luk());
	return 1;
}

auto lua_exports::get_map(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_map_id>(lua_vm, get_player(lua_vm, env)->get_map_id());
	return 1;
}

auto lua_exports::get_max_hp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_health>(lua_vm, get_player(lua_vm, env)->get_stats()->get_max_hp());
	return 1;
}

auto lua_exports::get_max_mp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_health>(lua_vm, get_player(lua_vm, env)->get_stats()->get_max_mp());
	return 1;
}

auto lua_exports::get_mp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_health>(lua_vm, get_player(lua_vm, env)->get_stats()->get_mp());
	return 1;
}

auto lua_exports::get_name(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<string>(lua_vm, get_player(lua_vm, env)->get_name());
	return 1;
}

auto lua_exports::get_player_variable(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	variable_type::type return_type = variable_type::string;
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		return_type = (variable_type::type)env.get<int32_t>(lua_vm, 2);
	}
	string val = get_player(lua_vm, env)->get_variables()->get_variable(env.get<string>(lua_vm, 1));
	push_get_variable_data(lua_vm, env, val, return_type);
	return 1;
}

auto lua_exports::get_pos_x(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_coord>(lua_vm, get_player(lua_vm, env)->get_pos().x);
	return 1;
}

auto lua_exports::get_pos_y(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_coord>(lua_vm, get_player(lua_vm, env)->get_pos().y);
	return 1;
}

auto lua_exports::get_real_max_hp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_health>(lua_vm, get_player(lua_vm, env)->get_stats()->get_max_hp(true));
	return 1;
}

auto lua_exports::get_real_max_mp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_health>(lua_vm, get_player(lua_vm, env)->get_stats()->get_max_mp(true));
	return 1;
}

auto lua_exports::get_skin(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_skin_id>(lua_vm, get_player(lua_vm, env)->get_skin());
	return 1;
}

auto lua_exports::get_sp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_stat>(lua_vm, get_player(lua_vm, env)->get_stats()->get_sp());
	return 1;
}

auto lua_exports::get_str(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<game_stat>(lua_vm, get_player(lua_vm, env)->get_stats()->get_str());
	return 1;
}

auto lua_exports::give_ap(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_stat ap = env.get<game_stat>(lua_vm, -1);
	get_player(lua_vm, env)->get_stats()->set_ap(get_player(lua_vm, env)->get_stats()->get_ap() + ap);
	return 0;
}

auto lua_exports::give_exp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_experience exp = env.get<game_experience>(lua_vm, -1);
	get_player(lua_vm, env)->get_stats()->give_exp(exp * channel_server::get_instance().get_config().rates.quest_exp_rate, true);
	return 0;
}

auto lua_exports::give_fame(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_fame fame = env.get<game_fame>(lua_vm, 1);
	bool success = quests::give_fame(get_player(lua_vm, env), fame) == result::successful;
	env.push<bool>(lua_vm, success);
	return 1;
}

auto lua_exports::give_sp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_stat sp = env.get<game_stat>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_sp(get_player(lua_vm, env)->get_stats()->get_sp() + sp);
	return 0;
}

auto lua_exports::is_active_item(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item = env.get<game_item_id>(lua_vm, 1);
	bool has_buff = get_player(lua_vm, env)->get_active_buffs()->has_buff(
		buff_source_type::item,
		item);

	env.push<bool>(lua_vm, has_buff);
	return 1;
}

auto lua_exports::is_active_skill(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_skill_id skill = env.get<game_skill_id>(lua_vm, 1);
	bool has_buff = get_player(lua_vm, env)->get_active_buffs()->has_buff(
		game_logic_utilities::is_mob_skill(skill) ?
			buff_source_type::mob_skill :
			buff_source_type::skill,
		skill);
	env.push<bool>(lua_vm, has_buff);
	return 1;
}

auto lua_exports::is_gm(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<bool>(lua_vm, get_player(lua_vm, env)->is_gm());
	return 1;
}

auto lua_exports::is_online(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<bool>(lua_vm, get_player_deduced(1, lua_vm, env) != nullptr);
	return 1;
}

auto lua_exports::revert_player(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.set<game_player_id>(lua_vm, "system_player_id", env.get<game_player_id>(lua_vm, "system_old_player_id"));
	return 0;
}

auto lua_exports::set_ap(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_stat ap = env.get<game_stat>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_ap(ap);
	return 0;
}

auto lua_exports::set_dex(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_stat dex = env.get<game_stat>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_dex(dex);
	return 0;
}

auto lua_exports::set_exp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_experience exp = env.get<game_experience>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_exp(exp);
	return 0;
}

auto lua_exports::set_hp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_health hp = env.get<game_health>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_hp(hp);
	return 0;
}

auto lua_exports::set_int(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_stat intt = env.get<game_stat>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_int(intt);
	return 0;
}

auto lua_exports::set_job(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_job_id job = env.get<game_job_id>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_job(job);
	return 0;
}

auto lua_exports::set_level(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_player_level level = env.get<game_player_level>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_level(level);
	return 0;
}

auto lua_exports::set_luk(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_stat luk = env.get<game_stat>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_luk(luk);
	return 0;
}

auto lua_exports::set_map(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);

	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	if (env.is(lua_vm, 2, lua::lua_type::string)) {
		// Optional portal parameter
		string to = env.get<string>(lua_vm, 2);
		map *map = maps::get_map(map_id);
		const portal_info * const destination_portal = map->query_portal_name(to);
		get_player(lua_vm, env)->set_map(map_id, destination_portal);
		env.set<bool>(lua_vm, "player_map_changed", true);
	}
	else {
		if (maps::get_map(map_id) != nullptr) {
			get_player(lua_vm, env)->set_map(map_id);
			env.set<bool>(lua_vm, "player_map_changed", true);
		}
	}

	return 0;
}

auto lua_exports::set_max_hp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_health hp = env.get<game_health>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_max_hp(hp);
	return 0;
}

auto lua_exports::set_max_mp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_health mp = env.get<game_health>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_max_mp(mp);
	return 0;
}

auto lua_exports::set_mp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_health mp = env.get<game_health>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_mp(mp);
	return 0;
}

auto lua_exports::set_player(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto player = get_player_deduced(1, lua_vm, env);
	if (player != nullptr) {
		env.set<game_player_id>(lua_vm, "system_old_player_id", env.get<game_player_id>(lua_vm, "system_player_id"));
		env.set<game_player_id>(lua_vm, "system_player_id", player->get_id());
	}
	env.push<bool>(lua_vm, player != nullptr);
	return 1;
}

auto lua_exports::set_player_variable(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto kvp = obtain_set_variable_pair(lua_vm, env);
	get_player(lua_vm, env)->get_variables()->set_variable(kvp.first, kvp.second);
	return 0;
}

auto lua_exports::set_sp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_stat sp = env.get<game_stat>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_sp(sp);
	return 0;
}

auto lua_exports::set_str(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_stat str = env.get<game_stat>(lua_vm, 1);
	get_player(lua_vm, env)->get_stats()->set_str(str);
	return 0;
}

auto lua_exports::set_style(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	int32_t id = env.get<int32_t>(lua_vm, 1);
	int32_t type = game_logic_utilities::get_item_type(id);
	auto player = get_player(lua_vm, env);
	if (type == 0) {
		player->set_skin(static_cast<game_skin_id>(id));
	}
	else if (type == 2) {
		player->set_face(id);
	}
	else if (type == 3) {
		player->set_hair(id);
	}
	player->send_map(packets::inventory::update_player(player));
	return 0;
}

auto lua_exports::show_instruction_bubble(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string msg = env.get<string>(lua_vm, 1);
	game_coord width = env.get<game_coord>(lua_vm, 2);
	// TODO FIXME lua
	// This export appears to believe this arg is "height" however the packet function thinks it's "time"
	// Figure out which is correct
	int16_t height = env.get<int16_t>(lua_vm, 3);

	if (width == 0) {
		width = -1;
	}
	if (height == 0) {
		height = 5;
	}

	get_player(lua_vm, env)->send(packets::player::instruction_bubble(msg, width, height));
	return 0;
}

auto lua_exports::show_message(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string msg = env.get<string>(lua_vm, 1);
	int8_t type = env.get<int8_t>(lua_vm, 2);
	get_player(lua_vm, env)->send(packets::player::show_message(msg, type));
	return 0;
}

// Effects
auto lua_exports::play_field_sound(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string val = env.get<string>(lua_vm, 1);
	auto &packet = packets::send_field_sound(val);
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		maps::get_map(env.get<game_map_id>(lua_vm, 2))->send(packet);
	}
	else {
		get_player(lua_vm, env)->send(packet);
	}
	return 0;
}

auto lua_exports::play_minigame_sound(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string val = env.get<string>(lua_vm, 1);
	auto &packet = packets::send_minigame_sound(val);
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		maps::get_map(env.get<game_map_id>(lua_vm, 2))->send(packet);
	}
	else {
		get_player(lua_vm, env)->send(packet);
	}
	return 0;
}

auto lua_exports::set_music(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = -1;
	string music = env.get<string>(lua_vm, 1);

	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		map_id = env.get<game_map_id>(lua_vm, 2);
	}
	else if (auto player = get_player(lua_vm, env)) {
		map_id = player->get_map_id();
	}

	map *map = maps::get_map(map_id);
	if (map_id == -1 || map == nullptr) {
		throw std::invalid_argument("map_id must be a valid map");
	}

	map->set_music(music);
	return 0;
}

auto lua_exports::show_map_effect(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string val = env.get<string>(lua_vm, 1);
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		game_map_id map_id = env.get<game_map_id>(lua_vm, 2);
		maps::get_map(map_id)->send(packets::send_effect(val));
	}
	else {
		get_player(lua_vm, env)->send_map(packets::send_effect(val));
	}
	return 0;
}

auto lua_exports::show_map_event(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string val = env.get<string>(lua_vm, 1);
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		game_map_id map_id = env.get<game_map_id>(lua_vm, 2);
		maps::get_map(map_id)->send(packets::send_event(val));
	}
	else {
		get_player(lua_vm, env)->send_map(packets::send_event(val));
	}
	return 0;
}

// Map
auto lua_exports::clear_drops(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	maps::get_map(map_id)->clear_drops(false);
	return 0;
}

auto lua_exports::clear_mobs(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	bool distribute_exp_and_drops = true;
	if (env.is(lua_vm, 2, lua::lua_type::boolean)) {
		distribute_exp_and_drops = env.get<bool>(lua_vm, 2);
	}
	maps::get_map(map_id)->kill_mobs(nullptr, distribute_exp_and_drops);
	return 0;
}

auto lua_exports::count_mobs(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_mob_id mob_id = 0;
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		mob_id = env.get<game_mob_id>(lua_vm, 2);
	}
	env.push<int32_t>(lua_vm, maps::get_map(map_id)->count_mobs(mob_id));
	return 1;
}

auto lua_exports::set_portal_state(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	string portal_name = env.get<string>(lua_vm, 2);
	bool enabled = env.get<bool>(lua_vm, 3);

	map *map = maps::get_map(map_id);
	map->set_portal_state(portal_name, enabled);
	return 0;
}

auto lua_exports::get_all_map_player_ids(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	if (map *map = maps::get_map(map_id)) {
		env.push<vector<game_player_id>>(lua_vm, map->get_all_player_ids());
		return 1;
	}
	return 0;
}

auto lua_exports::get_num_players(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	env.push<uint32_t>(lua_vm, maps::get_map(map_id)->get_num_players());
	return 1;
}

auto lua_exports::get_reactor_state(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_reactor_id reactor_id = env.get<game_reactor_id>(lua_vm, 2);
	map *map = maps::get_map(map_id);
	for (size_t i = 0; i < map->get_num_reactors(); i++) {
		if (map->get_reactor(i)->get_reactor_id() == reactor_id) {
			env.push<int8_t>(lua_vm, map->get_reactor(i)->get_state());
			return 1;
		}
	}
	env.push<int8_t>(lua_vm, 0);
	return 1;
}

auto lua_exports::kill_mobs(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_mob_id mob_id = env.get<game_mob_id>(lua_vm, 1);
	int32_t killed = get_player(lua_vm, env)->get_map()->kill_mobs(nullptr, true, mob_id);
	env.push<int32_t>(lua_vm, killed);
	return 1;
}

auto lua_exports::set_boat_docked(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	bool docked = env.get<bool>(lua_vm, 2);
	maps::get_map(map_id)->boat_dock(docked);
	return 0;
}

auto lua_exports::set_map_spawn(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_mob_id spawn = env.get<game_mob_id>(lua_vm, 2);
	maps::get_map(map_id)->set_mob_spawning(spawn);
	return 0;
}

auto lua_exports::set_reactor_state(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_reactor_id reactor_id = env.get<game_reactor_id>(lua_vm, 2);
	int8_t state = env.get<int8_t>(lua_vm, 3);
	for (size_t i = 0; i < maps::get_map(map_id)->get_num_reactors(); i++) {
		reactor *reactor = maps::get_map(map_id)->get_reactor(i);
		if (reactor->get_reactor_id() == reactor_id) {
			reactor->set_state(state, true);
			break;
		}
	}
	return 0;
}

auto lua_exports::show_map_message(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string msg = env.get<string>(lua_vm, 1);
	int8_t type = env.get<int8_t>(lua_vm, 2);
	get_player(lua_vm, env)->send_map(packets::player::show_message(msg, type));
	return 0;
}

auto lua_exports::show_map_timer(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	seconds time = env.get<seconds>(lua_vm, 2);
	maps::get_map(map_id)->set_map_timer(time);
	return 0;
}

auto lua_exports::spawn_mob(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_mob_id mob_id = env.get<game_mob_id>(lua_vm, 1);
	auto player = get_player(lua_vm, env);
	env.push<game_map_object>(lua_vm, player->get_map()->spawn_mob(mob_id, player->get_pos())->get_map_mob_id());
	return 1;
}

auto lua_exports::spawn_mob_pos(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_mob_id mob_id = env.get<game_mob_id>(lua_vm, 2);
	game_coord x = env.get<game_coord>(lua_vm, 3);
	game_coord y = env.get<game_coord>(lua_vm, 4);
	game_foothold_id foothold = 0;
	if (env.is(lua_vm, 5, lua::lua_type::number)) {
		foothold = env.get<game_foothold_id>(lua_vm, 5);
	}
	env.push<game_map_object>(lua_vm, maps::get_map(map_id)->spawn_mob(mob_id, point{x, y}, foothold)->get_map_mob_id());
	return 1;
}

// Mob
auto lua_exports::get_mob_fh(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_map_object map_mob_id = env.get<game_map_object>(lua_vm, 2);
	env.push<game_foothold_id>(lua_vm, maps::get_map(map_id)->get_mob(map_mob_id)->get_foothold());
	return 1;
}

auto lua_exports::get_mob_hp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_map_object map_mob_id = env.get<game_map_object>(lua_vm, 2);
	env.push<int32_t>(lua_vm, maps::get_map(map_id)->get_mob(map_mob_id)->get_hp());
	return 1;
}

auto lua_exports::get_mob_max_hp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_map_object map_mob_id = env.get<game_map_object>(lua_vm, 2);
	env.push<int32_t>(lua_vm, maps::get_map(map_id)->get_mob(map_mob_id)->get_max_hp());
	return 1;
}

auto lua_exports::get_mob_max_mp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_map_object map_mob_id = env.get<game_map_object>(lua_vm, 2);
	env.push<int32_t>(lua_vm, maps::get_map(map_id)->get_mob(map_mob_id)->get_max_mp());
	return 1;
}

auto lua_exports::get_mob_mp(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_map_object map_mob_id = env.get<game_map_object>(lua_vm, 2);
	env.push<int32_t>(lua_vm, maps::get_map(map_id)->get_mob(map_mob_id)->get_mp());
	return 1;
}

auto lua_exports::get_mob_pos_x(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_map_object map_mob_id = env.get<game_map_object>(lua_vm, 2);
	env.push<game_coord>(lua_vm, maps::get_map(map_id)->get_mob(map_mob_id)->get_pos().x);
	return 1;
}

auto lua_exports::get_mob_pos_y(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_map_object map_mob_id = env.get<game_map_object>(lua_vm, 2);
	env.push<game_coord>(lua_vm, maps::get_map(map_id)->get_mob(map_mob_id)->get_pos().y);
	return 1;
}

auto lua_exports::get_real_mob_id(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_map_object map_mob_id = env.get<game_map_object>(lua_vm, 2);
	env.push<game_mob_id>(lua_vm, maps::get_map(map_id)->get_mob(map_mob_id)->get_mob_id());
	return 1;
}

auto lua_exports::kill_mob(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_map_object map_mob_id = env.get<game_map_object>(lua_vm, 2);
	auto mob = maps::get_map(map_id)->get_mob(map_mob_id);
	if (mob != nullptr) {
		mob->kill();
	}
	return 0;
}

auto lua_exports::mob_drop_item(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	game_map_object map_mob_id = env.get<game_map_object>(lua_vm, 2);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 3);
	game_slot_qty amount = 1;
	if (env.is(lua_vm, 4, lua::lua_type::number)) {
		amount = env.get<game_slot_qty>(lua_vm, 4);
	}
	auto mob = maps::get_map(map_id)->get_mob(map_mob_id);
	if (mob != nullptr) {
		item f{item_id, amount};
		drop *value = new drop(map_id, f, mob->get_pos(), 0);
		value->set_time(0);
		value->do_drop(mob->get_pos());
	}
	return 0;
}

// Time
auto lua_exports::get_date(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, utilities::time::get_date());
	return 1;
}

auto lua_exports::get_day(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	bool is_string_return = false;
	if (env.is(lua_vm, 1, lua::lua_type::boolean)) {
		is_string_return = env.get<bool>(lua_vm, 1);
	}
	if (is_string_return) {
		env.push<string>(lua_vm, utilities::time::get_day_string(false));
	}
	else {
		env.push<int32_t>(lua_vm, utilities::time::get_day());
	}
	return 1;
}

auto lua_exports::get_hour(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	bool military = false;
	if (env.is(lua_vm, 1, lua::lua_type::boolean)) {
		military = env.get<bool>(lua_vm, 1);
	}
	env.push<int32_t>(lua_vm, utilities::time::get_hour(military));
	return 1;
}

auto lua_exports::get_minute(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, utilities::time::get_minute());
	return 1;
}

auto lua_exports::get_month(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, utilities::time::get_month());
	return 1;
}

auto lua_exports::get_nearest_minute(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<seconds>(lua_vm, utilities::time::get_distance_to_next_minute_mark(env.get<int32_t>(lua_vm, 1)));
	return 1;
}

auto lua_exports::get_second(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, utilities::time::get_second());
	return 1;
}

auto lua_exports::get_time(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<double>(lua_vm, static_cast<double>(time(nullptr)));
	return 1;
}

auto lua_exports::get_time_zone_offset(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, utilities::time::get_time_zone_offset());
	return 1;
}

auto lua_exports::get_week(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, utilities::time::get_week());
	return 1;
}

auto lua_exports::get_year(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, utilities::time::get_year(false));
	return 1;
}

auto lua_exports::is_dst(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<bool>(lua_vm, utilities::time::is_dst());
	return 1;
}

// Rates
auto lua_exports::get_drop_meso(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, channel_server::get_instance().get_config().rates.drop_meso);
	return 1;
}

auto lua_exports::get_drop_rate(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, channel_server::get_instance().get_config().rates.drop_rate);
	return 1;
}

auto lua_exports::get_exp_rate(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, channel_server::get_instance().get_config().rates.mob_exp_rate);
	return 1;
}

auto lua_exports::get_global_drop_meso(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, channel_server::get_instance().get_config().rates.global_drop_meso);
	return 1;
}

auto lua_exports::get_global_drop_rate(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, channel_server::get_instance().get_config().rates.global_drop_rate);
	return 1;
}

auto lua_exports::get_quest_exp_rate(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, channel_server::get_instance().get_config().rates.quest_exp_rate);
	return 1;
}

// Party
auto lua_exports::check_party_footholds(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	int8_t member_count = env.get<int8_t>(lua_vm, 1);
	party *p = get_player(lua_vm, env)->get_party();
	result verified = result::failure;
	if (p != nullptr) {
		vector<vector<game_foothold_id>> footholds = env.get<vector<vector<game_foothold_id>>>(lua_vm, 2);
		verified = p->check_footholds(member_count, footholds);
	}
	env.push<bool>(lua_vm, verified == result::successful);
	return 1;
}

auto lua_exports::get_all_party_player_ids(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	if (party *party = get_player(lua_vm, env)->get_party()) {
		env.push<vector<game_player_id>>(lua_vm, party->get_all_player_ids());
		return 1;
	}
	return 0;
}

auto lua_exports::get_party_count(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	int8_t member_count = 0;
	party *p = get_player(lua_vm, env)->get_party();
	if (p != nullptr) {
		member_count = p->get_members_count();
	}
	env.push<int8_t>(lua_vm, member_count);
	return 1;
}

auto lua_exports::get_party_id(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_party_id id = 0;
	party *p = get_player(lua_vm, env)->get_party();
	if (p != nullptr) {
		id = p->get_id();
	}
	env.push<game_party_id>(lua_vm, id);
	return 1;
}

auto lua_exports::get_party_map_count(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto player = get_player(lua_vm, env);
	party *p = player->get_party();
	int8_t members = 0;
	if (p != nullptr) {
		game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
		members = p->get_member_count_on_map(map_id);
	}
	env.push<int8_t>(lua_vm, members);
	return 1;
}

auto lua_exports::is_party_in_level_range(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto player = get_player(lua_vm, env);
	party *p = player->get_party();
	bool is_within = false;
	if (p != nullptr) {
		game_player_level low_bound = env.get<game_player_level>(lua_vm, 1);
		game_player_level high_bound = env.get<game_player_level>(lua_vm, 2);
		is_within = p->is_within_level_range(low_bound, high_bound);
	}
	env.push<bool>(lua_vm, is_within);
	return 1;
}

auto lua_exports::is_party_leader(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto player = get_player(lua_vm, env);
	party *p = player->get_party();
	bool is_leader = false;
	if (p != nullptr) {
		is_leader = player == p->get_leader();
	}
	env.push<bool>(lua_vm, is_leader);
	return 1;
}

auto lua_exports::verify_party_footholds(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	party *p = get_player(lua_vm, env)->get_party();
	result verified = result::failure;
	if (p != nullptr) {
		vector<vector<game_foothold_id>> footholds = env.get<vector<vector<game_foothold_id>>>(lua_vm, 1);
		verified = p->verify_footholds(footholds);
	}
	env.push<bool>(lua_vm, verified == result::successful);
	return 1;
}

auto lua_exports::warp_party(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	string to;
	if (env.is(lua_vm, 2, lua::lua_type::string)) {
		// Optional portal parameter
		string to = env.get<string>(lua_vm, 2);
	}
	auto player = get_player(lua_vm, env);
	party *p = player->get_party();
	if (p != nullptr) {
		p->warp_all_members(map_id, to);
	}
	return 0;
}

// Instance
auto lua_exports::add_instance_map(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	get_instance(lua_vm, env)->add_map(map_id);
	return 0;
}

auto lua_exports::add_instance_party(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_party_id id = env.get<game_party_id>(lua_vm, 1);
	if (party *p = channel_server::get_instance().get_player_data_provider().get_party(id)) {
		get_instance(lua_vm, env)->add_party(p);
	}
	return 0;
}

auto lua_exports::add_instance_player(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto player = get_player_deduced(1, lua_vm, env);
	get_instance(lua_vm, env)->add_player(player);
	return 0;
}

auto lua_exports::check_instance_timer(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string name = env.get<string>(lua_vm, 1);
	env.push<int32_t>(lua_vm, static_cast<int32_t>(get_instance(lua_vm, env)->get_timer_seconds_remaining(name).count()));
	return 1;
}

auto lua_exports::create_instance(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string name = env.get<string>(lua_vm, 1);
	seconds time = env.get<seconds>(lua_vm, 2);
	bool show_timer = env.get<bool>(lua_vm, 3);
	seconds persistent{0};
	game_map_id map = 0;
	game_player_id id = 0;
	auto player = get_player(lua_vm, env);
	if (env.is(lua_vm, 4, lua::lua_type::number)) {
		persistent = env.get<seconds>(lua_vm, 4);
	}
	if (player != nullptr) {
		map = player->get_map_id();
		id = player->get_id();
	}
	instance *inst = new instance(name, map, id, time, persistent, show_timer);
	channel_server::get_instance().get_instances().add_instance(inst);
	inst->begin_instance();

	if (inst->show_timer()) {
		inst->show_timer(true, true);
	}

	env.set<string>(lua_vm, "system_instance_name", name);
	return 0;
}

auto lua_exports::delete_instance_variable(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_instance(lua_vm, env)->get_variables()->delete_variable(env.get<string>(lua_vm, 1));
	return 0;
}

auto lua_exports::get_all_instance_player_ids(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<vector<game_player_id>>(lua_vm, get_instance(lua_vm, env)->get_all_player_ids());
	return 1;
}

auto lua_exports::get_instance_player_count(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<uint32_t>(lua_vm, get_instance(lua_vm, env)->get_player_num());
	return 1;
}

auto lua_exports::get_instance_player_id(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto player = get_player_deduced(1, lua_vm, env);
	env.push<game_player_id>(lua_vm, player->get_id());
	return 1;
}

auto lua_exports::get_instance_variable(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	variable_type::type return_type = variable_type::string;
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		return_type = (variable_type::type)env.get<int32_t>(lua_vm, 2);
	}
	string val = get_instance(lua_vm, env)->get_variables()->get_variable(env.get<string>(lua_vm, 1));
	push_get_variable_data(lua_vm, env, val, return_type);
	return 1;
}

auto lua_exports::is_instance(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<bool>(lua_vm, channel_server::get_instance().get_instances().is_instance(env.get<string>(lua_vm, 1)));
	return 1;
}

auto lua_exports::is_instance_map(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<bool>(lua_vm, get_instance(lua_vm, env)->is_instance_map(env.get<game_map_id>(lua_vm, 1)));
	return 1;
}

auto lua_exports::is_instance_persistent(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<bool>(lua_vm, get_instance(lua_vm, env)->get_persistence().count() != 0);
	return 1;
}

auto lua_exports::mark_for_delete(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_instance(lua_vm, env)->mark_for_delete();
	return 0;
}

auto lua_exports::move_all_players(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);

	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	if (env.is(lua_vm, 2, lua::lua_type::string)) {
		// Optional portal parameter
		string to = env.get<string>(lua_vm, 2);
		map *map = maps::get_map(map_id);
		const portal_info * const destination_portal = map->query_portal_name(to);
		get_instance(lua_vm, env)->move_all_players(map_id, true, destination_portal);
	}
	else {
		get_instance(lua_vm, env)->move_all_players(map_id, true);
	}
	return 0;
}

auto lua_exports::pass_players_between_instances(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);

	game_map_id map_id = env.get<game_map_id>(lua_vm, 1);
	if (env.is(lua_vm, 2, lua::lua_type::string)) {
		// Optional portal parameter
		string to = env.get<string>(lua_vm, 2);
		map *map = maps::get_map(map_id);
		const portal_info * const destination_portal = map->query_portal_name(to);
		get_instance(lua_vm, env)->move_all_players(map_id, false, destination_portal);
	}
	else {
		get_instance(lua_vm, env)->move_all_players(map_id, false);
	}
	return 0;
}

auto lua_exports::remove_all_instance_players(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_instance(lua_vm, env)->remove_all_players();
	return 0;
}

auto lua_exports::remove_instance_player(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto player = get_player_deduced(1, lua_vm, env);
	get_instance(lua_vm, env)->remove_player(player);
	return 0;
}

auto lua_exports::respawn_instance_mobs(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = vana::maps::no_map;
	if (env.is(lua_vm, 1, lua::lua_type::number)) {
		map_id = env.get<game_map_id>(lua_vm, 1);
	}
	get_instance(lua_vm, env)->respawn_mobs(map_id);
	return 0;
}

auto lua_exports::respawn_instance_reactors(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_map_id map_id = vana::maps::no_map;
	if (env.is(lua_vm, 1, lua::lua_type::number)) {
		map_id = env.get<game_map_id>(lua_vm, 1);
	}
	get_instance(lua_vm, env)->respawn_reactors(map_id);
	return 0;
}

auto lua_exports::revert_instance(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.set<string>(lua_vm, "system_instance_name", env.get<string>(lua_vm, "system_old_instance_name"));
	return 0;
}

auto lua_exports::set_instance(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string instance_name = env.get<string>(lua_vm, 1);
	instance *inst = channel_server::get_instance().get_instances().get_instance(instance_name);
	if (inst != nullptr) {
		string old_instance_name = env.get<string>(lua_vm, "system_instance_name");
		env.set<string>(lua_vm, "system_old_instance_name", old_instance_name);
		env.set<string>(lua_vm, "system_instance_name", instance_name);
	}
	env.push<bool>(lua_vm, inst != nullptr);
	return 1;
}

auto lua_exports::set_instance_persistence(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	seconds persistence = env.get<seconds>(lua_vm, 1);
	get_instance(lua_vm, env)->set_persistence(persistence);
	return 0;
}

auto lua_exports::set_instance_reset(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	bool reset = env.get<bool>(lua_vm, 1);
	get_instance(lua_vm, env)->set_reset_at_end(reset);
	return 0;
}

auto lua_exports::set_instance_variable(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	auto kvp = obtain_set_variable_pair(lua_vm, env);
	get_instance(lua_vm, env)->get_variables()->set_variable(kvp.first, kvp.second);
	return 0;
}

auto lua_exports::show_instance_time(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_instance(lua_vm, env)->show_timer(env.get<bool>(lua_vm, 1));
	return 0;
}

auto lua_exports::start_instance_future_timer(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string name = env.get<string>(lua_vm, 1);
	seconds time = env.get<seconds>(lua_vm, 2);
	seconds persistence{0};
	if (env.is(lua_vm, 3, lua::lua_type::number)) {
		persistence = env.get<seconds>(lua_vm, 3);
	}
	env.push<bool>(lua_vm, get_instance(lua_vm, env)->add_future_timer(name, time, persistence));
	return 1;
}

auto lua_exports::start_instance_second_of_hour_timer(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string name = env.get<string>(lua_vm, 1);
	int16_t second_of_hour = env.get<int16_t>(lua_vm, 2);
	seconds persistence{0};
	if (env.is(lua_vm, 3, lua::lua_type::number)) {
		persistence = env.get<seconds>(lua_vm, 3);
	}
	env.push<bool>(lua_vm, get_instance(lua_vm, env)->add_second_of_hour_timer(name, second_of_hour, persistence));
	return 1;
}

auto lua_exports::stop_all_instance_timers(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_instance(lua_vm, env)->remove_all_timers();
	return 0;
}

auto lua_exports::stop_instance_timer(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string name = env.get<string>(lua_vm, 1);
	get_instance(lua_vm, env)->remove_timer(name);
	return 0;
}

}
}