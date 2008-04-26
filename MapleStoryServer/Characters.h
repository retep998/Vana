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

class Characters {
private:
	static void showEquips(int id, vector <CharEquip> &vec);
	static void createEquip(int equipid, int type, int charid);
public:
	static void connectGame(PlayerLogin* player, unsigned char* packet);
	static void checkCharacterName(PlayerLogin* player, unsigned char* packet);
	static void createCharacter(PlayerLogin* player, unsigned char* packet);
	static void deleteCharacter(PlayerLogin* player, unsigned char* packet);
	static void showCharacters(PlayerLogin* player);
};

#endif