#ifndef SKILLSPACK_H
#define SKILLSPACK_H

class Player;

#include <vector>
using namespace std;
struct SkillActiveInfo;

class SkillsPacket {
public:
	static void addSkill(Player* player, int skillid, int level);
	static void showSkill(Player* player, vector <Player*> players, int skillid);
	static void useSkill(Player* player, vector <Player*> players, int skillid, int time, SkillActiveInfo pskill, SkillActiveInfo mskill);
	static void endSkill(Player* player, vector <Player*> players, SkillActiveInfo pskill, SkillActiveInfo mskill);
	static void healHP(Player* player, short hp);
};

#endif