#ifndef SKILLSPACK_H
#define SKILLSPACK_H

class Player;

#include <vector>
using namespace std;
struct SkillActiveInfo;

namespace SkillsPacket {
	void addSkill(Player* player, int skillid, int level);
	void showSkill(Player* player, vector <Player*> players, int skillid);
	void useSkill(Player* player, vector <Player*> players, int skillid, int time, SkillActiveInfo pskill, SkillActiveInfo mskill);
	void endSkill(Player* player, vector <Player*> players, SkillActiveInfo pskill, SkillActiveInfo mskill);
	void healHP(Player* player, short hp);
};

#endif