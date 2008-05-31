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
#ifndef PLAYER_H
#define PLAYER_H

#include "AbstractPlayer.h"
#include "PacketHandler.h"
#include "PlayerInventory.h"
#include "Skills.h"
#include "Quests.h"
#include "ChannelServer.h"
#include <cmath>
#include <vector>
#include <string>
#include <memory>

using namespace std;

struct Pos {
	short x;
	short y;
};

class NPC;
class ReadPacket;

int distPos(Pos pos1, Pos pos2);

struct SkillMapEnterActiveInfo;

class Player : public AbstractPlayer {
public:
	Player() : isconnect(false), save_on_dc(true), shop(0), npc(0), chair(0), itemEffect(0) {
		channel = ChannelServer::Instance()->getChannel();
		skill.types[0] = 0;
		skill.types[1] = 0;
		skill.types[2] = 0;
		skill.types[3] = 0;
		skill.types[4] = 0;
		skill.types[5] = 0;
		skill.types[6] = 0;
		skill.types[7] = 0;
		skill.val = 0;
		skill.isval = false;
	}

	~Player();

	void realHandleRequest(ReadPacket *packet);
	void setPlayerid(int id){
		this->id = id;
	}
	int getPlayerid(){
		return this->id;
	}
	char* getName(){
		return this->name;
	}
	int getChannel() {
		return this->channel;
	}
	char getGender(){
		return this->gender;
	}
	void setSkin(char id);
	char getSkin(){
		return this->skin;
	}
	void setEyes(int id);
	int getEyes(){
		return this->eyes;
	}
	void setHair(int id);
	int getHair(){
		return this->hair;
	}
	void setLevel(int level);
	unsigned char getLevel(){
		return this->level;
	}
	void setJob(short job);
	short getJob(){
		return this->job;
	}
	void setStr(short str){
		this->str=str;
	}
	short getStr(){
		return this->str;
	}
	void setDex(short dex){
		this->dex=dex;
	}
	short getDex(){
		return this->dex;
	}
	void setInt(short intt){
		this->intt=intt;
	}
	short getInt(){
		return this->intt;
	}
	void setLuk(short luk){
		this->luk=luk;
	}
	short getLuk(){
		return this->luk;
	}
	void setHP(int hp, bool is=1);
	unsigned short getHP(){
		return this->hp;
	}
	void setMP(int mp, bool is=0);
	unsigned short getMP(){
		return this->mp;
	}
	void setMHP(int mhp){
		if (mhp>30000) { mhp = 30000; }
		this->mhp=mhp;
	}
	unsigned short getMHP(){
		return this->mhp;
	}
	void setRMHP(int rmhp){
		this->rmhp=rmhp;
	}
	unsigned short getRMHP(){
		return this->rmhp;
	}
	void setMMP(int mmp){
		if (mmp>30000) { mmp = 30000; }
		this->mmp=mmp;
	}
	unsigned short getMMP(){
		return this->mmp;
	}
	void setRMMP(int rmmp){
		this->rmmp=rmmp;
	}
	unsigned short getRMMP(){
		return this->rmmp;
	}
	void setHPMPAp(unsigned short ap) {
		hpmp_ap = ap;
	}
	unsigned short getHPMPAp() {
		return hpmp_ap;
	}
	short getAp(){
		return this->ap;
	}
	short getSp(){
		return this->sp;
	}
	void setAp(short ap);
	void setSp(short sp);
	void setFame(short fame){
		this->fame=fame;
	}
	short getFame(){
		return this->fame;
	}
	void setExp(int exp, bool is);
	int getExp(){
		return this->exp;
	}
	void setMap(int map){
		this->map=map;
	}
	int getMap(){
		return this->map;
	}
	void setMappos(char pos){
		this->mappos = pos;
	}
	char getMappos(){
		return this->mappos;
	}
	Pos getPos(){
		return this->pos;
	}
	void setPos(Pos pos){
		this->pos = pos;
	}
	void setType(char type){
		this->type=type;
	}
	char getType(){
		return this->type;
	}
	void setShop(int shopid){
		shop=shopid;
	}
	int getShop(){
		return shop;
	}
	void setNPC(NPC* npc){
		this->npc = npc;
	}
	void setChair(int chair){
		this->chair = chair;
	}
	int getChair(){
		return chair;
	}
	void setItemEffect(int effect){
		this->itemEffect = effect;
	}
	int getItemEffect(){
		return this->itemEffect;
	}
	NPC* getNPC(){
		return npc;
	}
	bool isGM(){
		return gm > 0;
	}
	int getGMLevel() {
		return gm;
	}
	void setSkill(SkillMapEnterActiveInfo skill){
		this->skill = skill;
	}
	void setCombo(int combo){ // Combo Attack
		this->combo = combo;
	}
	int getCombo(){ // Combo Attack
		return combo;
	}
	void setSaveOnDC(bool save) {
		save_on_dc = save;
	}
	SkillMapEnterActiveInfo getSkill(){
		return skill;
	}
	void deleteVariable(const string &name) {
		variables.erase(name);
	}
	void setVariable(const string &name, const string &val) {
		variables[name] = val;
	}
	string getVariable(const string &name) {
		return (variables.find(name) == variables.end()) ? "0" : variables[name];
	}
	bool addWarning();
	void changeChannel(char channel);
	void saveSkills();
	void saveStats();
	void saveEquips();
	void saveItems();
	void saveVariables();
	void save();
	void setOnline(bool online);
	auto_ptr<PlayerInventory> inv;
	auto_ptr<PlayerSkills> skills;
	auto_ptr<PlayerQuests> quests;
private:
	void playerConnect(unsigned char *packet);
	void changeKey(unsigned char *packet);

	bool isconnect;
	int id;
	char name[15];
	int channel;
	char gender;
	char skin;
	int eyes;
	int hair;
	unsigned char level;
	short job;
	short str;
	short dex;
	short intt;
	short luk;
	unsigned short hp;
	unsigned short mhp;
	unsigned short rmhp;
	unsigned short mp;
	unsigned short mmp;
	unsigned short rmmp;
	unsigned short hpmp_ap;
	short ap;
	short sp;
	int exp;
	short fame;
	int map;
	char mappos;
	char type;
	int keys[90];
	int shop;
	int itemEffect;
	int chair;
	int gm;
	Pos pos;
	NPC* npc;
	vector <int> warnings;
	int combo; // Combo Attack
	bool save_on_dc;
	hash_map <string, string> variables;
	SkillMapEnterActiveInfo skill;
};

class PlayerFactory:public AbstractPlayerFactory {
public:
	AbstractPlayer* createPlayer() {
		return new Player();
	}
};

#endif
