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
#include "NPCs.h"
#include "NPCsScripts.h"

void NPCsScripts::npc_9900000(NPC* npc){
	int state = npc->getState();
	int skins[] = {0, 1, 2, 3, 4};
	int hairs[] = {30000, 30010, 30020, 30030, 30040, 30050, 30060, 30070, 30080, 30090, 30100, 30110, 30120, 30130, 30140, 30150, 30160, 30170, 30180, 30190, 30200, 30210, 30220, 30230, 30240, 30250, 30260, 30270, 30280, 30290, 30300, 30310, 30320, 30330, 30340, 30350, 30360, 30370, 30400, 30410, 30420, 30430, 30440, 30450, 30460, 30470, 30480, 30490, 30510, 30520, 30530, 30540, 30550, 30560, 30570, 30580, 30590, 30600, 30610, 30620, 30630, 30640, 30650, 30660, 30700, 30710, 30720};
	int hairscolor[] = {8, 1, 8, 8, 8, 8, 8, 1, 1, 1, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
	int eyes[] = {20000, 20001, 20002, 20003, 20004, 20005, 20006, 20007, 20008, 20009, 20010, 20011, 20012, 20013, 20014, 20016, 20017, 20018, 20019, 20020, 20021, 20022, 20023};
	if(state == 0){
		npc->addText("#L0#Skin#l\r\n#L1#Hair#l\r\n#L2#Hair Color#l\r\n#L3#Eyes#l\r\n#L4#Eyes Color#l");
		npc->sendSimple();
	}
	else if(state == 1){
		int type = npc->getSelected();
		npc->setVariable("type", type);
		if(type == 0){
			npc->sendStyle(skins, 5);
		}
		else if(type == 1){
			npc->sendStyle(hairs, 67);
		}
		else if(type == 2){
			int cur = npc->getPlayerHair()/10*10;
			int colors[] = {cur, cur+1, cur+2, cur+3, cur+4, cur+5, cur+6, cur+7};
			npc->sendStyle(colors, hairscolor[npc->getPlayerHair()%1000/10]);
		}
		else if(type == 3){
			npc->sendStyle(eyes, 23);
		}
		else if(type == 4){
			int cur = npc->getPlayerEyes()%100+20000;
			int colors[] = {cur, cur+100, cur+200, cur+300, cur+400, cur+500, cur+600, cur+700};
			npc->sendStyle(colors, 8);
		}
		else{
			npc->end();
		}
	}
	else if(state == 2){
		npc->end();
		int type = npc->getVariable("type");
		if(type == 0){
			npc->setStyle(skins[npc->getSelected()]);
		}
		else if(type == 1){
			npc->setStyle(hairs[npc->getSelected()]);
		}
		else if(type == 2){
			npc->setStyle(npc->getPlayerHair()/10*10 + npc->getSelected());
		}
		else if(type == 3){
			npc->setStyle(eyes[npc->getSelected()]);
		}
		else if(type == 4){
			npc->setStyle(20000+npc->getPlayerEyes()%100 + npc->getSelected()*100);
		}
		else
			npc->setStyle(eyes[npc->getSelected()]);
	}

}


