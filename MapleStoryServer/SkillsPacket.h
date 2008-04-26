#ifndef SKILLSPACK_H
#define SKILLSPACK_H

class Player;

#include <vector>
using namespace std;
struct SkillActiveInfo;

short getShort(unsigned char* buf);
int getInt(unsigned char* buf);
void getString(unsigned char* buf, int len, char* out);

class SkillsPacket {
public:
	static void addSkill(Player* player, int skillid, int level);
	static void showSkill(Player* player, vector <Player*> players, int skillid);
	static void useSkill(Player* player, vector <Player*> players, int skillid, int time, SkillActiveInfo pskill, SkillActiveInfo mskill);
	static void endSkill(Player* player, vector <Player*> players, SkillActiveInfo pskill, SkillActiveInfo mskill);
	static void healHP(Player* player, short hp);
};

#endif