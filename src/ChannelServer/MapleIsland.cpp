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

void QuestsScripts::npc_2000s(NPC* npc){
	int state = npc->getState();
	if(state == 0){
		npc->addText("Hey there, Pretty~ I am Roger who teachs you adroable new Maplers with lots of information.");
		npc->sendNext();
	}
	else if(state == 1){
		npc->addText("I know you are busy! Please spare me some time~ I can teach you some useful information! Ahahaha!");
		npc->sendBackNext();
	}
	else if(state == 2){
		npc->addText("So..... Let me just do this for fun! Abaracadabra~!");
		npc->sendAcceptDecline();
	}
	else if(state == 3){
		if(npc->getSelected() == ACCEPT){
			npc->setPlayerHP(25);
			npc->giveItem(2010007, 1);
			npc->addQuest(1021);
			npc->setState(npc->getState()+1);
			npc->addText("Surprised? If HP becomes 0, then you are in trouble. Now, I will give you #r#t2010007##k. Please take it. ");
			npc->addText("You will feel stronger. Open the Item window and double click to consume. Hey, It's very simple to open the Item window. Just press #bI#k on your keyboard.");
			npc->sendNext();
		}
		else{
			npc->addText("I can't believe you just have turned down a attractive guys like me!");
			npc->sendNext();
			npc->end();
		}
	}
	else if(state == 4){
		npc->addText("Surprised? If HP becomes 0, then you are in trouble. Now, I will give you #r#t2010007##k. Please take it. ");
		npc->addText("You will feel stronger. Open the Item window and double click to consume. Hey, It's very simple to open the Item window. Just press #bI#k on your keyboard.");
		npc->sendNext();
	}
	else if(state == 5){
			npc->addText("Please take all #t2010007#s that I gave you. You will be able to see the HP bar increasing. "); 
			npc->addText("Please talk to me again when you recover your HP 100%");
			npc->sendBackOK();
	}
	else if(state == 6){
		npc->end();
	}
}

void QuestsScripts::npc_2000e(NPC* npc){
	int state = npc->getState();
	if(state == 0){
		npc->addText("How easy is it to consume the item? Simple, right? You can set a #bhotkey#k on the right bottom slot. Haha you didn't know that! right? ");
		npc->addText("Oh, and if you are a begineer, HP will automatically recover itself as time goes by. Well it takes time but this is one of the strategies for the beginners.");
		npc->sendNext();
	}
	else if(state == 1){
		npc->addText("Alright! Now that you have learned alot, I will give you a present. This is a must for your travle in Maple World, so thank me! Please use this under emergency cases!");
		npc->sendBackNext();
	}
	else if(state == 2){
		npc->addText("Okay, this is all I can teach you. I know it's sad but it is time to say good bye. Well tack care of yourself and Good luck my friend!\r\n\r\n");
		npc->addText("#fUI/UIWindow.img/QuestIcon/4/0#\r\n#v2010000# 3 #t2010000#\r\n#v2010009# 3 #t2010009#\r\n\r\n#fUI/UIWindow.img/QuestIcon/8/0# 10 exp");	
		npc->sendBackNext();
	}
	else if(state == 3){
		npc->endQuest(1021);
		npc->giveItem(2010000, 3);
		npc->giveItem(2010009, 3);
		npc->giveEXP(10);
		npc->end();
		
	}

}

void NPCsScripts::npc_2020005(NPC* npc){
	char npcs[10][20] = {"0022000", "2012019", "2030002", "2030009", "2081006", "1061013", "2010003", "1052006", "1061000", "2060003"};
	char npcnames[10][20] = {"Shanks", "Moppie", "Corporal Easy", "Glivver", "Moira", "Gwin", "Neve", "Jake", "Chrishrama", "Melias"};
	char npcmaps[10][30] = {"Southperry", "Orbis", "Cloud Park VI", "Ice Valley II", "Cave of Life - Entrance", "Another Entrance", "Orbis Park", "Subway Ticketing Booth", "Sleepywood", "Department Store"};
	char scrolls[14][20] = {"2043001", "2043101", "2043201", "2043301", "2043701", "2043801", "2044001", "2044101", "2044201", "2044301", "2044401", "2044501", "2044601", "2044701"};
	int state = npc->getState();
	if(state == 0){
		npc->addText("In my old age, I've forgotten many of my friends names from far and distant lands. Can you help me remember them?");
		npc->sendAcceptDecline();
	}
	else if(state == 1){
		if(npc->getSelected() == DECLINE){
			npc->end();
			return;
		}
		npc->addText("Thank you. My mind's sharpness has left me years ago *chuckles*. My pocketbook still contains many pictures of friends from long ages past. Many of my friends I'm sure you'll find very familiar, ");
		npc->addText("however some of my newer friends will probably be just as difficult for you as it is for me. I'm sure you've had the ability to meet all of these people. After all, that ship in Cloud City can only take you so far, mmm?");
		npc->sendNext();
	}
	else if(state == 2){
		npc->addText("I have two questions for you for every picture in my pocketbook, and I have ten forgetful memories. #bIf you could be so kind as to not only tell me the name of the person, but also where I might find that person?#k ");
		npc->addText("Should you say something that perhaps jogs my memory a bit, perhaps I will reward you. Take as long as you like - by Merlin's beard I'm in no hurry. Are you ready to begin this quiz?");
		npc->sendYesNo();
	}
	else if(state >= 3 && state <= 32){
		if(npc->getSelected() == NO){
			npc->end();
			return;
		}
		if((state-3)%3 == 0){
			if(state-3!=0){
				if(strcmp(npc->getText(), npcmaps[(state-3)/3-1]) == 0){
					npc->setVariable("count", npc->getVariable("count")+1);
				}
			}
			if((state-3)/3+1>=10){
				npc->addChar(((state-3)/3)/10+'1');
				npc->addChar(((state-3)/3+1)%10+'0');
			}
			else
				npc->addChar((state-3)/3+'1');
			npc->addText(".\r\n#fNpc/");
			npc->addText(npcs[(state-3)/3]);
			npc->addText(".img/stand/0#");
			npc->sendNext();
		}
		else if((state-3)%3 == 1){
			npc->addText("What is his name?");
			npc->sendGetText();
		}
		else if((state-3)%3 == 2){
			if(strcmp(npc->getText(), npcnames[(state-3)/3]) == 0){
				npc->setVariable("count", npc->getVariable("count")+1);
			}
			npc->addText("And where can I find him?");
			npc->sendGetText();
		}
	}
	else if(state == 33){
		if(strcmp(npc->getText(), npcmaps[(state-3)/3-1]) == 0){
			npc->setVariable("count", npc->getVariable("count")+1);
		}
		npc->addText("Thank you so much for hel... hold on just a moment. Ah, dear me! I left a list of these people's names in here should I ever forget. Wonderful, now I feel foolish! ");
		npc->addText("Well, a deal is a deal. I'll reward you based upon the ones you got correct.");
		npc->sendNext();
	}
	else if(state == 34){
		int count = npc->getVariable("count");
		if(count <= 5){
			npc->addText("You get nothing. You don't know your NPC!");
		}
		else if(count <= 10){
			npc->addText("#fUI/UIWindow.img/QuestIcon/7/0#\r\n#fItem/Special/0900.img/09000003/iconRaw/0# 1,000,000 Mesos");
		}
		else if(count <= 15){
			npc->addText("#fUI/UIWindow.img/QuestIcon/7/0#\r\n#fItem/Special/0900.img/09000003/iconRaw/0# 3,000,000 Mesos\r\n\r\n#fUI/UIWindow.img/QuestIcon/4/0#\r\n#v2070005# #t2070005#");
		}
		else if(count <= 19){
			npc->addText("#fUI/UIWindow.img/QuestIcon/7/0#\r\n#fItem/Special/0900.img/09000003/iconRaw/0# 10,000,000 Mesos\r\n\r\n#fUI/UIWindow.img/QuestIcon/4/0#\r\n#v2070005# #t2070005#\r\n\r\n#fUI/UIWindow.img/QuestIcon/3/0#");
			for(int i=0; i<14; i++){
				npc->addText("\r\n#L");
				if(i>=10)
					npc->addChar(i/10+'0');
				npc->addChar(i%10+'0');
				npc->addText("##v");
				npc->addText(scrolls[i]);
				npc->addText("# #t");
				npc->addText(scrolls[i]);
				npc->addText("##l");
			}
			npc->addText("\r\n");
		}
		else{
			npc->addText("#fUI/UIWindow.img/QuestIcon/7/0#\r\n#fItem/Special/0900.img/09000003/iconRaw/0# 12,000,000 Mesos\r\n\r\n#fUI/UIWindow.img/QuestIcon/4/0#\r\n#v2070006# #t2070006#\r\n#v4001102# Alcaster's Statue\r\n\r\n#fUI/UIWindow.img/QuestIcon/3/0#");
			for(int i=0; i<14; i++){
				npc->addText("\r\n#L");
				if(i>=10)
					npc->addChar(i/10+'0');
				npc->addChar(i%10+'0');
				npc->addText("##v");
				npc->addText(scrolls[i]);
				npc->addText("# #t");
				npc->addText(scrolls[i]);
				npc->addText("##l");
			}
			npc->addText("\r\n");
		}
		if(count>15)
			npc->sendSimple();
		else
			npc->sendOK();
	}
	else if(state == 35){
		int count = npc->getVariable("count");
		if(count <= 5){
		}
		else if(count <= 10){
			npc->giveMesos(1000000);
		}
		else if(count <= 15){
			npc->giveMesos(3000000);
			npc->giveItem(2070005, 1);
		}
		else if(count <= 19){
			npc->giveMesos(10000000);
			npc->giveItem(2070005, 1);
			npc->giveItem(atoi(scrolls[npc->getSelected()]), 1);
		}
		else {
			npc->giveMesos(12000000);
			npc->giveItem(4001102, 1);
			npc->giveItem(2070006, 1);
			npc->giveItem(atoi(scrolls[npc->getSelected()]), 1);
		}
		npc->end();

	}
}
