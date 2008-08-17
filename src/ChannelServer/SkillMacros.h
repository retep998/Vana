/*
Copyright (C) 2008 Vana Development Team

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
#ifndef SKILLMACROS_H
#define SKILLMACROS_H

#include <unordered_map>
#include <memory>
#include <string>

using std::string;
using std::tr1::shared_ptr;
using std::tr1::unordered_map;

class SkillMacros {
public:
	struct SkillMacro;

	SkillMacros();

	void add(int pos, SkillMacro *macro);
	SkillMacro * getSkillMacro(int pos);
	int getMax();

	void load(int charid);
	void save(int charid);
private:
	unordered_map<int, shared_ptr<SkillMacro>> skillMacros;
	int maxPos;
};

struct SkillMacros::SkillMacro {
	SkillMacro(const string &name, bool shout, int skill1, int skill2, int skill3);
	string name;
	bool shout;
	int skill1;
	int skill2;
	int skill3;
};

inline SkillMacros::SkillMacros() : maxPos(-1) { }

inline void SkillMacros::add(int pos, SkillMacro *macro) {
	skillMacros[pos].reset(macro);
	if (maxPos < pos) {
		maxPos = pos;
	}
}

inline SkillMacros::SkillMacro * SkillMacros::getSkillMacro(int pos) {
	if (skillMacros.find(pos) != skillMacros.end()) {
		return skillMacros[pos].get();
	}
	else {
		return 0;
	}
}

inline int SkillMacros::getMax() {
	return maxPos;
}

inline SkillMacros::SkillMacro::SkillMacro(const string &name, bool shout, int skill1, int skill2, int skill3) : name(name), shout(shout), skill1(skill1), skill2(skill2), skill3(skill3) { }

#endif
