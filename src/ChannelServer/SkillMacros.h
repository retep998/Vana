/*
Copyright (C) 2008-2013 Vana Development Team

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

#include "Types.h"
#include <memory>
#include <string>
#include <unordered_map>

using std::string;
using std::shared_ptr;
using std::unordered_map;

class SkillMacros {
public:
	struct SkillMacro;

	SkillMacros();

	void add(uint8_t pos, SkillMacro *macro);
	SkillMacro * getSkillMacro(int8_t pos);
	int8_t getMax();

	void load(int32_t charId);
	void save(int32_t charId);
private:
	unordered_map<int8_t, shared_ptr<SkillMacro>> m_skillMacros;
	int8_t m_maxPos;
};

struct SkillMacros::SkillMacro {
	SkillMacro(const string &name, bool shout, int32_t skill1, int32_t skill2, int32_t skill3);
	string name;
	bool shout;
	int32_t skill1;
	int32_t skill2;
	int32_t skill3;
};

inline SkillMacros::SkillMacros() : m_maxPos(-1)
{
}

inline void SkillMacros::add(uint8_t pos, SkillMacro *macro) {
	m_skillMacros[pos].reset(macro);
	if (m_maxPos < pos) {
		m_maxPos = pos;
	}
}

inline SkillMacros::SkillMacro * SkillMacros::getSkillMacro(int8_t pos) {
	if (m_skillMacros.find(pos) != m_skillMacros.end()) {
		return m_skillMacros[pos].get();
	}
	return nullptr;
}

inline int8_t SkillMacros::getMax() {
	return m_maxPos;
}

inline SkillMacros::SkillMacro::SkillMacro(const string &name, bool shout, int32_t skill1, int32_t skill2, int32_t skill3) :
	name(name),
	shout(shout),
	skill1(skill1),
	skill2(skill2),
	skill3(skill3)
{
}