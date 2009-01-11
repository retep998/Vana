/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef PLAYERACTIVEBUFF_H
#define PLAYERACTIVEBUFF_H

#include "Buffs.h"
#include "Types.h"
#include <list>
#include <boost/tr1/unordered_map.hpp>
#include <vector>

using std::list;
using std::tr1::shared_ptr;
using std::tr1::unordered_map;
using std::vector;

class Player;

namespace Timer {
	class Container;
};

class PlayerActiveBuffs {
public:
	PlayerActiveBuffs(Player *player) : m_player(player), m_combo(0), m_berserk(false) { }

	// Buff Skills
	void addBuff(int32_t skill, int32_t time);
	void removeBuff(int32_t skill, bool fromTimer = false);
	void removeBuff();
	int32_t buffTimeLeft(int32_t skill);

	// Skill "acts"
	void addAct(int32_t skill, Act act, int16_t value, int32_t time);
	Timer::Container * getActTimer(int32_t skill);
	void removeAct(int32_t skill);

	// Combo attack
	void setCombo(uint8_t combo, bool sendPacket);
	void addCombo();
	uint8_t getCombo() const { return m_combo; }

	// Berserk
	bool getBerserk() const { return m_berserk; }
	void checkBerserk(bool display = false);

	// Map garbage
	void deleteSkillMapEnterInfo(int32_t skillid);
	SkillMapEnterActiveInfo getSkillMapEnterInfo();
	SkillActiveInfo getBuffInfo(int32_t skillid);
	uint8_t getActiveSkillLevel(int32_t skillid);
	void setBuffInfo(int32_t skillid, SkillActiveInfo skill);
	void setSkillMapEnterInfo(int32_t skillid, const vector<SkillMapActiveInfo> &skill);
	void setActiveSkillLevel(int32_t skillid, uint8_t level);
private:
	Player *m_player;
	list<int32_t> m_buffs;
	unordered_map<int32_t, SkillActiveInfo> activeplayerskill;
	vector<SkillMapActiveInfo> activemapenterskill;
	unordered_map<int32_t, uint8_t> activelevels;
	unordered_map<int32_t, shared_ptr<Timer::Container> > m_skill_acts;
	uint8_t m_combo;
	bool m_berserk;
};

#endif
