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
#include "characters.hpp"
#include "common/algorithm.hpp"
#include "common/client_ip.hpp"
#include "common/constant/equip_slot.hpp"
#include "common/constant/inventory.hpp"
#include "common/constant/item.hpp"
#include "common/constant/job/track.hpp"
#include "common/data/provider/curse.hpp"
#include "common/data/provider/equip.hpp"
#include "common/data/provider/valid_char.hpp"
#include "common/io/database.hpp"
#include "common/packet_reader.hpp"
#include "common/session.hpp"
#include "common/util/game_logic/inventory.hpp"
#include "common/util/game_logic/job.hpp"
#include "common/util/misc.hpp"
#include "login_server/login_packet.hpp"
#include "login_server/login_server.hpp"
#include "login_server/login_server_accept_packet.hpp"
#include "login_server/sync_packet.hpp"
#include "login_server/user.hpp"
#include "login_server/world.hpp"
#include "login_server/worlds.hpp"
#include <unordered_map>

namespace vana {
namespace login_server {

auto characters::load_equips(game_player_id id, vector<char_equip> &vec) -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT i.item_id, i.slot "
		<< "FROM " << db.make_table(vana::table::items) << " i "
		<< "WHERE "
		<< "	i.character_id = :id "
		<< "	AND i.inv = :inv "
		<< "	AND i.slot < 0 "
		<< "ORDER BY slot ASC",
		soci::use(id, "id"),
		soci::use(constant::inventory::equip, "inv"));

	for (const auto &row : rs) {
		char_equip equip;
		equip.id = row.get<game_item_id>("item_id");
		equip.slot = row.get<game_inventory_slot>("slot");
		vec.push_back(equip);
	}
}

auto characters::load_character(character &charc, const soci::row &row) -> void {
	charc.id = row.get<game_player_id>("character_id");
	charc.name = row.get<string>("name");
	charc.gender = row.get<game_gender_id>("gender");
	charc.skin = row.get<game_skin_id>("skin");
	charc.face = row.get<game_face_id>("face");
	charc.hair = row.get<game_hair_id>("hair");
	charc.level = row.get<game_player_level>("level");
	charc.job = row.get<game_job_id>("job");
	charc.str = row.get<game_stat>("str");
	charc.dex = row.get<game_stat>("dex");
	charc.intt = row.get<game_stat>("int");
	charc.luk = row.get<game_stat>("luk");
	charc.hp = row.get<game_health>("chp");
	charc.mhp = row.get<game_health>("mhp");
	charc.mp = row.get<game_health>("cmp");
	charc.mmp = row.get<game_health>("mmp");
	charc.ap = row.get<game_stat>("ap");
	charc.sp = row.get<game_stat>("sp");
	charc.exp = row.get<game_experience>("exp");
	charc.fame = row.get<game_fame>("fame");
	charc.map = row.get<game_map_id>("map");
	charc.pos = row.get<int8_t>("pos");

	if (vana::util::game_logic::job::get_job_track(charc.job) == constant::job::track::gm) {
		// GMs can't have their rank sent otherwise the client will crash
		charc.world_rank = 0;
		charc.world_rank_change = 0;
		charc.job_rank = 0;
		charc.job_rank_change = 0;
	}
	else {
		charc.world_rank = row.get<int32_t>("world_cpos");
		charc.world_rank_change = charc.world_rank - row.get<int32_t>("world_opos");
		charc.job_rank = row.get<int32_t>("job_cpos");
		charc.job_rank_change = charc.job_rank - row.get<int32_t>("job_opos");
	}
	load_equips(charc.id, charc.equips);
}

auto characters::show_all_characters(ref_ptr<user> user_value) -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT * "
		<< "FROM " << db.make_table(vana::table::characters) << " c "
		<< "WHERE c.account_id = :account ",
		soci::use(user_value->get_account_id(), "account"));

	hash_map<game_world_id, vector<character>> chars;
	uint32_t chars_num = 0;
	world *world_value;

	for (const auto &row : rs) {
		game_world_id world_id = row.get<game_world_id>("world_id");
		world_value = login_server::get_instance().get_worlds().get_world(world_id);
		if (world_value == nullptr || !world_value->is_connected()) {
			// World is not connected
			continue;
		}

		character charc;
		load_character(charc, row);
		chars[world_id].push_back(charc);
		chars_num++;
	}

	uint32_t unk = chars_num + (3 - chars_num % 3); // What I've observed
	user_value->send(packets::show_all_characters_info(static_cast<game_world_id>(chars.size()), unk));
	for (const auto &kvp : chars) {
		user_value->send(packets::show_view_all_characters(kvp.first, kvp.second));
	}
}

auto characters::show_characters(ref_ptr<user> user_value) -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	auto world_id = user_value->get_world_id();
	game_account_id account_id = user_value->get_account_id();
	if (!world_id.is_initialized()) {
		THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
	}

	soci::rowset<> rs = (sql.prepare
		<< "SELECT * "
		<< "FROM " << db.make_table(vana::table::characters) << " c "
		<< "WHERE c.account_id = :account AND c.world_id = :world ",
		soci::use(account_id, "account"),
		soci::use(world_id.get(), "world"));

	vector<character> chars;
	for (const auto &row : rs) {
		character charc;
		load_character(charc, row);
		chars.push_back(charc);
	}

	opt_int32_t max;
	sql.once
		<< "SELECT s.char_slots "
		<< "FROM " << db.make_table(vana::table::storage) << " s "
		<< "WHERE s.account_id = :account AND s.world_id = :world ",
		soci::use(account_id, "account"),
		soci::use(world_id.get(), "world"),
		soci::into(max);

	if (!sql.got_data() || !max.is_initialized()) {
		const auto &config = login_server::get_instance().get_worlds().get_world(world_id.get())->get_config();
		max = config.default_chars;
	}

	user_value->send(packets::show_characters(chars, max.get()));
}

auto characters::check_character_name(ref_ptr<user> user_value, packet_reader &reader) -> void {
	string name = reader.get<string>();
	if (!ext::in_range_inclusive<size_t>(name.size(), constant::character::min_name_size, constant::character::max_name_size)) {
		return;
	}

	if (name_invalid(name)) {
		user_value->send(packets::check_name(name, packets::check_name_errors::invalid));
	}
	else if (name_taken(name)) {
		user_value->send(packets::check_name(name, packets::check_name_errors::taken));
	}
	else {
		user_value->send(packets::check_name(name, packets::check_name_errors::none));
	}
}

auto characters::create_item(game_item_id item_id, ref_ptr<user> user_value, game_player_id char_id, game_inventory_slot slot, game_slot_qty amount) -> void {
	auto &db = vana::io::database::get_char_db();
	game_inventory inventory = vana::util::game_logic::inventory::get_inventory(item_id);
	auto world_id = user_value->get_world_id();
	if (!world_id.is_initialized()) {
		THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
	}
	item_db_info info{slot, char_id, user_value->get_account_id(), world_id.get(), item::inventory};

	if (inventory == constant::inventory::equip) {
		item equip{login_server::get_instance().get_equip_data_provider(), item_id, stat_variance::none, false};
		equip.database_insert(db, info);
	}
	else {
		item item{item_id, amount};
		item.database_insert(db, info);
	}
}

auto characters::create_character(ref_ptr<user> user_value, packet_reader &reader) -> void {
	if (user_value->get_status() != player_status::logged_in) {
		// Hacking
		return;
	}

	string name = reader.get<string>();
	if (!ext::in_range_inclusive<size_t>(name.size(), constant::character::min_name_size, constant::character::max_name_size)) {
		return;
	}

	// Let's check our char name again just to be sure
	if (name_invalid(name)) {
		user_value->send(packets::check_name(name, packets::check_name_errors::invalid));
		return;
	}
	if (name_taken(name)) {
		user_value->send(packets::check_name(name, packets::check_name_errors::taken));
		return;
	}

	game_face_id face = reader.get<game_face_id>();
	game_hair_id hair = reader.get<game_hair_id>();
	game_hair_id hair_color = reader.get<game_hair_id>();
	game_skin_id skin = static_cast<game_skin_id>(reader.get<int32_t>());
	game_item_id top = reader.get<game_item_id>();
	game_item_id bottom = reader.get<game_item_id>();
	game_item_id shoes = reader.get<game_item_id>();
	game_item_id weapon = reader.get<game_item_id>();
	game_gender_id gender = reader.get<game_gender_id>();

	if (!login_server::get_instance().get_valid_char_data_provider().is_valid_character(gender, hair, hair_color, face, skin, top, bottom, shoes, weapon, data::provider::valid_char::adventurer)) {
		// Hacking
		user_value->disconnect();
		return;
	}

	game_stat str = 12;
	game_stat dex = 5;
	game_stat intl = 4;
	game_stat luk = 4;

	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	optional<game_world_id> world_id = user_value->get_world_id();
	if (!world_id.is_initialized()) {
		THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
	}

	sql.once
		<< "INSERT INTO " << db.make_table(vana::table::characters) << " (name, account_id, world_id, face, hair, skin, gender, str, dex, `int`, luk) "
		<< "VALUES (:name, :account, :world, :face, :hair, :skin, :gender, :str, :dex, :int, :luk)",
		soci::use(name, "name"),
		soci::use(user_value->get_account_id(), "account"),
		soci::use(world_id.get(), "world"),
		soci::use(face, "face"),
		soci::use(hair + hair_color, "hair"),
		soci::use(skin, "skin"),
		soci::use(gender, "gender"),
		soci::use(str, "str"),
		soci::use(dex, "dex"),
		soci::use(intl, "int"),
		soci::use(luk, "luk");

	game_player_id id = db.get_last_id<game_player_id>();

	create_item(top, user_value, id, -constant::equip_slot::top);
	create_item(bottom, user_value, id, -constant::equip_slot::bottom);
	create_item(shoes, user_value, id, -constant::equip_slot::shoe);
	create_item(weapon, user_value, id, -constant::equip_slot::weapon);
	create_item(constant::item::beginners_guidebook, user_value, id, 1);

	soci::row row;
	sql.once
		<< "SELECT * "
		<< "FROM " << db.make_table(vana::table::characters) << " c "
		<< "WHERE c.character_id = :id",
		soci::use(id, "id"),
		soci::into(row);

	character charc;
	load_character(charc, row);
	user_value->send(packets::show_character(charc));
	login_server::get_instance().get_worlds().send(world_id.get(), packets::interserver::player::character_created(id));
}

auto characters::delete_character(ref_ptr<user> user_value, packet_reader &reader) -> void {
	if (user_value->get_status() != player_status::logged_in) {
		// Hacking
		return;
	}
	int32_t password = reader.get<int32_t>();
	game_player_id id = reader.get<game_player_id>();

	if (!owner_check(user_value, id)) {
		// Hacking
		return;
	}
	enum deletion_constants : int8_t {
		success = 0x00,
		incorrect_birthday = 0x12,
		no_guild_master = 0x16,
		no_proposing_chumps = 0x18,
		no_world_transfers = 0x1A,
	};

	uint8_t result = success;
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	optional<game_world_id> world_id;

	sql.once
		<< "SELECT world_id "
		<< "FROM " << db.make_table(vana::table::characters) << " c "
		<< "WHERE c.character_id = :char ",
		soci::use(id, "char"),
		soci::into(world_id);

	if (!sql.got_data() || !world_id.is_initialized()) {
		// ???
		return;
	}

	opt_int32_t del_password = user_value->get_char_delete_password();
	if (!del_password.is_initialized() || del_password.get() == password) {
		login_server::get_instance().get_worlds().run_function([&id, &world_id](world *world_value) -> bool {
			if (world_value->is_connected() && world_value->get_id() == world_id.get()) {
				// packets::interserver::remove_character(world_value->get_connection(), player_id);
				// For guilds
				return true;
			}
			return false;
		});

		sql.once
			<< "DELETE p "
			<< "FROM " << db.make_table(vana::table::pets) << " p "
			<< "INNER JOIN " << db.make_table(vana::table::items) << " i ON p.pet_id = i.pet_id "
			<< "WHERE i.character_id = :char ",
			soci::use(id, "char");

		sql.once
			<< "DELETE FROM " << db.make_table(vana::table::characters) << " "
			<< "WHERE character_id = :char ",
			soci::use(id, "char");
	}
	else {
		result = incorrect_birthday;
	}

	user_value->send(packets::delete_character(id, result));
	login_server::get_instance().get_worlds().send(world_id.get(), packets::interserver::player::character_deleted(id));
}

auto characters::connect_game(ref_ptr<user> user_value, game_player_id char_id) -> void {
	if (user_value->get_status() != player_status::logged_in) {
		// Hacking
		return;
	}
	if (!owner_check(user_value, char_id)) {
		// Hacking
		return;
	}

	auto world_id = user_value->get_world_id();
	if (!world_id.is_initialized()) {
		THROW_CODE_EXCEPTION(codepath_invalid_exception, "!world_id.is_initialized()");
	}

	auto world = login_server::get_instance().get_worlds().get_world(world_id.get());
	world->send(packets::interserver::player_connecting_to_channel(user_value->get_channel(), char_id, user_value->get_ip().get(ip{0})));

	optional<client_ip> ip_value;
	optional<connection_port> port;
	if (channel *chan = world->get_channel(user_value->get_channel())) {
		ip_value = client_ip{chan->match_ip_to_subnet(user_value->get_ip().get(ip{0}))};
		port = chan->get_port();
	}
	user_value->send(packets::connect_ip(ip_value, port, char_id));
}

auto characters::connect_game(ref_ptr<user> user_value, packet_reader &reader) -> void {
	game_player_id id = reader.get<game_player_id>();

	connect_game(user_value, id);
}

auto characters::connect_game_world_from_view_all_characters(ref_ptr<user> user_value, packet_reader &reader) -> void {
	if (user_value->get_status() != player_status::logged_in) {
		// Hacking
		return;
	}
	game_player_id id = reader.get<game_player_id>();
	game_world_id world_id = static_cast<game_world_id>(reader.get<int32_t>());
	user_value->set_world_id(world_id);

	// We need to do 2 validations here
	// One to ensure that we're picking a character for the account
	// And another to ensure that the world matches the expected world
	optional<game_world_id> char_world_id;
	optional<game_account_id> account_id;
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	sql.once
		<< "SELECT world_id, account_id "
		<< "FROM " << db.make_table(vana::table::characters) << " c "
		<< "WHERE c.character_id = :char ",
		soci::use(id, "char"),
		soci::into(char_world_id),
		soci::into(account_id);

	if (!sql.got_data() || !char_world_id.is_initialized() || !account_id.is_initialized()) {
		// ???
		// Most likely hacking
		return;
	}

	if (account_id != user_value->get_account_id() || char_world_id != world_id) {
		// Hacking
		return;
	}

	// TODO FIXME packet
	// We need the packet for failure here - it's possible that there are no channels available

	auto world = login_server::get_instance().get_worlds().get_world(world_id);
	if (world->get_channel_count() == 0) {
		// Packet
		return;
	}

	// Take the player to a random channel
	game_channel_id chan_id = world->get_random_channel();
	user_value->set_channel(chan_id);

	connect_game(user_value, id);
}

auto characters::owner_check(ref_ptr<user> user_value, int32_t id) -> bool {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	opt_int32_t exists;

	sql.once
		<< "SELECT 1 "
		<< "FROM " << db.make_table(vana::table::characters) << " c "
		<< "WHERE c.character_id = :char AND c.account_id = :account "
		<< "LIMIT 1 ",
		soci::use(id, "char"),
		soci::use(user_value->get_account_id(), "account"),
		soci::into(exists);

	return sql.got_data() && exists.is_initialized();
}

auto characters::name_taken(const string &name) -> bool {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	opt_int32_t exists;

	sql.once
		<< "SELECT 1 "
		<< "FROM " << db.make_table(vana::table::characters) << " c "
		<< "WHERE c.name = :name "
		<< "LIMIT 1",
		soci::use(name, "name"),
		soci::into(exists);

	return sql.got_data() && exists.is_initialized();
}

auto characters::name_invalid(const string &name) -> bool {
	return
		login_server::get_instance().get_valid_char_data_provider().is_forbidden_name(name) ||
		login_server::get_instance().get_curse_data_provider().is_curse_word(name);
}

}
}