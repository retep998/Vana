#ifndef INITIALIZE_H
#define INITIALIZE_H

class Initializing {
public:
	static void initializing();
	static void initializeMobs();
	static void initializeNPCs();
	static void initializeDrops();
	static void initializeMaps();
	static void initializeEquips();
	static void initializeShops();
	static void initializeItems();
	static void initializeQuests();
	static void initializeSkills();
	static void initializeDrop(char* name);
	static void initializeMap(char* name);
	static void initializeNPC(char* name);
	static void initializeMob(char* name);
	static void initializeEquip(char* name);
	static void initializeShop(char* name);
	static void initializeItem(char* name);
	static void initializeQuest(char* name);
	static void initializeSkill(char* name);
};

#endif