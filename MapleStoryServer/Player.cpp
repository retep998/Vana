#include "Player.h"
#include "Drops.h"
#include "Inventory.h"
#include "Maps.h"
#include "Mobs.h"
#include "NPCs.h"
#include "Players.h"
#include "MySQLM.h"
#include "PlayerPacket.h"
#include "Levels.h"
#include "Skills.h"
#include "Quests.h"
#include "Server.h"
#include "Fame.h"

int distPos(Pos pos1, Pos pos2){
	return (int)sqrt(pow((float)(pos1.x+pos2.x), 2)+pow((float)(pos1.y+pos2.y), 2));
}

Player::~Player(){
	if(isconnect){
		Maps::removePlayer(this);
		Players::deletePlayer(this);
		save();
		Skills::stopTimerPlayer(this);
		isconnect = false;
	}
}

void Player::handleRequest(unsigned char* buf, int len){
	short header = buf[0] + buf[1]*0x100;
	switch(header){  
		case 0x14: getUserID(buf+2); break;
		case 0x21: NPCs::handleNPCIn(this ,buf+2); break;
		case 0x22: Inventory::useShop(this ,buf+2); break;
		case 0x23: NPCs::handleNPC(this, buf+2); break;
		case 0x2A: Players::damagePlayer(this ,buf+2); break;
		case 0x2B: Inventory::stopChair(this ,buf+2); break;
		case 0x2C: Players::chatHandler(this ,buf+2); break;
		case 0x2D: Inventory::useChair(this ,buf+2); break;
		case 0x2E: Mobs::damageMobSkill(this ,buf+2); break;
		case 0x2F: Maps::moveMap(this ,buf+2); break;
		case 0x35: Players::handleMoving(this ,buf+2, len-2); break;
		case 0x36: Mobs::damageMobS(this ,buf+2, len-2); break;
		case 0x44: Players::getPlayerInfo(this, buf+2); break;
		case 0x4B: Inventory::useSummonBag(this, buf+2); break;
		case 0x4D: Skills::addSkill(this, buf+2); break;
		case 0x4E: Skills::cancelSkill(this, buf+2); break;
		case 0x51: Skills::useSkill(this, buf+2); break;
		case 0x53: Inventory::useMegaphone(this, buf+2); break;
		case 0x58: Players::commandHandler(this ,buf+2); break;
		case 0x59: Mobs::damageMob(this ,buf+2); break;
		case 0x5C: Players::faceExperiment(this ,buf+2); break;
		case 0x62: Inventory::itemMove(this ,buf+2); break;
		case 0x63: Inventory::useItem(this, buf+2); break;
		case 0x64: Inventory::useReturnScroll(this, buf+2); break; 
		case 0x65: Inventory::useScroll(this, buf+2); break;
		case 0x66: Levels::addStat(this, buf+2); break;
		case 0x67: Players::healPlayer(this, buf+2); break;
		case 0x68: Drops::dropMesos(this ,buf+2); break;
		case 0x6B: Quests::getQuest(this, buf+2); break;
		case 0x75: chaneKey(buf+2);
		case 0x89: Drops::lootItem(this ,buf+2); break;
		case 0x9D: Mobs::monsterControl(this ,buf+2, len-2); break;
		case 0xA0: Mobs::monsterControlSkill(this ,buf+2); break;
		case 0x69: Fame::handleFame(this, buf+2); break;
	}
}

void Player::playerConnect(){
	MySQL::getStringI("characters", "ID", getPlayerid(), "name", name);
	gender = (char)MySQL::getInt("characters", getPlayerid(), "gender");
	skin = (char)MySQL::getInt("characters", getPlayerid(), "skin");
	eyes = MySQL::getInt("characters", getPlayerid(), "eyes");
	hair = MySQL::getInt("characters", getPlayerid(), "hair");
	level = (char)MySQL::getInt("characters", getPlayerid(), "level");
	job = (short)MySQL::getInt("characters", getPlayerid(), "job");
	str = (short)MySQL::getInt("characters", getPlayerid(), "str");
	dex = (short)MySQL::getInt("characters", getPlayerid(), "dex");
	intt = (short)MySQL::getInt("characters", getPlayerid(), "intt");
	luk = (short)MySQL::getInt("characters", getPlayerid(), "luk");
	hp = (short)MySQL::getInt("characters", getPlayerid(), "chp");
	rmhp = mhp = (short)MySQL::getInt("characters", getPlayerid(), "mhp");
	mp = (short)MySQL::getInt("characters", getPlayerid(), "cmp");
	rmmp = mmp = (short)MySQL::getInt("characters", getPlayerid(), "mmp");
	ap = (short)MySQL::getInt("characters", getPlayerid(), "ap");
	sp = (short)MySQL::getInt("characters", getPlayerid(), "sp");
	exp = MySQL::getInt("characters", getPlayerid(), "exp");
	fame = (short)MySQL::getInt("characters", getPlayerid(), "fame");
	map = MySQL::getInt("characters", getPlayerid(), "map");
	mappos = (char)MySQL::getInt("characters", getPlayerid(), "pos");
	gm = MySQL::getInt("users", MySQL::getInt("characters", getPlayerid(), "userid"), "gm");
	int equips[130][21];
	int many = MySQL::showEquipsIn(getPlayerid(), equips);
	inv = new PlayerInventory();
	skills = new PlayerSkills();
	quests = new PlayerQuests();
	quests->setPlayer(this);
	for(int i=0; i<many; i++){
		Equip* equip = new Equip;
		equip->id = equips[i][0];
		equip->slots = equips[i][4];
		equip->scrolls = equips[i][5];
		equip->type = equips[i][1];
		equip->pos = equips[i][3];
		equip->istr = equips[i][6];
		equip->idex = equips[i][7];
		equip->iint = equips[i][8];
		equip->iluk = equips[i][9];
		equip->ihp = equips[i][10];
		equip->imp = equips[i][11];
		equip->iwatk = equips[i][12];
		equip->imatk = equips[i][13];
		equip->iwdef = equips[i][14];
		equip->imdef = equips[i][15];
		equip->iacc = equips[i][16];
		equip->iavo = equips[i][17];
		equip->ihand = equips[i][18];
		equip->ijump = equips[i][19];
		equip->ispeed = equips[i][20];
		inv->addEquip(equip);
	}
	int items[400][4];
	many = MySQL::getItems(getPlayerid(), items);
	for(int i=0; i<many; i++){
		Item* item = new Item;
		item->id = items[i][0];
		item->inv = items[i][1];
		item->pos = items[i][2];
		item->amount = items[i][3];
		inv->addItem(item);
	}
	int skill[200][2];
	many = MySQL::getSkills(getPlayerid(), skill);
	for(int i=0; i<many; i++){
		skills->addSkillLevel(skill[i][0], skill[i][1]);
	}
	inv->setMesosStart(MySQL::getInt("characters", getPlayerid(), "mesos"));
	inv->setPlayer(this);
	MySQL::getKeys(getPlayerid(), keys);
	PlayerPacket::connectData(this);
	Server::showScrollingHeader(this);
	if(Maps::info[map].Portals.size() > 0){
		pos.x = Maps::info[map].Portals[0].x;
		pos.y = Maps::info[map].Portals[0].y;
	}
	else{
		pos.x = 0;
		pos.y = 0;
	}
	type=0;
	PlayerPacket::showKeys(this, keys);
	Maps::newMap(this);

}

void Player::setHP(int hp, bool is){
	if(hp<0)
		this->hp=0;
	else if(hp>mhp)
		this->hp=mhp;
	else
		this->hp=hp;
	if(is)
		PlayerPacket::newHP(this, (short)this->hp);
}

void Player::setMP(int mp, bool is){
	if(mp<0)
		this->mp=0;
	else if(mp>mmp)
		this->mp=mmp;
	else
		this->mp=mp;
	PlayerPacket::newMP(this, (short)this->mp, is);
}

void Player::setSp(short sp){
	this->sp=sp;
	PlayerPacket::setSP(this);
}

void Player::setJob(short job){
	this->job=job;
	PlayerPacket::setJob(this);
}


void Player::setExp(int exp, bool is){
	this->exp=exp;
	if(is)
		PlayerPacket::newEXP(this, this->exp);
}

void Player::setLevel(int level){
	this->level=(unsigned char)level;

}
int getIntg(unsigned char* buf){
	return buf[0] + buf[1]*0x100 + buf[2]*0x100*0x100 + buf[3]*0x100*0x100*0x100;
}

void Player::chaneKey(unsigned char* packet){
	int howmany = getIntg(packet+4);
	for(int i=0; i<howmany; i++){
		int pos = getIntg(packet+8+i*9);
		int key = getIntg(packet+12+i*9);
		if(packet[12+i*9] == 0) // TODO 1st type byte, than key int
			key=0;
		if(pos>=0 && pos<90)
			keys[pos] = key;
	}
}

void Player::setHair(int id){
	this->hair = id;
	PlayerPacket::newHair(this);
}

void Player::setEyes(int id){
	this->eyes = id;
	PlayerPacket::newEyes(this);
}

void Player::setSkin(char id){
	this->skin = id;
	PlayerPacket::newSkin(this);
}

void Player::addWarning(){
	int t = GetTickCount();
	// Deleting old warnings
	for(unsigned int i=0; i<warnings.size(); i++){
		if(warnings[i] + 300000 < t){
			warnings.erase(warnings.begin()+i);
			i--;
		}
	}
	warnings.push_back(t);
	if(warnings.size()>50){
		// Hacker - Temp DCing
		int tmap = map;
		Maps::changeMap(this, 999999999, 0);
		Maps::changeMap(this, tmap, 0);
	}
}

void Player::save(){
	char sql[10000];
	sprintf_s(sql, 10000, "update keymap set ");
	for(int i=0; i<90; i++){
		char temp[100];
		if(i!=89)
			sprintf_s(temp, 100, "pos%d=%d, ", i, keys[i]);
		else
			sprintf_s(temp, 100, "pos%d=%d where charid=%d; ", i, keys[i], getPlayerid());
		strcat_s(sql, 10000, temp);
	}
	MySQL::insert(sql);
	sprintf_s(sql, 10000, "update characters set level=%d, job=%d, str=%d, dex=%d, intt=%d, luk=%d, chp=%d, mhp=%d, cmp=%d, mmp=%d, ap=%d, sp=%d, exp=%d, fame=%d, map=%d, gender=%d, skin=%d, eyes=%d, hair=%d, mesos=%d where id=%d", getLevel(), getJob(), getStr(), getDex(), getInt(), getLuk(), getHP(), getRMHP(), getMP(), getRMMP(), getAp(), getSp(), getExp(), getFame(), getMap(), getGender(), getSkin(), getEyes(), getHair(), inv->getMesos() ,getPlayerid());
	MySQL::insert(sql);
	char temp[1000];
	sprintf_s(sql, 10000, "delete from equip where charid=%d;", getPlayerid());
	MySQL::insert(sql);
	bool firstrun = true;
	for(int i=0; i<inv->getEquipNum(); i++){
		if(firstrun == true){
			sprintf_s(sql, 10000, "INSERT INTO equip VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)", inv->getEquip(i)->id, Drops::equips[inv->getEquip(i)->id].type ,getPlayerid(), inv->getEquipPos(i), inv->getEquip(i)->slots, inv->getEquip(i)->scrolls,
				inv->getEquip(i)->istr, inv->getEquip(i)->idex, inv->getEquip(i)->iint, inv->getEquip(i)->iluk, inv->getEquip(i)->ihp, inv->getEquip(i)->imp, inv->getEquip(i)->iwatk, inv->getEquip(i)->imatk, inv->getEquip(i)->iwdef, 
				inv->getEquip(i)->imdef, inv->getEquip(i)->iacc, inv->getEquip(i)->iavo, inv->getEquip(i)->ihand, inv->getEquip(i)->ijump, inv->getEquip(i)->ispeed);
			firstrun = false;
		}
		else{
			sprintf_s(temp, 1000, ",(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)", inv->getEquip(i)->id, Drops::equips[inv->getEquip(i)->id].type ,getPlayerid(), inv->getEquipPos(i), inv->getEquip(i)->slots, inv->getEquip(i)->scrolls,
				inv->getEquip(i)->istr, inv->getEquip(i)->idex, inv->getEquip(i)->iint, inv->getEquip(i)->iluk, inv->getEquip(i)->ihp, inv->getEquip(i)->imp, inv->getEquip(i)->iwatk, inv->getEquip(i)->imatk, inv->getEquip(i)->iwdef, 
				inv->getEquip(i)->imdef, inv->getEquip(i)->iacc, inv->getEquip(i)->iavo, inv->getEquip(i)->ihand, inv->getEquip(i)->ijump, inv->getEquip(i)->ispeed);
			strcat_s(sql, 10000, temp);
		}
	}
	MySQL::insert(sql);
	sprintf_s(sql, 10000, "delete from skills where charid=%d;", getPlayerid());
	MySQL::insert(sql);
	firstrun = true;
	for(int i=0; i<skills->getSkillsNum(); i++){
		if(firstrun == true){
			sprintf_s(sql, 10000, "INSERT INTO skills VALUES (%d, %d, %d)", getPlayerid(), skills->getSkillID(i), skills->getSkillLevel(skills->getSkillID(i)));
			firstrun = false;
		}
		else{
			sprintf_s(temp, 1000, ",(%d, %d, %d)", getPlayerid(), skills->getSkillID(i), skills->getSkillLevel(skills->getSkillID(i)));
			strcat_s(sql, 10000, temp);
		}
	}
	MySQL::insert(sql);
	sprintf_s(sql, 10000, "DELETE FROM items WHERE charid=%d;", getPlayerid());
	MySQL::insert(sql);
	firstrun = true;
	for(int i=0; i<inv->getItemNum(); i++){
		if(firstrun == true){
			sprintf_s(sql, 10000, "INSERT INTO items VALUES (%d, %d, %d, %d, %d)", inv->getItem(i)->id, getPlayerid() ,inv->getItem(i)->inv, inv->getItem(i)->pos, inv->getItem(i)->amount);
			firstrun = false;
		}
		else{
			sprintf_s(temp, 1000, ",(%d, %d, %d, %d, %d)", inv->getItem(i)->id, getPlayerid() ,inv->getItem(i)->inv, inv->getItem(i)->pos, inv->getItem(i)->amount);
			strcat_s(sql, 10000, temp);
		}
	}
	MySQL::insert(sql);
}