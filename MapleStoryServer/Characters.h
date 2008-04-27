#ifndef CHARACTERS_H
#define CHARACTERS_H

#include <vector>

using namespace std;

struct CharEquip {
	char type;
	int id;
};

struct Character {
	int id;
	char name[15];
	char gender;
	char skin;
	int eyes;
	int hair;
	char level;
	short job;
	short str;
	short dex;
	short intt;
	short luk;
	short hp;
	short mhp;
	short mp;
	short mmp;
	short ap;
	short sp;
	short exp;
	short fame;
	int map;
	char pos;
	vector <CharEquip> equips;
};

class PlayerLogin;

namespace Characters {
	void connectGame(PlayerLogin* player, unsigned char* packet);
	void checkCharacterName(PlayerLogin* player, unsigned char* packet);
	void createCharacter(PlayerLogin* player, unsigned char* packet);
	void deleteCharacter(PlayerLogin* player, unsigned char* packet);
	void showCharacters(PlayerLogin* player);
	extern private void showEquips(int id, vector <CharEquip> &vec);
	extern private void createEquip(int equipid, int type, int charid);
};

#endif