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
#ifndef PLAYERACTIVEBUFF_H
#define PLAYERACTIVEBUFF_H

#include <list>
#include <memory>
#include <unordered_map>

using std::list;
using std::tr1::shared_ptr;
using std::tr1::unordered_map;

class Player;
enum Act;

namespace Timer {
	class Container;
};

class PlayerActiveBuffs {
public:
	PlayerActiveBuffs(Player *player) : m_player(player), m_combo(0), m_berserk(false) { }

	// Buff Skills
	void addBuff(int skill, unsigned char level);
	void removeBuff(int skill, bool fromTimer = false);
	void removeBuff();
	int buffTimeLeft(int skill);

	// Skill "acts"
	void addAct(int skill, Act act, short value, int time);
	Timer::Container * getActTimer(int skill);
	void removeAct(int skill);

	// Combo attack
	void setCombo(char combo, bool sendPacket);
	void addCombo();
	int getCombo() const { return m_combo; }

	// Berserk
	bool getBerserk() const { return m_berserk; }
	void checkBerserk(bool display = false);
private:
	Player *m_player;
	list<int> m_buffs;
	unordered_map<int, shared_ptr<Timer::Container>> m_skill_acts;
	char m_combo;
	bool m_berserk;
};

#endif