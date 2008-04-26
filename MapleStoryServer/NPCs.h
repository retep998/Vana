#ifndef NPCS_H
#define NPCS_H

#include <hash_map>
#include <vector>

using namespace std;
using namespace stdext;

#define YES 1
#define NO 0
#define ACCEPT 1
#define DECLINE 0

class Player;
class Packet;

struct NPCInfo {
	int id;
	short x;
	short cy;
	short fh;
	short rx0;
	short rx1; 
};

typedef vector<NPCInfo> NPCsInfo;

class NPCs {
public:
	static hash_map <int, NPCsInfo> info;
	static void addNPC(int id, NPCsInfo npc){
		info[id] = npc;
	}
	static void handleNPC(Player* player, unsigned char* packet);
	static void handleQuestNPC(Player* player, int npcid, bool start);
	static void showNPCs(Player* player);
	static void handleNPCIn(Player* player, unsigned char* packet);
};

class NPC {
private:
	int npcid;
	Player* player;
	char text[1000];	
	int state;
	int selected;
	bool cend;
	int getnum;
	char gettext[101];
	bool isquest;
	bool isstart;
	hash_map <char*, int> vars;
public:
	NPC(int npcid, Player* player, bool isquest = 0);
	~NPC();
	void addText(char* text){
		strcat_s(this->text, strlen(text)+1+strlen(this->text), text);
	}
	void addChar(char cha){
		char temp[2]={0};
		temp[0] = cha;
		addText(temp);
	}

	Packet npcPacket(char type);
	void sendSimple();
	void sendYesNo();
	void sendNext();
	void sendBackNext();
	void sendBackOK();
	void sendOK();
	void sendAcceptDecline();
	void sendGetText();
	void sendGetNumber(int def, int min, int max);
	void sendStyle(int styles[], char size);
	bool isQuest(){
		return isquest;
	}
	bool isStart(){
		return isstart;
	}
	void setIsStart(bool what){
		isstart = what;
	}
	void setState(int state){
		this->state = state;
	}
	int getState(){
		return state;
	}
	int getSelected(){
		return selected;
	}
	void setSelected(int selected){
		this->selected=selected;
	}
	void setGetNumber(int num){
		this->getnum = num;
	}
	int getNumber(){
		return getnum;
	}
	void setGetText(char* text){
		strcpy_s(this->gettext, strlen(text)+1, text);
	}
	char* getText(){
		return gettext;
	}
	void end(){
		cend=true;
	}
	bool isEnd(){
		return cend;
	}
	int getNpcID(){
		return npcid;
	}
	Player* getPlayer(){
		return player;
	}
	void teleport(int mapid);
	int getItemNum(int itemid);
	int getMesos();
	void giveItem(int itemid, short amount);
	void giveMesos(int amount);
	int getPlayerMap();
	void showShop();
	void setStyle(int id);
	void setVariable(char* name, int val){
		vars[name] = val;
	}
	int getVariable(char* name){
		if(vars.find(name) == vars.end())
			return 0;
		else
			return vars[name];
	}
	int getPlayerHair();
	int getPlayerEyes();
	void setPlayerHP(short hp);
	short getPlayerHP();
	void addQuest(short questid);
	void endQuest(short questid);
	void giveEXP(int exp);
	int getLevel();

};

#endif