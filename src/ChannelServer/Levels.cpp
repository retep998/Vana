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
#include "Levels.h"
#include "Player.h"
#include "LevelsPacket.h"
#include "PlayersPacket.h"
#include "Skills.h"
#include "Randomizer.h"
#include "ReadPacket.h"
#include <string>

using std::string;

uint32_t Levels::exps[200] = {15, 34, 57, 92, 135, 372, 560, 840, 1242, 1716, 2360, 3216, 4200,
	5460, 7050, 8840, 11040, 13716, 16680, 20216, 24402, 28980, 34320, 40512, 47216, 54900,
	63666, 73080, 83720, 95700, 108480, 122760, 138666, 155540, 174216, 194832, 216600, 240500,
	266682, 294216, 324240, 356916, 391160, 428280, 468450, 510420, 555680, 604416, 655200,
	709716, 748608, 789631, 832902, 878545, 926689, 977471, 1031036, 1087536, 1147132, 1209994,
	1276301, 1346242, 1420016, 1497832, 1579913, 1666492, 1757815, 1854143, 1955750, 2062925,
	2175973, 2295216, 2420993, 2553663, 2693603, 2841212, 2996910, 3161140, 3334370, 3517093,
	3709829, 3913127, 4127566, 4353756, 4592341, 4844001, 5109452, 5389449, 5684790, 5996316,
	6324914, 6671519, 7037118, 7422752, 7829518, 8258575, 8711144, 9188514, 9692044, 10223168,
	10783397, 11374327, 11997640, 12655110, 13348610, 14080113, 14851703, 15665576, 16524049,
	17429566, 18384706, 19392187, 20454878, 21575805, 22758159, 24005306, 25320796, 26708375,
	28171993, 29715818, 31344244, 33061908, 34873700, 36784778, 38800583, 40926854, 43169645,
	45535341, 48030677, 50662758, 53439077, 56367538, 59456479, 62714694, 66151459, 69776558,
	73600313, 77633610, 81887931, 86375389, 91108760, 96101520, 101367883, 106992842, 112782213,
	118962678, 125481832, 132358236, 139611467, 147262175, 155332142, 163844343, 172823012,
	182293713, 192283408, 202820538, 213935103, 225658746, 238024845, 251068606, 264827165,
	279339639, 294647508, 310794191, 327825712, 345790561, 364739883, 384727628, 405810702,
	428049128, 451506220, 476248760, 502347192, 529875818, 558913012, 589541445, 621848316,
	655925603, 691870326, 729784819, 769777027, 811960808, 856456260, 903390063, 952895838,
	1005114529, 1060194805, 1118293480, 1179575962, 1244216724, 1312399800, 1384319309,
	1460180007, 1540197871, 1624600714, 1713628833, 1807535693, 1906558648, 2011069705, 2121276324};

void Levels::giveEXP(Player *player, uint32_t exp, int8_t type) {
	if (player->getLevel() >= 200) // Do not give EXP to characters level 200 or over
		return;
	uint32_t cexp = player->getExp() + exp;
	if (exp != 0)
		LevelsPacket::showEXP(player, exp, type);
	uint8_t level = player->getLevel();
	if (cexp >= exps[level - 1]) {
		int16_t apgain = 0;
		int16_t spgain = 0;
		int16_t hpgain = 0;
		int16_t mpgain = 0;
		uint8_t levelsgained = 0;
		uint8_t levelsmax = ChannelServer::Instance()->getMaxMultiLevel();
		while (cexp >= exps[level - 1] && levelsgained < levelsmax) {
			cexp -= exps[player->getLevel() - 1];
			level++;
			levelsgained++;
			apgain += 5;
			int16_t job = player->getJob() / 100;
			int16_t x = 0;
			int16_t intt = player->getInt() / 10;
			switch (job) {
				case 0:
					hpgain += Randomizer::Instance()->randShort(4) + 12;
					mpgain += Randomizer::Instance()->randShort(2) + 10 + intt;
					break;
				case 1:
					if (player->getSkills()->getSkillLevel(1000001) > 0)
						x = Skills::skills[1000001][player->getSkills()->getSkillLevel(1000001)].x;
					hpgain += Randomizer::Instance()->randShort(4) + 24 + x;
					mpgain += Randomizer::Instance()->randShort(2) + 4 + intt;
					break;
				case 2:
					if (player->getSkills()->getSkillLevel(2000001) > 0)
						x = Skills::skills[2000001][player->getSkills()->getSkillLevel(2000001)].x;
					hpgain += Randomizer::Instance()->randShort(4) + 10;
					mpgain += Randomizer::Instance()->randShort(2) + 22 + 2 * x + intt;
					break;
				case 9: // GM
					hpgain += 150;
					mpgain += 150;
					break;
				default: // Will have to split 5 away when pirates are fully released
					hpgain += Randomizer::Instance()->randShort(4) + 20;
					mpgain += Randomizer::Instance()->randShort(2) + 14 + intt;
					break;
			}
			if (player->getJob() > 0)
				spgain += 3;
			if (level >= 200) { // Do not let people level past the level 200 cap
				cexp = 0;
				break;
			}
		}

		if (cexp >= exps[level - 1]) { // If the desired number of level ups have passed and they're still above, set it to where it should be
			cexp = exps[level - 1] - 1;
		}

		if (levelsgained) { // Check if the player has leveled up at all, it is possible that the user haven't level up if multi-level limit is 0
			player->modifyRMHP(hpgain);
			player->modifyRMMP(mpgain);
			player->setLevel(level);
			player->setAp(player->getAp() + apgain);
			player->setSp(player->getSp() + spgain);
			// Let hyperbody remain on if on during a level up, as it should
			int32_t skillid = 0;
			if (player->getActiveBuffs()->getActiveSkillLevel(1301007) > 0)
				skillid = 1301007;
			else if (player->getActiveBuffs()->getActiveSkillLevel(9101008) > 0) // GM Hyperbody, separating because any player may get a map-wide effect of GM Hyperbody
				skillid = 9101008;
			if (skillid > 0) {
				uint8_t hblevel = player->getActiveBuffs()->getActiveSkillLevel(skillid);
				player->setHyperBody(Skills::skills[skillid][hblevel].x, Skills::skills[skillid][hblevel].y);
			}
			else {
				player->setMHP(player->getRMHP());
				player->setMMP(player->getRMMP());
			}
			player->setHP(player->getMHP());
			player->setMP(player->getMMP());
			player->setLevelDate();
			if (player->getLevel() == 200 && !player->isGM()) {
				string message;
				message += "[Congrats] ";
				message += player->getName();
				message += " has reached Level 200! Congratulate ";
				message += player->getName();
				message += " on such an amazing achievement!";
				PlayersPacket::showMessageWorld(message, 6);
			}
		}
	}
	player->setExp(cexp);
}

void Levels::addStat(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int32_t type = packet->getInt();
	if (player->getAp() == 0) {
		// hacking
		return;
	}
	LevelsPacket::statOK(player);
	addStat(player, type);
}

void Levels::addStat(Player *player, int32_t type, bool isreset, bool issubtract) {
	int16_t maxstat = ChannelServer::Instance()->getMaxStats();
	int16_t mod = 1;
	if (issubtract)
		mod = -1;
	switch (type) {
		case 0x40:
			if (player->getStr() >= maxstat)
				return;
			player->setStr(player->getStr() + mod);
			break;
		case 0x80:
			if (player->getDex() >= maxstat)
				return;
			player->setDex(player->getDex() + mod);
			break;
		case 0x100:
			if (player->getInt() >= maxstat)
				return;
			player->setInt(player->getInt() + mod);
			break;
		case 0x200:
			if (player->getLuk() >= maxstat)
				return;
			player->setLuk(player->getLuk() + mod);
			break;
		case 0x800:
		case 0x2000: {
			if (type == 0x800 && player->getRMHP() >= 30000)
				return;
			else if (type == 0x2000 && player->getRMMP() >= 30000)
				return;
			if (issubtract && player->getHPMPAp() == 0) {
				// Hacking
				return;
			}
			int16_t job = player->getJob() / 100;
			int16_t hpgain = 0;
			int16_t mpgain = 0;
			int16_t y = 0;
			switch (job) {
				case 0: // Beginner
					hpgain = (isreset ? (issubtract ? -12 : 8) : Randomizer::Instance()->randShort(4) + 8);
					mpgain = (isreset ? (issubtract ? -8 : 6) : Randomizer::Instance()->randShort(2) + 6);
					break;
				case 1: // Warrior
					if (player->getSkills()->getSkillLevel(1000001) > 0)
						y = Skills::skills[1000001][player->getSkills()->getSkillLevel(1000001)].y;
					hpgain = (isreset ? (issubtract ? (-1 * (24 + y)) : 20) : (Randomizer::Instance()->randShort(4) + 20 + y));
					mpgain = (isreset ? (issubtract ? -4 : 2) : Randomizer::Instance()->randShort(2) + 2);
					break;
				case 2: // Magician
					if (player->getSkills()->getSkillLevel(2000001) > 0)
						y = Skills::skills[2000001][player->getSkills()->getSkillLevel(2000001)].y;
					hpgain = (isreset ? (issubtract ? -10 : 6) : Randomizer::Instance()->randShort(4) + 6);
					mpgain = (isreset ? (issubtract ? (-1 * (20 + 2 * y)) : 18) : (Randomizer::Instance()->randShort(2) + 18 + 2 * y));
					break;
				default: // Bowman, Thief, Pirate, GM
					hpgain = (isreset ? (issubtract ? -20 : 16) : Randomizer::Instance()->randShort(4) + 16);
					mpgain = (isreset ? (issubtract ? -12 : 10) : Randomizer::Instance()->randShort(2) + 10);
					break;
			}
			player->setHPMPAp(player->getHPMPAp() + mod);
			switch (type) {
				case 0x800: player->modifyRMHP(hpgain); break;
				case 0x2000: player->modifyRMMP(mpgain); break;
			}
			int32_t skillid = 0;
			if (player->getActiveBuffs()->getActiveSkillLevel(1301007) > 0)
				skillid = 1301007;
			else if (player->getActiveBuffs()->getActiveSkillLevel(9101008) > 0)
				skillid = 9101008;
			if (skillid > 0) {
				uint8_t hblevel = player->getActiveBuffs()->getActiveSkillLevel(skillid);
				player->setHyperBody(Skills::skills[skillid][hblevel].x, Skills::skills[skillid][hblevel].y);
			}
			else {
				player->setMHP(player->getRMHP());
				player->setMMP(player->getRMMP());
			}
			player->setHP(player->getHP());
			player->setMP(player->getMP());
			break;
		}
		default:
			// Hacking, one assumes
			break;
	}
	if (!isreset)
		player->setAp(player->getAp() - 1);
}