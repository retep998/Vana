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
