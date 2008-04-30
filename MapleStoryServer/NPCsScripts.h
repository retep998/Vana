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
		if(npc->isQuest()){
			QuestsScripts::handle(npcid, npc, npc->isStart());
			return;
		}
		switch(npcid){
			case 2100: npc_2100(npc); break;
			case 2101: npc_2101(npc); break;
			case 22000: npc_22000(npc); break;
			case 2020005: npc_2020005(npc); break;
			case 9101001: npc_9101001(npc); break;
			case 9900000: npc_9900000(npc); break;
			//Shops
			case 11000: npc->showShop(); npc->end(); break; // Sid - Amherst Weapon Store (1010001)
			case 11100: npc->showShop(); npc->end(); break; // Lucy - Amherst Department Store (1010003)
			case 21000: npc->showShop(); npc->end(); break; // Pan - Southperry Armor Store (60001)
			case 9201020: npc->showShop(); npc->end(); break; // Vivian Boutique - Amoria Wedding Shop (680000001)
			case 2070003: npc->showShop(); npc->end(); break; // Dori - Korean Folk Town (222000000)
			case 2070001: npc->showShop(); npc->end(); break; // Bung's Mama - Korean Folk Town (222000000)
			case 2070002: npc->showShop(); npc->end(); break; // Moki - Korean Folk Town (222000000)
			case 2041002: npc->showShop(); npc->end(); break; // Hid - Ludibrium Weapon Store (220000001)
			case 2041003: npc->showShop(); npc->end(); break; // Miru - Ludibrium Weapon Store (220000001)
			case 2041006: npc->showShop(); npc->end(); break; // Misky - Ludibrium Pharmacy (220000002)
			case 2090002: npc->showShop(); npc->end(); break; // Bidiwon - Mu Lung (250000000)
			case 2090001: npc->showShop(); npc->end(); break; // Gong Gong - Mu Lung (250000000)
			case 2090003: npc->showShop(); npc->end(); break; // Dalsuk - Mu Lung Department Store (250000002)
			case 9110003: npc->showShop(); npc->end(); break; // Janken - Mushroom Shrine (800000000)
			case 9110004: npc->showShop(); npc->end(); break; // Taru - Mushroom Shrine (800000000)
			case 9110005: npc->showShop(); npc->end(); break; // Bronze - Mushroom Shrine (800000000)
			case 9110006: npc->showShop(); npc->end(); break; // Jin Jia - Mushroom Shrine (800000000)
			case 9110007: npc->showShop(); npc->end(); break; // Robo - Mushroom Shrine (800000000)
			case 9201058: npc->showShop(); npc->end(); break; // Delphi - New Leaf City - Town Center (600000000)
			case 9201059: npc->showShop(); npc->end(); break; // Kyle - New Leaf City - Town Center (600000000)
			case 9201060: npc->showShop(); npc->end(); break; // Miki - New Leaf City - Town Center (600000000)
			case 1061001: npc->showShop(); npc->end(); break; // 24 Hr Mobile Store - Ant Tunnel Park (105070001)
			case 1031000: npc->showShop(); npc->end(); break; // Flora the Fairy - Ellinia Weapon Store (101000001)
			case 1031001: npc->showShop(); npc->end(); break; // Serabi the Fairy - Ellinia Weapon Store (101000001)
			case 1031100: npc->showShop(); npc->end(); break; // Len the Fairy - Ellinia Department Store (101000002)
			case 1081000: npc->showShop(); npc->end(); break; // Valen - Florina Beach (110000000)
			case 1011000: npc->showShop(); npc->end(); break; // Karl - Henesys Weapon Store (100000101)
			case 1011001: npc->showShop(); npc->end(); break; // Sam - Henesys Weapon Store (100000101)
			case 1011100: npc->showShop(); npc->end(); break; // Luna - Henesys Department Store (100000102)
			case 1051000: npc->showShop(); npc->end(); break; // Cutthroat Manny - Kerning City Self-Defence Item Store (103000001)
			case 1051001: npc->showShop(); npc->end(); break; // Don Hwang - Kerning City Self-Defence Item Store (103000001)
			case 1051002: npc->showShop(); npc->end(); break; // Dr. Faymus - Kerning City Pharmacy (103000002)
			case 1001000: npc->showShop(); npc->end(); break; // Silver - Lith Harbor Weapon Shop (104000003)
			case 1001001: npc->showShop(); npc->end(); break; // Natasha - Lith Harbor Armor Shop (104000001)
			case 1001100: npc->showShop(); npc->end(); break; // Mina - Lith Harbor Department Store (104000002)
			case 1021000: npc->showShop(); npc->end(); break; // River - Perion Weapon Store (102000001)
			case 1021001: npc->showShop(); npc->end(); break; // Harry - Perion Weapon Store (102000001)
			case 1021100: npc->showShop(); npc->end(); break; // Arturo - Perion Department Store (102000002)
			case 1061002: npc->showShop(); npc->end(); break; // Mr. Sweatbottom - Regular Sauna (105040401)
			case 2022001: npc->showShop(); npc->end(); break; // Hana - El Nath Department Store (211000102)
			case 2020001: npc->showShop(); npc->end(); break; // Scott - El Nath Weapon Store (211000101)
			case 2022000: npc->showShop(); npc->end(); break; // Rumi - El Nath Weapon Store (211000101)
			case 2060004: npc->showShop(); npc->end(); break; // Oannes - Department Store (230000002)
			case 2060003: npc->showShop(); npc->end(); break; // Melias - Department Store (230000002)
			case 2060007: npc->showShop(); npc->end(); break; // Calypso - Department Store (230000002)
			case 2050000: npc->showShop(); npc->end(); break; // Dr. San - Silo (221000200)
			case 2050003: npc->showShop(); npc->end(); break; // Spacen - Silo (221000200)
			case 2051000: npc->showShop(); npc->end(); break; // Dr. Pepper - Silo (221000200)
			case 2012003: npc->showShop(); npc->end(); break; // Neri the Fairy - Orbis Weapon Store (200000001)
			case 2012004: npc->showShop(); npc->end(); break; // Nuri the Fairy - Orbis Weapon Store (200000001)
			case 2012005: npc->showShop(); npc->end(); break; // Edel the Fairy - Orbis Department Store (200000002)
			case 2093000: npc->showShop(); npc->end(); break; // Mu Tan - Herb Town (251000000)
			case 2093002: npc->showShop(); npc->end(); break; // Lan Ming - Herb Town (251000000)
			case 2093001: npc->showShop(); npc->end(); break; // So Won - Herb Town (251000000)
			case 2080002: npc->showShop(); npc->end(); break; // Max - Leafre (240000000)
			case 2080001: npc->showShop(); npc->end(); break; // Sly - Department Store (240000002)
			case 1052104: npc->showShop(); npc->end(); break; // Tulcus - The Swamp of Despair II (107000100)
			case 1032103: npc->showShop(); npc->end(); break; // El Moth - The Tree That Grew III (101010102)
			case 2022002: npc->showShop(); npc->end(); break; // Barun - Orbis Tower <14th Floor> (200080800)
			case 2041016: npc->showShop(); npc->end(); break; // Vega - Eos Tower 44th Floor (221022000)
			case 2040049: npc->showShop(); npc->end(); break; // Gumball Machine - Eos Tower 26th ~ 40th Floor (221021600)
			case 2030009: npc->showShop(); npc->end(); break; // Glibber - Ice Valley II (211040200)
			default: npc->end(); break;
		}
	}
private:
	static void npc_2100(NPC* npc);
	static void npc_2101(NPC* npc);
	static void npc_22000(NPC* npc);
	static void npc_2020005(NPC* npc);
	static void npc_9101001(NPC* npc);
	static void npc_9900000(NPC* npc);
};

#endif