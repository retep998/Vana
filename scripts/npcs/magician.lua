--[[
Copyright (C) 2008-2011 Vana Development Team

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
--]]
-- Grendel the Really Old - Magician Instructor

dofile("scripts/lua_functions/jobFunctions.lua");

if getJob() == 0 then
	addText("Do you want to be a Magician? You need to meet some requirements in ");
	addText("order to do so. You need to be at least at #bLevel 8#k. ");
	addText("Let's see if you have what it takes to become a Magician...");
	sendNext();

	if (getLevel() >= 8) then
		addText("You definitely have the look of a Magician. You may not be there yet, but I can ");
		addText("see the Magician in you...what do you think? Do you want to become the Magician?");
		yes = askYesNo();

		if yes == 1 then
			addText("Alright, you're a Magician from here on out, since I, Grendel the Really old, the ");
			addText("head Magician, allow you so. It isn't much, but I'll give you a little bit of what I have...");
			sendNext();

			if getLevel() >= 30 then -- For rare "too high level" instance.
				addText("I think you've made the job advancement way too late. Usually, for beginners under Level 29 ");
				addText("that were late in making job advancements, we compensate them with lost Skill Points, ");
				addText("that weren't rewarded, but...I think you're a little too late for that. I am so sorry, but there's nothing I can do.");
				sendBackNext();

				giveSP(1);
			else
				giveSP((getLevel() - 8) * 3 + 1); -- Make up SP for any over-leveling like in GMS
			end

			setJob(200);
			giveItem(1372043, 1); 
			mpinc = 100 + getRandomNumber(50); 
			setMaxMP(getRealMaxMP() + mpinc); 
			setSTR(4); -- Stat reset
			setDEX(4);
			setINT(20);
			setLUK(4);
			setAP((getLevel() - 1) * 5 - 7);
			addText("You have just equipped yourself with much more magicial power. Please keep training and ");
			addText("make yourself much better...I'll be watching you from here and there...");
			sendNext();

			addText("I just gave you a little bit of #bSP#k. When you open up the #bSkill menu#k on the lower left ");
			addText("corner of the screen, there are skills you can learn by using SP's. One warning, though: You ");
			addText("can't raise it all together all at once. There are also skills you can acquire only after having ");
			sendNext();

			addText("One more warning. Once you have chosen your job, try to stay alive as much as you can. Once you ");
			addText("reach that level, when you die, you will lose your experience level. You wouldn't want to lose ");
			addText("your hard-earned experience points, do you?");
			sendBackNext();

			addText("OK! This is all I can teach you. Go to places, train and better yourself. Find me when you feel ");
			addText("like you've done all you can, and need something interesting. I'll be waiting for you here...");
			sendBackNext();

			addText("Oh, and... if you have any questions about being the Magician, feel free to ask. I don't know ");
			addText("EVERYTHING, per se, but I'll help you out with all that I know of. Til then...");
			sendBackNext();
		else
			addText("Really? Have to give more thought to it, huh? Take your time, take your time. This is not ");
			addText("something you should take lightly...come talk to me once your have made your decision...");
			sendNext();
		end
	else
		addText("You need more training to be a Magician. In order to be one, you need to train ");
		addText("yourself to be more powerful than you are right now. Please come back much stronger.");
		sendBackNext();
	end
elseif (getJobLine() == 2 and getJobTrack() > 0) then
	addText("Any questions about being a Magician?\r\n");
	addText("#L0##bWhat are the basic characteristics of being a Magician?#k#l\r\n");
	addText("#L1##bWhat are the weapons that the Magicians use?#k#l\r\n");
	addText("#L2##bWhat are the armors the Magicians can wear?#k#l\r\n");
	addText("#L3##bWhat are the skills available for Magicians?#k#l");
	explain = askChoice();

	if explain == 0 then
		addText("I'll tell you more about being a Magician. Magicians put high levels of magic and intelligence to ");
		addText("good use. They can use the power of nature all around us to kill the enemies, but they are very ");
		addText("weak in close combats. The stamina isn't high, either, so be careful and avoid death at all cost.");
		sendNext();

		addText("Since you can attack the monsters from afar, that'll help you quite a bit. Try boosting up the level ");
		addText("of INT if you want to attack the enemies accurately with your magic. The higher your intelligence, the ");
		addText("better you'll be able to handle your magic...");
		sendBackNext();
	elseif explain == 1 then
		addText("I'll tell you more about the weapons that Magicians use. Actually, it doesn't mean much for Magicians ");
		addText("to attack the opponents with weapons. Magicians lack power and dexterity, so you will have a hard time ");
		addText("even defeating a snail.");
		sendNext();

		addText("If we're talking about the magicial powers, then THAT's a whole different story. The weapons that ");
		addText("Magicians use are blunt weapons, staff, and wands. Blunt weapons are good for, well, blunt attacks, ");
		addText("but...I would not recommend that on Magicians, period...");
		sendBackNext();

		addText("Rather, staffs and wands are the main weaponry of choice. These weapons have special magicial powers ");
		addText("in them, so it enhances the Magicians' effectiveness. It'll be wise for you to carry a weapon with ");
		addText("a lot of magicial powers in it...");
		sendBackNext();
	elseif explain == 2 then
		addText("I'll tell you more about the armors that Magicians can wear. Honestly, the Magicians don't have much ");
		addText("armor to wear since they are weak in physiical strength and low in stamina. Its defensive abilities ");
		addText("isn't great either, so I don't know if it helps a lot or not...");
		sendNext();

		addText("Some armors, however, have the ability to eliminate the magicial power, so it can guard you from magic ");
		addText("attacks. It won't help much, but still better than not warning them at all...so buy them if you have time...");
		sendBackNext();
	elseif explain == 3 then
		addText("The skills available for Magicians use the high levels of intelligence and magic that Magicians have. ");
		addText("Also available are Magic Guard and Magic Armor, which help Magicians with weak stamina prevent from dying.");
		sendNext();

		addText("The offensive skills are #bEnergy Bolt#k and #bMagic Claw#k. First, Energy Bolt is a skill that applies a ");
		addText("lot of damage to the opponent with minimal use of MP.");
		sendBackNext();

		addText("Magic Claw, on the other hand, uses up a lot of MP to attack one opponent TWICE. But, you can only use ");
		addText("Energy Bolt once it's more than 1, so keep that in mind. Whatever you choose to do, it's all upto you...");
		sendBackNext();
	end
elseif (getJob() == 200 and getLevel() >= 30) then
	if (getItemAmount(4031009) == 0 and getItemAmount(4031012) == 0) then
		addText("Hmmm...you have grown quite a bit since last time. You look much different from before, where you looked weak and ");
		addText("small...instead now I can definitely feel you presence as the Magician...so...what do you think? Do you want to ");
		addText("get even stronger than you are right now? Pass a simple test and I can do that for you...do you want to do it?");
		yes = askYesNo();

		if yes == 0 then
			addText("Really? It will help you out a great deal on your journey if you get stronger fast...if you choose to ");
			addText("change your mind in the future, please feel free to come back. Know that I'll make you much more ");
			addText("powerful than you are right now.");
			sendNext();
		else
			addText("Good...you look strong, alright, but I need to see if it is for real. The test isn't terribly difficult ");
			addText("and you should be able to pass it. Here, take my letter first. Make sure you don't lose it.");
			sendNext();

			giveItem(4031009, 1);
			addText("Please get this letter to #b#p1072001##k around #b#m101020000##k near Ellinia. ");
			addText("He's doing the role of an instructor in place of me. He'll give you all the details about it. ");
			addText("Best of luck to you...");
			sendBackNext();
		end
	elseif (getItemAmount(4031009) > 0 and getItemAmount(4031012) == 0) then
		addText("Still haven't met the person yet? Find #b#p1072001##k who's around #b#m101020000#k ");
		addText("near Ellinia. Give the letter to him and he may let you know what to do.");
		sendNext();
	elseif (getItemAmount(4031009) == 0 and getItemAmount(4031012) > 0) then
		addText("You got back here safely. Well done. I knew you'd pass the tests very easily...alright, I'll make you much ");
		addText("stronger now. Before that, though...you need to choose one of the three paths that will be given to you. ");
		addText("It will be a tough decision for you to make, but...if you have any questions about it, feel free to ask.");
		sendNext();

		addText("Alright, when you have made your decision, click on [I'll choose my occupation!] at the very bottom...\r\n");
		addText("#L0##bPlease explain the characteristics of the Wizard of Fire and Poison.#l#k\r\n");
		addText("#L1##bPlease explain the characteristics of the Wizard of ICe and Lightning.#l#k\r\n");
		addText("#L2##bPlease explain the characteristics of the Cleric.#l#k\r\n");		
		addText("#L3##bI'll choose my occupation!#l#k\r\n");
		choice = askChoice();

		if choice == 0 then
			addText("Allow me to explain the Wizard of Fire and Poison. They specialize in fire and poision magic. Skills like ");
			addText("#bMeditation#k, that allows you and your whole party's magic ability to increase for a time being, and ");
			addText("#bMP Eater#k, which allows you a certain probability of absorbing some of your enemy's MP, are essential ");
			addText("to all the attacking Magicians.");
			sendNext();

			addText("I'll explain to you a magic attack called #bFire Arrow#k. It fires away flamearrows to the enemies, ");
			addText("making it the most powerful skill available for the skills in the 2nd level. It'll work best on enemies that are ");
			addText("weak against fire in general, for the damage will be much bigger. On the other hand, if you use them on enemies ");
			addText("that are strong against fire, the damage will only be half of what it usually is, so keep that in mind.");
			sendBackNext();

			addText("I'll explain to you a magic attack called #bPoison Breath#k. It fires away venomous bubbles on the enemies, ");
			addText("poisoning them in the process. Once poisoned, the enemy's HP will decrease little by little over time. ");
			addText("If the magic doesn't work too well or the monster has high HP, it may be a good idea to fire enough ");
			addText("to kill them with the overdose of poison.");
			sendBackNext();
		elseif choice == 1 then
			addText("Allow me to explain the Wizard of Ice and Lightning. They specialize in ice and lightning magic. Skills like ");
			addText("#bMeditation#k, that allows you and your whole party's magic ability to increase for a time being, and ");
			addText("#bMP Eater#k, which allows you a certain probability of absorbing some of your enemy's MP, are essential ");
			addText("to all the attacking Magicians.");
			sendNext();

			addText("I'll explain to you a magic attack called #bCold Beam#k. It fires away pieces of ice at the enemies, and although ");
			addText("not quite as powerful as Fire Arrow, whoever's struck by it will be frozen for a short period of time. ");
			addText("The damage increases much more if the enemy happens to be weak against ice. The opposite holds true, too, ");
			addText("in that if the enemy is used to ice, the damage won't quite be as much, so keep that in mind.");
			sendBackNext();

			addText("I'll explain to you a magic attack called #bThunder Bolt#k. It's the only 2nd-level skill for Magicians that ");
			addText("can be considered the Total Spell, affecting a lot of monsters at once. It may not dish out a lot of damage, ");
			addText("but the advantage is that it damages all the monsters around you. You can only attack upto six monsters at once, though. ");
			addText("Still, it's a pretty incredible attack.");
			sendBackNext();
		elseif choice == 2 then
			addText("Allow me to explain the Cleric. Clerics use religious magic on monsters through prayers and incantation. ");
			addText("Skills like #bBless#k, which temporarily improves the weapon def., magic def., accuracy, avoidability, and ");
			addText("#bInvincible#k, which decreases the weapon damage for a certain amount, help magicians overcome their shortcomings ...");
			sendNext();

			addText("Cleric is the only Wizard that can perform recovering magic. Clerics are the only one that can do recovery magic. ");
			addText("It's called #bHeal#k, and the more MP, INT's, and the skill level for this skill you have, the more HP ");
			addText("you may recover. It also affects your party close by so it's a very useful skill, enabling you to continue to hunt ");
			addText("without the help of the potion.");
			sendBackNext();

			addText("Clerics also have a magic attack called #bHoly Arrow#k. It's a spell that allows the Cleric to fire away phantom ");
			addText("arrows at the monsters. The damage isn't too great, but it can apply tremendous damage to the undead's and other ");
			addText("evil-based monsters. Those monsters are very weak against holy attack. What do you think, isn't it interesting, right?");
			sendBackNext();
		elseif choice == 3 then
			addText("Now, have you made up your mind? Please select your occupation for the 2nd job advancement.\r\n");
			addText("#L0##bThe Wizard of Fire and Poison#l#k\r\n");
			addText("#L1##bThe Wizard of Ice and Lightning#l#k\r\n");
			addText("#L2##bCleric#l#k\r\n");
			job = askChoice();

			if job == 0 then
				addText("So you want to make the 2nd job advancement as the #bWizard of Fire and Poison#k? Once you have ");
				addText("made your decision, you can't go back and change your job anymore. Are you sure about the decision?");
				yes = askYesNo();

				if yes == 0 then
					addText("Really? Have to give more thought to it, huh? Take your time, take your time. ");
					addText("This is not something you should take lightly ... come talk to me once you have made your decision.");				
					sendNext();
				else
					if getSP() > ((getLevel() - 30) * 3) then
						addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many ");
						addText("SP in store. Use more SP on the skills on the 1st level and then come back.");
						sendNext();
					else
						setJob(210);
						giveSP(1); -- Give necessary SP
						giveItem(4031012, -1); -- Take The Proof of a Hero
						addSlots(4, 1); -- Add inventory slot
						newmp = 450 + getRandomNumber(50); -- Extra MP given
						setMaxMP(getRealMaxMP() + newmp); -- Add to current MP
						addText("From here on out, you have become the #bWizard of Fire and Poison#k... Wizards use high intelligence ");
						addText("and the power of nature all around us to take down the enemies...please continue your studies, ");
						addText("for one day I may make you much more powerful with my own power...");
						sendNext();

						addText("I have just given you a book that gives you the list of skills you can acquire as the Wizard of Fire and Poison...");
						addText("I've also extended your etc. inventory by added a whole row to it, along with your maximum MP...go see it for yourself.");
						sendBackNext();

						addText("I have also given you a little bit of #bSP#k. Open the #bSkill Menu#k located at the bottomleft ");
						addText("corner. You'll be able to boost up the newly-acquired 2nd level skills. A word of warning ");
						addText("though: You can't boost them up all at once. Some of the skills are only available after ");
						addText("you have learned other skills. Make sure to remember that.");
						sendBackNext();

						addText("The Wizards have to be strong. But remember that you can't abuse that power and use it on a ");
						addText("weakling. Please use your enormous power the right way, because...for you to use that the right way, ");
						addText("that is much harder than just getting stronger. Find me after you have advanced much further ...");
						sendBackNext();
					end
				end
			elseif job == 1 then
				addText("So you want to make the 2nd job advancement as the #bWizard of Ice and Lightning#k? Once you have ");
				addText("made your decision, you can't go back and change your job anymore. Are you sure about the decision?");
				yes = askYesNo();

				if yes == 0 then
					addText("Really? Have to give more thought to it, huh? Take your time, take your time. ");
					addText("This is not something you should take lightly ... come talk to me once you have made your decision.");				
					sendNext();
				else
					if getSP() > ((getLevel() - 30) * 3) then
						addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many ");
						addText("SP in store. Use more SP on the skills on the 1st level and then come back.");
						sendNext();
					else
						setJob(220);
						giveSP(1); -- Give necessary SP
						giveItem(4031012, -1); -- Take The Proof of a Hero
						addSlots(4, 1); -- Add inventory slots
						newmp = 450 + getRandomNumber(50); -- Extra MP given
						setMaxMP(getRealMaxMP() + newmp); -- Add to current MP
						addText("From here on out, you have become the #bWizard of Ice and Lightning#k... Wizards use high intelligence ");
						addText("and the power of nature all around us to take down the enemies...please continue your studies, ");
						addText("for one day I may make you much more powerful with my own power...");
						sendNext();

						addText("I have just given you a book that gives you the list of skills you can acquire as the Wizard of Ice and Lightning...");
						addText("I've also extended your etc. inventory by added a whole row to it. Your maximum MP has gone up, too. Go see for it yourself.");
						sendBackNext();

						addText("I have also given you a little bit of #bSP#k. Open the #bSkill Menu#k located at the bottomleft ");
						addText("corner. You'll be able to boost up the newly-acquired 2nd level skills. A word of warning ");
						addText("though: You can't boost them up all at once. Some of the skills are only available after ");
						addText("you have learned other skills. Make sure to remember that.");
						sendBackNext();

						addText("The Wizards have to be strong. But remember that you can't abuse that power and use it on a ");
						addText("weakling. Please use your enormous power the right way, because...for you to use that the right way, ");
						addText("that is much harder than just getting stronger. Find me after you have advanced much further. ");
						addText("I'll be waiting ...");
						sendBackNext();
					end
				end
			elseif job == 2 then
				addText("So you want to make the 2nd job advancement as the #Cleric#k? Once you have ");
				addText("made your decision, you can't go back and change your job anymore. Are you sure about the decision?");
				yes = askYesNo();

				if yes == 0 then
					addText("Really? Have to give more thought to it, huh? Take your time, take your time. ");
					addText("This is not something you should take lightly ... come talk to me once you have made your decision.");				
					sendNext();
				else
					if getSP() > ((getLevel() - 30) * 3) then
						addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many ");
						addText("SP in store. Use more SP on the skills on the 1st level and then come back.");
						sendNext();
					else
						setJob(230);
						giveSP(1); -- Necessary SP given
						giveItem(4031012, -1); -- Take The Proof of a Hero
						addSlots(4, 1); -- Add inventory slots
						newmp = 450 + getRandomNumber(50); -- Extra MP given
						setMaxMP(getRealMaxMP() + newmp); -- Add to current MP
						addText("Alright, you're a #bCleric#k from here on out. Clerics blow life into every living organism here ");
						addText("with their undying faith in God. Never stop working on your faith...then one day, I'll help you ");
						addText("become much more powerful...");
						sendNext();

						addText("I have just given you a book that gives you the list of skills you can acquire as the Cleric...");
						addText("I've also extended your etc. inventory by added a whole row to it, along with your maximum MP...go see it for yourself.");
						sendBackNext();

						addText("I have also given you a little bit of #bSP#k. Open the #bSkill Menu#k located at the bottomleft ");
						addText("corner. You'll be able to boost up the newly-acquired 2nd level skills. A word of warning ");
						addText("though: You can't boost them up all at once. Some of the skills are only available after ");
						addText("you have learned other skills. Make sure to remember that.");
						sendBackNext();

						addText("The Cleric needs more faith than anything else. Keep your strong faith in God and treat everyone with respect ");
						addText("and dignity they deserve. Keep working hard and you may one day earn more religious magic power...");
						addText("alright...please find me after you have made more strides. I'll be waiting for you...");
						sendBackNext();
					end
				end
			end
		end
	end
else
	addText("Would you like to have the power of nature in itself in your hands? It may be a long, ");
	addText("hard road to be on, but you'll surely be rewarded in the end, reaching the very top ");
	addText("of wizardry...");
	sendNext();
end
