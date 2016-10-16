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
#include "beauty.hpp"
#include "common/algorithm.hpp"
#include "common/constant/gender.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/util/game_logic/player.hpp"
#include "common/util/randomizer.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>

namespace vana {
namespace data {
namespace provider {

auto beauty::load_data() -> void {
	load_skins();

	m_male.clear();
	m_female.clear();

	load_hair();
	load_faces();
}

auto beauty::load_skins() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Skins... ";
	m_skins.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::character_skin_data) << " ORDER BY skinid ASC");

	for (const auto &row : rs) {
		m_skins.push_back(row.get<game_skin_id>("skinid"));
	}

	std::cout << "DONE" << std::endl;
}

auto beauty::load_hair() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Hair... ";

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::character_hair_data) << " ORDER BY hairid ASC");

	for (const auto &row : rs) {
		game_gender_id gender_id = vana::util::game_logic::player::get_gender_id(row.get<string>("gender"));
		game_hair_id hair = row.get<game_hair_id>("hairid");
		auto &gender = gender_id == constant::gender::female ? m_female : m_male;
		gender.hair.push_back(hair);
	}

	std::cout << "DONE" << std::endl;
}

auto beauty::load_faces() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Faces... ";

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::character_face_data) << " ORDER BY faceid ASC");

	for (const auto &row : rs) {
		game_gender_id gender_id = vana::util::game_logic::player::get_gender_id(row.get<string>("gender"));
		game_face_id face = row.get<game_face_id>("faceid");
		auto &gender = gender_id == constant::gender::female ? m_female : m_male;
		gender.faces.push_back(face);
	}

	std::cout << "DONE" << std::endl;
}

auto beauty::get_random_skin() const -> game_skin_id {
	return *vana::util::randomizer::select(m_skins);
}

auto beauty::get_random_hair(game_gender_id gender_id) const -> game_hair_id {
	return *vana::util::randomizer::select(get_gender(gender_id).hair);
}

auto beauty::get_random_face(game_gender_id gender_id) const -> game_face_id {
	return *vana::util::randomizer::select(get_gender(gender_id).faces);
}

auto beauty::get_skins() const -> const vector<game_skin_id> & {
	return m_skins;
}

auto beauty::get_hair(game_gender_id gender_id) const -> const vector<game_hair_id> & {
	return get_gender(gender_id).hair;
}

auto beauty::get_faces(game_gender_id gender_id) const -> const vector<game_face_id> & {
	return get_gender(gender_id).faces;
}

auto beauty::is_valid_hair(game_gender_id gender_id, game_hair_id hair) const -> bool {
	const auto &gender = get_gender(gender_id);
	return ext::any_of(gender.hair, [hair](int32_t test_hair) { return test_hair == hair; });
}

auto beauty::is_valid_face(game_gender_id gender_id, game_face_id face) const -> bool {
	const auto &gender = get_gender(gender_id);
	return ext::any_of(gender.faces, [face](int32_t test_face) { return test_face == face; });
}

auto beauty::is_valid_skin(game_skin_id skin) const -> bool {
	return ext::any_of(m_skins, [skin](game_skin_id test_skin) { return test_skin == skin; });
}

auto beauty::get_gender(game_gender_id gender_id) const -> const valid_look_data & {
	return gender_id == constant::gender::female ? m_female : m_male;
}

}
}
}