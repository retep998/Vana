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
#include "Maps.h"
#include "LevelsPacket.h"
#include "SkillsPacket.h"
#include "Skills.h"
#include "Randomizer.h"
#include "BufferUtilities.h"
#include "ReadPacket.h"

int Levels::exps[200] = {15, 34, 57, 92, 135, 372, 560, 840, 1242, 1716, 2360, 3216, 4200,
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

void Levels::giveEXP(Player *player, long exp, char type) {
	if (player->getLevel() >= 200) return; // Do not give EXP to characters level 200 or over
	long cexp = player->getExp() + exp;
	if (cexp<0)
		cexp = cexp*(-1);
	if (exp!=0)
		LevelsPacket::showEXP(player, exp, type);
	player->setExp(cexp, 1);
	if (cexp >= exps[player->getLevel()-1]) {
		while (cexp>=exps[player->getLevel()-1]) {
			if (player->getLevel()>=200) { // Do not let people level past the level 200 cap
				player->setExp(0, 0);
				break;
			}
			cexp -= exps[player->getLevel()-1];
			player->setExp(cexp, 0);
			player->setLevel(player->getLevel()+1);
			player->setAp(player->getAp()+5);
			int job = player->getJob() / 100;
			short hpgain = 0;
			short mpgain = 0;
			short intt = player->getInt() / 10;
			if (job == 0) {
				hpgain = Randomizer::Instance()->randInt(4) + 12;
				mpgain = Randomizer::Instance()->randInt(2) + 10 + intt;
			}
			else if (job == 1) {
				int x = 0;
				if (player->skills->getSkillLevel(1000001) > 0) { x = Skills::skills[1000001][player->skills->getSkillLevel(1000001)].x; }
				hpgain = Randomizer::Instance()->randInt(4) + 24 + x;
				mpgain = Randomizer::Instance()->randInt(2) + 4 + intt;
			}
			else if (job == 2) {
				int x = 0;
				if (player->skills->getSkillLevel(2000001) > 0) { x = Skills::skills[2000001][player->skills->getSkillLevel(2000001)].x; }
				hpgain = Randomizer::Instance()->randInt(4) + 10;
				mpgain = Randomizer::Instance()->randInt(2) + 22 + 2 * x + intt;
			} 
			else if (job == 5) {
				hpgain = 150;
				mpgain = 150;
			}
			else {
				hpgain = Randomizer::Instance()->randInt(4) + 20;
				mpgain = Randomizer::Instance()->randInt(2) + 14 + intt;
			}
			player->setRMHP(player->getRMHP() + hpgain);
			player->setRMMP(player->getRMMP() + mpgain);
			LevelsPacket::levelUP(player, Maps::maps[player->getMap()]->getPlayers());
			if (player->getJob() > 0) {
				player->setSp(player->getSp()+3);
			}
		}
		// Let hyperbody remain on if on during a level up, as it should
		if (player->skills->getActiveSkillLevel(1301007)>0) {
			player->setMHP(player->getRMHP()*(100 + Skills::skills[1301007][player->skills->getActiveSkillLevel(1301007)].x)/100);
			player->setMMP(player->getRMMP()*(100 + Skills::skills[1301007][player->skills->getActiveSkillLevel(1301007)].y)/100);
		}
		else{
			player->setMHP(player->getRMHP());
			player->setMMP(player->getRMMP());
		}
		player->setHP(player->getMHP());
		player->setMP(player->getMMP());
	}
}

void Levels::addStat(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int type = packet->getInt();
	if (player->getAp() == 0) {
		// hacking
		return;
	}
	LevelsPacket::statOK(player);
	switch (type) {
		case 0x40:
			if (player->getStr() > 998) return;
			player->setStr(player->getStr()+1);
			player->setAp(player->getAp()-1);
			break;
		case 0x80:
			if (player->getDex() > 998) return;
			player->setDex(player->getDex()+1);
			player->setAp(player->getAp()-1);
			break;
		case 0x100:
			if (player->getInt() > 998) return;
			player->setInt(player->getInt()+1);
			player->setAp(player->getAp()-1);
			break;
		case 0x200:
			if (player->getLuk() > 998) return;
			player->setLuk(player->getLuk()+1);
			player->setAp(player->getAp()-1);
			break;
		case 0x800:
		case 0x2000: {
			if ((player->getRMHP() > 29999 && type == 0x800) || (player->getRMMP() > 29999 && type == 0x2000)) return;
			int job = player->getJob() / 100;
			short hpgain = 0;
			short mpgain = 0;
			switch (job) {
				case 0:
					hpgain = Randomizer::Instance()->randInt(4) + 8;
					mpgain = Randomizer::Instance()->randInt(2) + 10;
					break;
				case 1: {
					int y = 0;
					if (player->skills->getSkillLevel(1000001) > 0) { y = Skills::skills[1000001][player->skills->getSkillLevel(1000001)].y; }
					hpgain = Randomizer::Instance()->randInt(4) + 20 + y;
					mpgain = Randomizer::Instance()->randInt(2) + 2;
					break;
				}
				case 2: {
					int y = 0;
					if (player->skills->getSkillLevel(2000001) > 0) { y = Skills::skills[2000001][player->skills->getSkillLevel(2000001)].y; }
					hpgain = Randomizer::Instance()->randInt(4) + 6;
					mpgain = Randomizer::Instance()->randInt(2) + 18 + 2 * y;
					break;
				}
				default:
					hpgain = Randomizer::Instance()->randInt(4) + 16;
					mpgain = Randomizer::Instance()->randInt(2) + 10;
					break;
			}
			player->setAp(player->getAp()-1);
			player->setHPMPAp(player->getHPMPAp()+1);
			int hb = player->skills->getActiveSkillLevel(1301007);
			switch (type) {
				case 0x800:
					player->setRMHP(player->getRMHP() + hpgain);
					player->setMHP(player->getRMHP() * (hb ? (Skills::skills[1301007][hb].x / 100) : 1));
					break;
				case 0x2000:
					player->setRMMP(player->getRMMP() + mpgain);
					player->setMMP(player->getRMMP() * (hb ? (Skills::skills[1301007][hb].y / 100) : 1));
					break;
			}
			break;
		}
		default:
			break;
	}
}

void Levels::setLevel(Player *player, int level) {
	player->setLevel(level);
	LevelsPacket::levelUP(player, Maps::maps[player->getMap()]->getPlayers());
}

void Levels::setJob(Player *player, int job) {
	player->setJob(job);
	LevelsPacket::jobChange(player, Maps::maps[player->getMap()]->getPlayers());
}
