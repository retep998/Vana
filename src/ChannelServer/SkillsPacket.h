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
#ifndef SKILLSPACK_H
#define SKILLSPACK_H

#include <vector>

using std::vector;

class Player;
struct SkillActiveInfo;
struct SpecialSkillInfo;

namespace SkillsPacket {
	void addSkill(Player *player, int skillid, int level, int maxlevel = 0);
	void showSkill(Player *player, int skillid, unsigned char level);
	void showCombo(Player *player, int time); // Combo Attack
	void useSkill(Player *player, int skillid, int time, SkillActiveInfo pskill, SkillActiveInfo mskill, short addedinfo);
	void endSkill(Player *player, SkillActiveInfo pskill, SkillActiveInfo mskill);
	void healHP(Player *player, short hp);
	void showSkillEffect(Player *player, int skillid);
	void showSpecialSkill(Player *player, SpecialSkillInfo info); // Hurricane/Pierce/Big Bang/Monster Magnet
	void endSpecialSkill(Player *player, SpecialSkillInfo info);
	void showMagnetSuccess(Player *player, int mapmobid, unsigned char success); // New packet in .56
	void sendCooldown(Player *player, int skillid, short time);
};

#endif
