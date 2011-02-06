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
#pragma once

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <boost/tr1/memory.hpp>
#include <string>

using std::string;
using std::tr1::shared_ptr;
using std::tr1::unordered_map;

class SkillMacros {
public:
	struct SkillMacro;

	SkillMacros();

	void add(uint8_t pos, SkillMacro *macro);
	SkillMacro * getSkillMacro(int8_t pos);
	int8_t getMax();

	void load(int32_t charid);
	void save(int32_t charid);
private:
	unordered_map<int8_t, shared_ptr<SkillMacro> > skillMacros;
	int8_t maxPos;
};

struct SkillMacros::SkillMacro {
	SkillMacro(const string &name, bool shout, int32_t skill1, int32_t skill2, int32_t skill3);
	string name;
	bool shout;
	int32_t skill1;
	int32_t skill2;
	int32_t skill3;
};

inline SkillMacros::SkillMacros() : maxPos(-1) { }

inline void SkillMacros::add(uint8_t pos, SkillMacro *macro) {
	skillMacros[pos].reset(macro);
	if (maxPos < pos) {
		maxPos = pos;
	}
}

inline SkillMacros::SkillMacro * SkillMacros::getSkillMacro(int8_t pos) {
	if (skillMacros.find(pos) != skillMacros.end()) {
		return skillMacros[pos].get();
	}
	return nullptr;
}

inline int8_t SkillMacros::getMax() {
	return maxPos;
}

inline SkillMacros::SkillMacro::SkillMacro(const string &name, bool shout, int32_t skill1, int32_t skill2, int32_t skill3) : name(name), shout(shout), skill1(skill1), skill2(skill2), skill3(skill3) { }
