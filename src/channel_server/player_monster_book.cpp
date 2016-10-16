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
#include "player_monster_book.hpp"
#include "common/data/provider/item.hpp"
#include "common/io/database.hpp"
#include "common/util/game_logic/monster_card.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/monster_book_packet.hpp"
#include "channel_server/player.hpp"

namespace vana {
namespace channel_server {

player_monster_book::player_monster_book(ref_ptr<player> player) :
	m_player{player}
{
	load();
}

auto player_monster_book::load() -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	if (auto player = m_player.lock()) {
		game_player_id char_id = player->get_id();

		soci::rowset<> rs = (sql.prepare
			<< "SELECT b.card_id, b.level "
			<< "FROM " << db.make_table(vana::table::monster_book) << " b "
			<< "WHERE b.character_id = :char "
			<< "ORDER BY b.card_id ASC",
			soci::use(char_id, "char"));

		for (const auto &row : rs) {
			add_card(row.get<game_item_id>("card_id"), row.get<uint8_t>("level"), true);
		}

		calculate_level();
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_monster_book::save() -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	if (auto player = m_player.lock()) {
		game_player_id char_id = player->get_id();

		sql.once << "DELETE FROM " << db.make_table(vana::table::monster_book) << " WHERE character_id = :char",
			soci::use(char_id, "char");

		if (m_cards.size() > 0) {
			game_item_id card_id = 0;
			uint8_t level = 0;

			soci::statement st = (sql.prepare
				<< "INSERT INTO " << db.make_table(vana::table::monster_book) << " "
				<< "VALUES (:char, :card, :level) ",
				soci::use(char_id, "char"),
				soci::use(card_id, "card"),
				soci::use(level, "level"));

			for (const auto &kvp : m_cards) {
				const monster_card &c = kvp.second;
				card_id = c.id;
				level = c.level;
				st.execute(true);
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_monster_book::get_card_level(int32_t card_id) -> uint8_t {
	return m_cards[card_id].level;
}

auto player_monster_book::add_card(int32_t card_id, uint8_t level, bool initial_load) -> bool {
	if (m_cards.find(card_id) == std::end(m_cards)) {
		if (vana::util::game_logic::monster_card::is_special_card(card_id)) {
			++m_special_count;
		}
		else {
			++m_normal_count;
		}
	}

	if (initial_load) {
		monster_card card(card_id, level);
		m_cards[card_id] = card;
	}
	else {
		auto kvp = m_cards.find(card_id);
		monster_card card = kvp != std::end(m_cards) ? kvp->second : monster_card(card_id, 0);
		if (is_full(card_id)) {
			return true;
		}
		++card.level;
		m_cards[card_id] = card;
		if (card.level == 1) {
			calculate_level();
		}
		return false;
	}
	return false;
}

auto player_monster_book::connect_packet(packet_builder &builder) -> void {
	if (get_cover() != 0) {
		optional<game_item_id> cover_id = channel_server::get_instance().get_item_data_provider().get_card_id(get_cover());
		if (cover_id.is_initialized()) {
			builder.add<int32_t>(cover_id.get());
		}
		else {
			// ???
			// Shouldn't happen, server screwed up in modification somewhere
			builder.add<int32_t>(0);
		}
	}
	else {
		builder.add<int32_t>(0);
	}
	builder.unk<int8_t>();

	builder.add<uint16_t>(static_cast<uint16_t>(m_cards.size()));
	for (const auto &kvp : m_cards) {
		builder.add<int16_t>(vana::util::game_logic::monster_card::get_card_short_id(kvp.second.id));
		builder.add<int8_t>(kvp.second.level);
	}
}

auto player_monster_book::calculate_level() -> void {
	int32_t size = get_size();
	m_level = constant::monster_card::max_player_level;
	for (int32_t i = 1; i < constant::monster_card::max_player_level; i++) {
		// We don't calculate for the last level because that includes all values above the second to last level
		if (size < constant::monster_card::player_levels[i - 1]) {
			m_level = i;
			break;
		}
	}
}

auto player_monster_book::info_packet(packet_builder &builder) -> void {
	builder.add<int32_t>(get_level());
	builder.add<int32_t>(get_normals());
	builder.add<int32_t>(get_specials());
	builder.add<int32_t>(get_size());
	builder.add<int32_t>(get_cover());
}

auto player_monster_book::get_card(int32_t card_id) -> monster_card * {
	auto kvp = m_cards.find(card_id);
	return kvp != std::end(m_cards) ? &kvp->second : nullptr;
}

auto player_monster_book::is_full(int32_t card_id) -> bool {
	auto kvp = m_cards.find(card_id);
	return kvp != std::end(m_cards) ? (kvp->second.level == constant::monster_card::max_card_level) : false;
}

}
}