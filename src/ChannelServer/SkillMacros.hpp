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
#pragma once

#include "Types.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class SkillMacros {
public:
	struct SkillMacro;

	auto add(uint8_t pos, SkillMacro *macro) -> void;
	auto getSkillMacro(int8_t pos) -> SkillMacro *;
	auto getMax() -> int8_t;

	auto load(player_id_t charId) -> void;
	auto save(player_id_t charId) -> void;
private:
	int8_t m_maxPoint = -1;
	hash_map_t<int8_t, ref_ptr_t<SkillMacro>> m_skillMacros;
};

struct SkillMacros::SkillMacro {
	NONCOPYABLE(SkillMacro);
	NO_DEFAULT_CONSTRUCTOR(SkillMacro);
public:
	SkillMacro(const string_t &name, bool shout, skill_id_t skill1, skill_id_t skill2, skill_id_t skill3);

	bool shout = false;
	skill_id_t skill1 = 0;
	skill_id_t skill2 = 0;
	skill_id_t skill3 = 0;
	string_t name;
};

inline
auto SkillMacros::add(uint8_t pos, SkillMacro *macro) -> void {
	m_skillMacros[pos].reset(macro);
	if (m_maxPoint < pos) {
		m_maxPoint = pos;
	}
}

inline
SkillMacros::SkillMacro * SkillMacros::getSkillMacro(int8_t pos) {
	if (m_skillMacros.find(pos) != std::end(m_skillMacros)) {
		return m_skillMacros[pos].get();
	}
	return nullptr;
}

inline
auto SkillMacros::getMax() -> int8_t {
	return m_maxPoint;
}

inline
SkillMacros::SkillMacro::SkillMacro(const string_t &name, bool shout, skill_id_t skill1, skill_id_t skill2, skill_id_t skill3) :
	name{name},
	shout{shout},
	skill1{skill1},
	skill2{skill2},
	skill3{skill3}
{
}