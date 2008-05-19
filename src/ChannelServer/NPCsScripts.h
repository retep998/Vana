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
#ifndef NPCSSCRIPT_H
#define NPCSSCRIPT_H

#include "Shops.h"
#include "LuaNPC.h"
#include <sys/stat.h>

class NPC;

class QuestsScripts {
public:
	static void handle(int npcid, NPC* npc, bool start){
		if(start){
			switch(npcid){
				case 2000: npc_2000s(npc); break;
				default: npc->end(); break;
			}
		}
		else{
			switch(npcid){
				case 2000: npc_2000e(npc); break;
				default: npc->end(); break;
			}
		}
	}
private:
	static void npc_2000s(NPC* npc);
	static void npc_2000e(NPC* npc);
};

class NPCsScripts {
public:
	static void handle(int npcid, NPC* npc){
		struct stat fileinfo;
		char filename[255];
		sprintf_s(filename, "scripts/npcs/%d.lua", npcid);
		if(npc->isQuest()){
			QuestsScripts::handle(npcid, npc, npc->isStart());
		}
		else if (Shops::shops.find(npcid) != Shops::shops.end()) { // Shop
			npc->showShop();
			npc->end();
		}
		else if (!stat(filename, &fileinfo)) { // Lua NPC
			LuaNPC npc(filename, npc);
		}
	}
};

#endif
