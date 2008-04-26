#include "Initializing.h"
#include "tinyxml.h"
#include "Mobs.h"
#include "Drops.h"
#include "Maps.h"
#include "NPCs.h"
#include "Shops.h"
#include "Quests.h"
#include "Skills.h"
#include "windows.h"
#include <string>
#include "tchar.h"
using namespace std;

int strval(char* str){
	if(strlen(str)==0)
		return 0;
	int sign = 0;
	if(str[0] == '-')
		sign = 1;
	int ret=0;
	for(unsigned int i=sign; i<strlen(str); i++){
		ret*=10;
		if(sign)
			ret-=(str[i]-'0');
		else
			ret+=(str[i]-'0');
	}
	return ret;
}

void wtoc(const WCHAR* Source, char* to)
{
int i = 0;

while(Source[i] != '\0')
{
to[i] = (CHAR)Source[i];
++i;
}
to[i] = '\0';
}

void Initializing::initializeMob(char *name){
	char id[18];
	sprintf_s(id, 18, "Mobs/%s", name);
	TiXmlDocument doc(id);
	if(!doc.LoadFile()) return;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	pElem=hDoc.FirstChildElement().FirstChild().Element();
	if(!pElem) return;
	MobInfo mob;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("HP", pElem->Value()) == 0){
			mob.hp = strval((char*)pElem->GetText());
		}
		else if(strcmp("MP", pElem->Value()) == 0){
			mob.mp = strval((char*)pElem->GetText());
		}
		else if(strcmp("exp", pElem->Value()) == 0){
			mob.exp = strval((char*)pElem->GetText());
		}
		else if(strcmp("Summon", pElem->Value()) == 0){
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("ID", pCur->Value()) == 0){
					mob.summon.push_back(strval((char*)pCur->GetText()));
				}
			}
		}
	}
	int di=strval((char*)string(name).substr(0, string(name).find('.')).c_str());
	Mobs::addMob(di, mob);
}

void Initializing::initializeMobs(){
	WIN32_FIND_DATAA FindFileData;
	
	HANDLE hFind = FindFirstFileA("Mobs\\*.xml", &FindFileData);
	initializeMob((char*)FindFileData.cFileName);
	while (FindNextFileA(hFind, &FindFileData)) 
	{
		initializeMob((char*)FindFileData.cFileName);
	}
}

void Initializing::initializeItem(char *name){
	char id[25];
	sprintf_s(id, 25, "Items/%s", name);
	TiXmlDocument doc(id);
	if(!doc.LoadFile()) return;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	pElem=hDoc.FirstChildElement().FirstChild().Element();
	if(!pElem) return;
	ItemInfo item;
	item.consume = 0;
	int di=strval((char*)string(name).substr(0, string(name).find('.')).c_str());
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Type", pElem->Value()) == 0){
			item.type = strval((char*)pElem->GetText());
		}
		else if(strcmp("Price", pElem->Value()) == 0){
			item.price = strval((char*)pElem->GetText());
		}
		else if(strcmp("MaxSlot", pElem->Value()) == 0){
			item.maxslot = strval((char*)pElem->GetText());
		}
		else if(strcmp("Quest", pElem->Value()) == 0){
			if(strval((char*)pElem->GetText()) == 1)
				item.quest = 1;
			else
				item.quest = 0;
		}
		else if(strcmp("Consume", pElem->Value()) == 0){
			if(strval((char*)pElem->GetText()) == 1)
				item.consume = 1;
			else
				item.consume = 0;
		}
		else if(strcmp("Effect", pElem->Value()) == 0){	
			ConsumeInfo cons;
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("HP", pCur->Value()) == 0){
					cons.hp = strval((char*)pCur->GetText());
				}
				else if(strcmp("MP", pCur->Value()) == 0){
					cons.mp = strval((char*)pCur->GetText());
				}
				else if(strcmp("HPR", pCur->Value()) == 0){
					cons.hpr = strval((char*)pCur->GetText());
				}
				else if(strcmp("MPR", pCur->Value()) == 0){
					cons.mpr = strval((char*)pCur->GetText());
				}
				else if(strcmp("moveTo", pCur->Value()) == 0){
					cons.moveTo = strval((char*)pCur->GetText());
				}
				else if(strcmp("success", pCur->Value()) == 0){
					cons.success = strval((char*)pCur->GetText());
				}
				else if(strcmp("cursed", pCur->Value()) == 0){
					cons.cursed = strval((char*)pCur->GetText());
				}
				else if(strcmp("iSTR", pCur->Value()) == 0){
					cons.istr = strval((char*)pCur->GetText());
				}
				else if(strcmp("iDEX", pCur->Value()) == 0){
					cons.idex = strval((char*)pCur->GetText());
				}
				else if(strcmp("iINT", pCur->Value()) == 0){
					cons.iint = strval((char*)pCur->GetText());
				}
				else if(strcmp("iLUK", pCur->Value()) == 0){
					cons.iluk = strval((char*)pCur->GetText());
				}
				else if(strcmp("iHP", pCur->Value()) == 0){
					cons.ihp = strval((char*)pCur->GetText());
				}
				else if(strcmp("iMP", pCur->Value()) == 0){
					cons.imp = strval((char*)pCur->GetText());
				}
				else if(strcmp("iWAtk", pCur->Value()) == 0){
					cons.iwatk = strval((char*)pCur->GetText());
				}
				else if(strcmp("iMAtk", pCur->Value()) == 0){
					cons.imatk = strval((char*)pCur->GetText());
				}
				else if(strcmp("iWDef", pCur->Value()) == 0){
					cons.iwdef = strval((char*)pCur->GetText());
				}
				else if(strcmp("iMDef", pCur->Value()) == 0){
					cons.imdef = strval((char*)pCur->GetText());
				}
				else if(strcmp("iAcc", pCur->Value()) == 0){
					cons.iacc = strval((char*)pCur->GetText());
				}
				else if(strcmp("iAvo", pCur->Value()) == 0){
					cons.iavo = strval((char*)pCur->GetText());
				}
				else if(strcmp("iHand", pCur->Value()) == 0){
					cons.ihand = strval((char*)pCur->GetText());
				}
				else if(strcmp("iJump", pCur->Value()) == 0){
					cons.ijump = strval((char*)pCur->GetText());
				}
				else if(strcmp("iSpeed", pCur->Value()) == 0){
					cons.ispeed = strval((char*)pCur->GetText());
				}
				else if(strcmp("Mobs", pCur->Value()) == 0){
					SummonBag s;
					TiXmlElement* pCur2;
  					pCur2 = pCur->FirstChildElement();
					for( pCur2; pCur2; pCur2=pCur2->NextSiblingElement()){
						if(strcmp("Mob", pCur2->Value()) == 0){ 
							TiXmlElement* pCur3;
  							pCur3 = pCur2->FirstChildElement();
							for( pCur3; pCur3; pCur3=pCur3->NextSiblingElement()){
								if(strcmp("ID", pCur3->Value()) == 0){
									s.mobid = strval((char*)pCur3->GetText());
								}
								else if(strcmp("Chance", pCur3->Value()) == 0){
									s.chance = strval((char*)pCur3->GetText());
								}
							}
						}
						cons.mobs.push_back(s);
					}
				}	
			}
			Drops::addConsume(di, cons);
		}
	}	
	Drops::addItem(di, item);
}

void Initializing::initializeItems(){
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA("Items\\*.xml", &FindFileData);
	initializeItem((char*)FindFileData.cFileName);
	while (FindNextFileA(hFind, &FindFileData)) 
	{
		initializeItem((char*)FindFileData.cFileName);
	}
}
void Initializing::initializeNPCs(){
}
void Initializing::initializeDrop(char* name){
	char id[18];
	sprintf_s(id, 18, "Drops/%s", name);
	TiXmlDocument doc(id);
	if(!doc.LoadFile()) return;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	pElem=hDoc.FirstChildElement().Element();
	if(!pElem) return;
	if(strcmp("Mob",pElem->Value()) != 0) return;
	hRoot=TiXmlHandle(pElem);
	pElem=hRoot.FirstChildElement( "Drops" ).FirstChild().Element();
	MobDropsInfo drops;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Drop", pElem->Value()) == 0){
			MobDropInfo drop;
			drop.id = 0;
			drop.chance = 0;
			drop.quest = 0;
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("ID", pCur->Value()) == 0){
					drop.id = strval((char*)pCur->GetText());
				}
				else if(strcmp("Chance", pCur->Value()) == 0){
					drop.chance = strval((char*)pCur->GetText());
				}
				else if(strcmp("Quest", pCur->Value()) == 0){
					drop.quest = strval((char*)pCur->GetText());
				}

			}
			drops.push_back(drop);
		}
	}
	pElem=hRoot.FirstChild( "Mesos" ).FirstChild().Element();
	int min=0, max=0;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Min", pElem->Value()) == 0){
			min = strval((char*)pElem->GetText());
		}
		else if(strcmp("Max", pElem->Value()) == 0){
			max = strval((char*)pElem->GetText());
		}
	}
	int di=strval((char*)string(name).substr(0, string(name).find('.')).c_str());
	Drops::addDrop(di, drops);
	Mesos mesos;
	mesos.min = min;
	mesos.max = max;
	Drops::addMesos(di, mesos);
}
void Initializing::initializeDrops(){
  WIN32_FIND_DATAA FindFileData;
   
   HANDLE hFind = FindFirstFileA("Drops\\*.xml", &FindFileData);
	initializeDrop((char*)FindFileData.cFileName);
      while (FindNextFileA(hFind, &FindFileData)) 
      {
    	initializeDrop((char*)FindFileData.cFileName);
      }
}
void Initializing::initializeEquip(char *name){
	char id[25];
	sprintf_s(id, 25, "Equips/%s", name);
	TiXmlDocument doc(id);
	if(!doc.LoadFile()) return;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	pElem=hDoc.FirstChildElement().FirstChild().Element();
	if(!pElem) return;
	EquipInfo equip;
	equip.slots = 0;
	equip.type = 0;
	equip.price = 0;
	equip.istr = 0;
	equip.idex = 0;
	equip.iint = 0;
	equip.iluk = 0;
	equip.ihp = 0;
	equip.imp = 0;
	equip.iwatk = 0;
	equip.imatk = 0;
	equip.iwdef = 0;
	equip.imdef = 0;
	equip.iacc = 0;
	equip.iavo = 0;
	equip.ihand = 0;
	equip.ijump = 0;
	equip.ispeed = 0;
	equip.cash = 0;
	equip.quest = 0;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Type", pElem->Value()) == 0){
			equip.type = strval((char*)pElem->GetText());
		}
		else if(strcmp("Price", pElem->Value()) == 0){
			equip.price = strval((char*)pElem->GetText());
		}
		else if(strcmp("Slots", pElem->Value()) == 0){
			equip.slots = strval((char*)pElem->GetText());
		}
		else if(strcmp("STR", pElem->Value()) == 0){
			equip.istr = strval((char*)pElem->GetText());
		}
		else if(strcmp("DEX", pElem->Value()) == 0){
			equip.idex = strval((char*)pElem->GetText());
		}
		else if(strcmp("INT", pElem->Value()) == 0){
			equip.iint = strval((char*)pElem->GetText());
		}
		else if(strcmp("LUK", pElem->Value()) == 0){
			equip.iluk = strval((char*)pElem->GetText());
		}
		else if(strcmp("HP", pElem->Value()) == 0){
			equip.ihp = strval((char*)pElem->GetText());
		}
		else if(strcmp("MP", pElem->Value()) == 0){
			equip.imp = strval((char*)pElem->GetText());
		}
		else if(strcmp("WAtk", pElem->Value()) == 0){
			equip.iwatk = strval((char*)pElem->GetText());
		}
		else if(strcmp("MAtk", pElem->Value()) == 0){
			equip.imatk = strval((char*)pElem->GetText());
		}
		else if(strcmp("WDef", pElem->Value()) == 0){
			equip.iwdef = strval((char*)pElem->GetText());
		}
		else if(strcmp("MDef", pElem->Value()) == 0){
			equip.imdef = strval((char*)pElem->GetText());
		}
		else if(strcmp("Acc", pElem->Value()) == 0){
			equip.iacc = strval((char*)pElem->GetText());
		}
		else if(strcmp("Avo", pElem->Value()) == 0){
			equip.iavo = strval((char*)pElem->GetText());
		}
		else if(strcmp("Hand", pElem->Value()) == 0){
			equip.ihand = strval((char*)pElem->GetText());
		}
		else if(strcmp("Jump", pElem->Value()) == 0){
			equip.ijump = strval((char*)pElem->GetText());
		}
		else if(strcmp("Speed", pElem->Value()) == 0){
			equip.ispeed = strval((char*)pElem->GetText());
		}
		else if(strcmp("Cash", pElem->Value()) == 0){
			if(strval((char*)pElem->GetText()) == 1)
				equip.cash = 1;
			else
				equip.cash = 0;
		}
		else if(strcmp("Quest", pElem->Value()) == 0){
			if(strval((char*)pElem->GetText()) == 1)
				equip.quest = 1;
			else
				equip.quest = 0;
		}
	}
	int di=strval((char*)string(name).substr(0, string(name).find('.')).c_str());
	Drops::addEquip(di, equip);
}

void Initializing::initializeEquips(){
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA("Equips\\*.xml", &FindFileData);
	initializeEquip((char*)FindFileData.cFileName);
	while (FindNextFileA(hFind, &FindFileData))
	{
		initializeEquip((char*)FindFileData.cFileName);
	}
}

void Initializing::initializeShop(char* name){
	char id[25];
	sprintf_s(id, 25, "Shops/%s", name);
	TiXmlDocument doc(id);
	if(!doc.LoadFile()) return;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	pElem=hDoc.FirstChildElement().Element();
	if(!pElem) return;
	if(strcmp("Shop",pElem->Value()) != 0) return;
	hRoot=TiXmlHandle(pElem);
	ShopInfo shop;
	pElem=hRoot.FirstChildElement( "NPC" ).Element();
	shop.npc = strval((char*)pElem->GetText());
	pElem=hRoot.FirstChildElement( "Items" ).FirstChild().Element();
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Item", pElem->Value()) == 0){
			ShopItemInfo item;
			item.id = 0;
			item.price = 1;
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("ID", pCur->Value()) == 0){
					item.id = strval((char*)pCur->GetText());
				}
				else if(strcmp("Price", pCur->Value()) == 0){
					item.price = strval((char*)pCur->GetText());
				}
			}
			shop.items.push_back(item);
		}
	}
	int di=strval((char*)string(name).substr(0, string(name).find('.')).c_str());
	Shops::addShop(di, shop);
}
void Initializing::initializeShops(){
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA("Shops\\*.xml", &FindFileData);
	initializeShop((char*)FindFileData.cFileName);
	while (FindNextFileA(hFind, &FindFileData))
	{
		initializeShop((char*)FindFileData.cFileName);
	}
}
void Initializing::initializeQuest(char* name){
	char id[18];
	sprintf_s(id, 18, "Quests/%s", name);
	TiXmlDocument doc(id);
	if(!doc.LoadFile()) return;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	pElem=hDoc.FirstChildElement().Element();
	if(!pElem) return;
	if(strcmp("Quest",pElem->Value()) != 0) return;
	hRoot=TiXmlHandle(pElem);
	pElem=hRoot.FirstChildElement( "NextQuest" ).Element();
	int di=strval((char*)string(name).substr(0, string(name).find('.')).c_str());
	Quests::setNextQuest(di, strval((char*)pElem->GetText()));
	pElem=hRoot.FirstChildElement( "Requests" ).FirstChild().Element();
	QuestRequestsInfo reqs;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Request", pElem->Value()) == 0){
			QuestRequestInfo req;
			req.isitem = 0;
			req.ismob = 0;
			req.isquest = 0;
			req.id = 0;
			req.count = 0;
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("Mob", pCur->Value()) == 0){
					if(strval((char*)pCur->GetText()) == 1)
						req.ismob = 1;
					else 
						req.ismob = 0;
				}
				else if(strcmp("Item", pCur->Value()) == 0){
					if(strval((char*)pCur->GetText()) == 1)
						req.isitem = 1;
					else 
						req.isitem = 0;
				}
				else if(strcmp("Quest", pCur->Value()) == 0){
					if(strval((char*)pCur->GetText()) == 1)
						req.isquest = 1;
					else 
						req.isquest = 0;
				}
				else if(strcmp("ID", pCur->Value()) == 0){
					req.id = strval((char*)pCur->GetText());
				}
				else if(strcmp("Count", pCur->Value()) == 0){
					req.count = strval((char*)pCur->GetText());
				}
			}
			reqs.push_back(req);
		}
	}
	pElem=hRoot.FirstChildElement( "Rewards" ).FirstChild().Element();
	QuestRewardsInfo raws;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Reward", pElem->Value()) == 0){
			QuestRewardInfo raw;
			raw.start=0;
			raw.ismesos=0;
			raw.isitem=0;
			raw.isexp=0;
			raw.isfame=0;
			raw.id=0;
			raw.count=0;
			raw.gender=0;
			raw.job=0;
			raw.prop=0;
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("Start", pCur->Value()) == 0){
					if(strval((char*)pCur->GetText()) == 1)
						raw.start = 1;
					else 
						raw.start = 0;
				}
				else if(strcmp("Item", pCur->Value()) == 0){
					if(strval((char*)pCur->GetText()) == 1)
						raw.isitem = 1;
					else 
						raw.isitem = 0;
				}
				else if(strcmp("EXP", pCur->Value()) == 0){
					if(strval((char*)pCur->GetText()) == 1)
						raw.isexp = 1;
					else 
						raw.isexp = 0;
				}
				else if(strcmp("Mesos", pCur->Value()) == 0){
					if(strval((char*)pCur->GetText()) == 1)
						raw.ismesos = 1;
					else 
						raw.ismesos = 0;
				}
				else if(strcmp("Fame", pCur->Value()) == 0){
					if(strval((char*)pCur->GetText()) == 1)
						raw.isfame = 1;
					else 
						raw.isfame = 0;
				}
				else if(strcmp("ID", pCur->Value()) == 0){
					raw.id = strval((char*)pCur->GetText());
				}
				else if(strcmp("Count", pCur->Value()) == 0){
					raw.count = strval((char*)pCur->GetText());
				}
				else if(strcmp("Gender", pCur->Value()) == 0){
					raw.gender = strval((char*)pCur->GetText());
				}
				else if(strcmp("Job", pCur->Value()) == 0){
					raw.job = strval((char*)pCur->GetText());
				}
				else if(strcmp("Prop", pCur->Value()) == 0){
					raw.prop = strval((char*)pCur->GetText());
				}
			}
			raws.push_back(raw);
		}
	}
	Quests::addRequest(di, reqs);
	Quests::addReward(di, raws);
}
void Initializing::initializeQuests(){
  WIN32_FIND_DATAA FindFileData;
   
   HANDLE hFind = FindFirstFileA("Quests\\*.xml", &FindFileData);
   
	initializeQuest((char*)FindFileData.cFileName);
      while (FindNextFileA(hFind, &FindFileData))
      {
    	initializeQuest((char*)FindFileData.cFileName);
      }
}
void Initializing::initializeSkill(char* name){
	char id[50];
	sprintf_s(id, 50, "Skills/%s", name);
	TiXmlDocument doc(id);
	if(!doc.LoadFile()) return;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	pElem=hDoc.FirstChildElement().Element();
	if(!pElem) return;
	if(strcmp("Skill",pElem->Value()) != 0) return;
	hRoot=TiXmlHandle(pElem);
	pElem=hRoot.FirstChildElement( "Levels" ).FirstChild().Element();
	SkillsLevelInfo skill;
	int count=1;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Level", pElem->Value()) == 0){
			SkillLevelInfo level;
			level.item =0;
			level.itemcount=0;
			level.mp=0;
			level.hp=0;
			level.speed=0;
			level.jump=0;
			level.time=0;
			level.x=0;
			level.y=0;
			level.watk=0;
			level.wdef=0; 
			level.matk=0;
			level.mdef=0;
			level.acc=0;
			level.avo=0;
			level.hpP=0;
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("Time", pCur->Value()) == 0){
					level.time = strval((char*)pCur->GetText());
				}
				else if(strcmp("MP", pCur->Value()) == 0){
					level.mp = strval((char*)pCur->GetText());
				}
				else if(strcmp("HP", pCur->Value()) == 0){
					level.hp = strval((char*)pCur->GetText());
				}
				else if(strcmp("Item", pCur->Value()) == 0){
					level.item = strval((char*)pCur->GetText());
				}
				else if(strcmp("ItemCount", pCur->Value()) == 0){
					level.itemcount = strval((char*)pCur->GetText());
				}
				else if(strcmp("X", pCur->Value()) == 0){
					level.x = strval((char*)pCur->GetText());
				}
				else if(strcmp("Y", pCur->Value()) == 0){
					level.y = strval((char*)pCur->GetText());
				}
				else if(strcmp("Speed", pCur->Value()) == 0){
					level.speed = strval((char*)pCur->GetText());
				}
				else if(strcmp("Jump", pCur->Value()) == 0){
					level.jump = strval((char*)pCur->GetText());
				}
				else if(strcmp("WAtk", pCur->Value()) == 0){
					level.watk = strval((char*)pCur->GetText());
				}
				else if(strcmp("WDef", pCur->Value()) == 0){
					level.wdef = strval((char*)pCur->GetText());
				}
				else if(strcmp("MAtk", pCur->Value()) == 0){
					level.matk = strval((char*)pCur->GetText());
				}
				else if(strcmp("MDef", pCur->Value()) == 0){
					level.mdef = strval((char*)pCur->GetText());
				}
				else if(strcmp("Acc", pCur->Value()) == 0){
					level.acc = strval((char*)pCur->GetText());
				}
				else if(strcmp("Avo", pCur->Value()) == 0){
					level.avo = strval((char*)pCur->GetText());
				}
				else if(strcmp("HPP", pCur->Value()) == 0){
					level.hpP = strval((char*)pCur->GetText());
				}

			}
			skill[count++] = level;
		}
	}
	int di=strval((char*)string(name).substr(0, string(name).find('.')).c_str());
	Skills::addSkill(di, skill);
}
void Initializing::initializeSkills(){
  WIN32_FIND_DATAA FindFileData;
   
   HANDLE hFind = FindFirstFileA("Skills\\*.xml", &FindFileData);
   
	initializeQuest((char*)FindFileData.cFileName);
      while (FindNextFileA(hFind, &FindFileData))
      {
    	initializeSkill((char*)FindFileData.cFileName);
      }
}
void Initializing::initializeMap(char *name){
	int di=strval((char*)string(name).substr(0, string(name).find('.')).c_str());
	char id[20];
	sprintf_s(id, 20, "Maps/%s", name);
	TiXmlDocument doc(id);
	if(!doc.LoadFile()) return;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	pElem=hDoc.FirstChildElement().Element();
	if(!pElem) return;
	if(strcmp("Map",pElem->Value()) != 0) return;
	hRoot=TiXmlHandle(pElem);
	MapInfo map;
	pElem=hRoot.FirstChildElement( "returnMap" ).Element();
	map.rm = strval((char*)pElem->GetText());
	pElem=hRoot.FirstChildElement( "NPCs" ).FirstChild().Element();
	NPCsInfo npcs;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("NPC", pElem->Value()) == 0){
			NPCInfo npc;
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("id", pCur->Value()) == 0){
					npc.id = strval((char*)pCur->GetText());
				}
				else if(strcmp("x", pCur->Value()) == 0){
					npc.x = strval((char*)pCur->GetText());
				}
				else if(strcmp("cy", pCur->Value()) == 0){
					npc.cy = strval((char*)pCur->GetText());
				}
				else if(strcmp("fh", pCur->Value()) == 0){
					npc.fh = strval((char*)pCur->GetText());
				}
				else if(strcmp("rx0", pCur->Value()) == 0){
					npc.rx0 = strval((char*)pCur->GetText());
				}
				else if(strcmp("rx1", pCur->Value()) == 0){
					npc.rx1 = strval((char*)pCur->GetText());
				}
			}
			npcs.push_back(npc);
		}
	}
	NPCs::addNPC(di, npcs);

	pElem=hRoot.FirstChildElement( "Mobs" ).FirstChild().Element();
	SpawnsInfo spawns;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Mob", pElem->Value()) == 0){
			SpawnInfo spawn;
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("id", pCur->Value()) == 0){
					spawn.id = strval((char*)pCur->GetText());
				}
				else if(strcmp("x", pCur->Value()) == 0){
					spawn.x = strval((char*)pCur->GetText());
				}
				else if(strcmp("cy", pCur->Value()) == 0){
					spawn.cy = strval((char*)pCur->GetText());
				}
				else if(strcmp("fh", pCur->Value()) == 0){
					spawn.fh = strval((char*)pCur->GetText());
				}
			}
			spawns.push_back(spawn);
		}
	}
	Mobs::addSpawn(di, spawns);
	pElem=hRoot.FirstChildElement( "Portals" ).FirstChild().Element();
	PortalsInfo portals;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Portal", pElem->Value()) == 0){
			PortalInfo portal;
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("id", pCur->Value()) == 0){
					portal.id = strval((char*)pCur->GetText());
				}
				else if(strcmp("from", pCur->Value()) == 0){
					sprintf_s(portal.from, 20,"%s",(char*)pCur->GetText());
				}
				else if(strcmp("to", pCur->Value()) == 0){
					sprintf_s(portal.to, 20,"%s",(char*)pCur->GetText());
				}
				else if(strcmp("toid", pCur->Value()) == 0){
					portal.toid = strval((char*)pCur->GetText());
				}
				else if(strcmp("type", pCur->Value()) == 0){
					portal.type = strval((char*)pCur->GetText());
				}
				else if(strcmp("x", pCur->Value()) == 0){
					portal.x = strval((char*)pCur->GetText());
				}
				else if(strcmp("y", pCur->Value()) == 0){
					portal.y = strval((char*)pCur->GetText());
				}
			}
			portals.push_back(portal);
		}
	}
	map.Portals = portals;
	Maps::addMap(di, map);
	pElem=hRoot.FirstChildElement( "Footholds" ).FirstChild().Element();
	FootholdsInfo foots;
	for( pElem; pElem; pElem=pElem->NextSiblingElement())
	{
		if(strcmp("Foothold", pElem->Value()) == 0){
			FootholdInfo foot;
			TiXmlElement* pCur;
  			pCur = pElem->FirstChildElement();
			for( pCur; pCur; pCur=pCur->NextSiblingElement())
			{
				if(strcmp("x1", pCur->Value()) == 0){
					foot.x1 = strval((char*)pCur->GetText());
				}
				else if(strcmp("y1", pCur->Value()) == 0){
					foot.y1 = strval((char*)pCur->GetText());
				}
				else if(strcmp("x2", pCur->Value()) == 0){
					foot.x2 = strval((char*)pCur->GetText());
				}
				else if(strcmp("y2", pCur->Value()) == 0){
					foot.y2 = strval((char*)pCur->GetText());
				}
			}
			foots.push_back(foot);
		}
	}
	Drops::addFoothold(di, foots);
	Drops::objids[di] = 100;
}
void Initializing::initializeMaps(){
	  WIN32_FIND_DATAA FindFileData;
   
   HANDLE hFind = FindFirstFileA(("Maps\\*.xml"), &FindFileData);
   
	initializeMap((char*)FindFileData.cFileName);
      while (FindNextFileA(hFind, &FindFileData))
      {
    	initializeMap((char*)FindFileData.cFileName);
      }
}
void Initializing::initializing(){
	printf("Initializing Mobs... ");
	initializeMobs();
	printf("DONE\n");
	printf("Initializing Items... ");
	initializeItems();
	printf("DONE\n");
	printf("Initializing NPCs... ");
	initializeNPCs();
	printf("DONE\n");
	printf("Initializing Drops... ");
	initializeDrops();
	printf("DONE\n");
	printf("Initializing Equips... ");
	initializeEquips();
	printf("DONE\n");
	printf("Initializing Shops... ");
	initializeShops();
	printf("DONE\n");
	printf("Initializing Quests... ");
	initializeQuests();
	printf("DONE\n");
	printf("Initializing Skills... ");
	initializeSkills();
	printf("DONE\n");
	printf("Initializing Maps... ");
	initializeMaps();
	printf("DONE\n");
}