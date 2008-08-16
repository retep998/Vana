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
#include <unordered_map>

using std::list;
using std::tr1::unordered_map;

class Player;
enum Act;

class PlayerActiveBuffs {
public:
	PlayerActiveBuffs(Player *player) : m_player(player), m_combo(0) { }

	// Skill "acts"
	void addAct(int skill, Act act, short value, int time);
	void removeAct(int skill, Act act);
	void removeAct(int skill);
	void removeAct();

	// Combo attack
	void setCombo(char combo, bool sendPacket);
	void addCombo();
	int getCombo() const { return m_combo; }
private:
	Player *m_player;
	unordered_map<int, list<Act>> m_skill_acts;
	char m_combo;
};

#endif