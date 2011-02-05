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
-- Athena Pierce - Bowman Instructor

dofile("scripts/lua_functions/jobFunctions.lua");

if getJob() == 0 then
	addText("So you want to become the Bowman??? Well...you need to meet some requirements to do so...at ");
	addText("least #bLevel10#k. Let's see...hmm...");
	sendNext();

	if (getLevel() >= 10) then
		addText("You look qualified for this. With a great pair of eyes being able to spot the real monsters ");
		addText("and have the coldhearted skills to shoot the arrow through them...we needed someone like that. ");
		addText("Do you want to become a Bowman?");
		yes = askYesNo();

		if yes == 1 then
			addText("Alright! You are the Bowman from here on out, because I said so...haha here's a little bit ");
			addText("of my power to you...Haahhhh!");
			sendNext();

			if getLevel() >= 30 then -- For the rare "too high level" instance.
				addText("I think you've made the job advancement way too late. Usually, for beginners under Level 29 ");
				addText("that were late in making job advancements, we compensate them with lost Skill Points, ");
				addText("that weren't rewarded, but...I think you're a little too late for that. I am so sorry, but there's nothing I can do.");
				sendBackNext();

				giveSP(1);
			else
				giveSP((getLevel() - 10) * 3 + 1); -- Make up any SP for over-leveling like in GMS
			end

			setJob(300);
			giveItem(1452051, 1); -- Beginner Bow
			giveItem(2060000, 6000); -- Arrow for Bow
			hpinc = 100 + getRandomNumber(50); -- Extra HP Given
			mpinc = 25 + getRandomNumber(25); -- Extra MP Given
			setMaxHP(getRealMaxHP() + hpinc); -- Set HP to current HP plus full amount of hp increase
			setMaxMP(getRealMaxMP() + mpinc); -- Set MP to current MP plus full amount of hp increase
			setSTR(4); -- Stat reset
			setDEX(25);
			setINT(4);
			setLUK(4);
			setAP((getLevel() - 1) * 5 - 12);
			addSlots(1, 1);
			addSlots(2, 1);

			addText("I have added slots for your equipment and etc. inventory. You have also gotten much stronger. ");
			addText("Train harder, and you may one day reach the very top of the bowman. I'll be watching you from ");
			addText("afar. Please work hard.");
			sendNext();

			addText("I just gave you a little bit of #bSP#k. When you open up the #bSkill menu#k on the lower left ");
			addText("corner of the screen, there are skills you can learn by using SP's. One warning, though: You ");
			addText("can't raise it all together all at once. There are also skills you can acquire only after having ");
			addText("learned a couple of skills first.");
			sendNext();

			addText("One more warning. Once you have chosen your job, try to stay alive as much as you can. Once you ");
			addText("reach that level, when you die, you will lose your experience level. You wouldn't want to lose ");
			addText("your hard-earned experience points, do you?");
			sendBackNext();

			addText("OK! This is all I can teach you. Go to places, train and better yourself. Find me when you feel ");
			addText("like you've done all you can, and need something interesting. I'll be waiting for you.");
			sendBackNext();

			addText("Oh, and... if you have any other questions about being the Bowman, feel free to ask. I don't ");
			addText("every single thing about  being the bowman, but I'll answer as many questions as I can. Til then...");
			sendBackNext();
		else
			addText("Really? Have to give more though to it, huh? Take your time, take your time. This is not ");
			addText("something you should take lightly...come talk to me once you have made your decision.");
			sendNext();
		end
	else
		addText("You need to train more. Don't think being the Bowman is a walk in the park...");
		sendBackNext();
	end

elseif (getJobLine() == 3 and getJobTrack() > 0) then
	addText("Do you have any questions regarding the life of the Bowman?\r\n");
	addText("#L0##bWhat are the basic characters of a Bowman?#k#l\r\n");
	addText("#L1##bWhat are the weapons that the Bowman use?#k#l\r\n");
	addText("#L2##bWhat are the armors that the bowman can wear?#k#l\r\n");
	addText("#L3##bWhat are the skills of the Bowman?#k#l");
	explain = askChoice();

	if explain == 0 then
		addText("This is what being a bowman is all about. The bowman possesses just enough stamina and strength. ");
		addText("Their most important ability to use is DEX. They don't have much of a stamina, so please avoid ");
		addText("close combat if possible.");
		sendNext();

		addText("The main advantage is that you can attack from afar, enabling you to avoid many close attacks by the ");
		addText("monsters. Not only that, but with high dexterity, you can avoid quite attacks up close. The higher the ");
		addText("DEX, the more damage you can dish out.");
		sendBackNext();
	elseif explain == 1 then
		addText("I'll explain the weapons that bowman use. Instead of using weapons to strike or slash the opponents, ");
		addText("they use long-distance weapons such as bows and rockbows to kill the monsters. They both have their ");
		addText("share of advantages and disadvantages.");
		sendNext();

		addText("Bows aren't as powerful as the rockbows, but they are much quicker to attack with. Rockbows, on the other ");
		addText("hand, are more powerful with less quickness. It'll be hard for you to make a decision on this...");
		sendBackNext();

		addText("Good arrows and rockbows are available through monsters, so it's a must that you hunt often. It won't ");
		addText("be easy to obtain, however. Train yourself harder each and everyday, and you'll see some success coming your way ...");
		sendBackNext();
	elseif explain == 2 then
		addText("I'll explain the armors the bowman use. They need to move around quickly so it won't be any good to ");
		addText("put on huge, elaborate armor. Clothes with long cumbersome laces are definitely off limits.");
		sendNext();

		addText("But if you wear huge stiff armor that the warriors don, you'll be surrounded by the enemies in no time. ");
		addText("Equip yourself with simple, comfortable armor that fits you just fine and still does the job. It'll help ");
		addText("you a great deal when hunting down the monsters.");
		sendBackNext();
	elseif explain == 3 then
		addText("For bowman, skills that are available are the ones that puts their high accuracy and dexterity to good ");
		addText("use. It's a must for the Bowman to acquire skills that allows them to attack the enemies accurately.");
		sendNext();

		addText("There are two kinds of offensive skills for the bowman: #bArrow Blow#k and #bDouble Shot#k. Arrow Blow is ");
		addText("a nice, basic skills that allows you to highly damage the enemy with minimal use of MP.");
		sendBackNext();

		addText("On the other hand, Double Shot allows you to attack the enemy twice using some MP. You'll only be able to ");
		addText("get it after boosting Arrow Blow to at least higher than 1, so remember that. Whatever the choice, make it your own.");
		sendBackNext();
	end

elseif (getJob() == 300 and getLevel() >= 30) then 
	if (getItemAmount(4031010) == 0 and getItemAmount(4031012) == 0) then 
		addText("Hmmm...you have grown a lot since I last saw you. I don't see the weakling I saw before, and instead, look much ");
		addText("more like a bowman now. Well, what do you think? Don't you want to get even more powerful than that? Pass a simple ");
		addText("test and I'll do just that for you. Do you want to do it?");
		yes = askYesNo();

		if yes == 0 then
			addText("Really? Have to give it more thought, huh? Take your time, take your time. This is not something you ");
			addText("should take lightly...come talk to me once you have made your decision.");
			sendNext();

		else
			addText("Good decision. You look strong, but I need to see if you really are strong enough to pass the test. It's not ");
			addText("a difficult test, so you'll do just fine. Here, take my letter first...make sure you don't lose it!");
			sendNext();

			giveItem(4031010, 1);
			addText("Please get this letter to #b#p1072002##k who's around #b#m106010000##k near Henesys. ");
			addText("She's taking care of the the job of an instructor in place of me. Give her the letter and she'll test you ");
			addText("in place of me. Best of luck to you.");
			sendBackNext();
		end
	elseif (getItemAmount(4031010) == 1 and getItemAmount(4031012) == 0) then
		addText("Still haven't met the person yet? Find #b#p1072002##k who's around #b#m106010000##k ");
		addText("near Henesys. Give the letter to her and she may let you know what to do.");
		sendNext();
	elseif (getItemAmount(4031010) == 0 and getItemAmount(4031012) == 1) then
		addText("Haha...I knew you'd breeze through that test. I'll admit, you are a great bowman. I'll make you much stronger ");
		addText("than you are right now. Before that, however...you'll need to choose one of two ppaths given to you. It'll be ");
		addText("a difficult decision for you to make, but...if there's any question to ask, please do so.");
		sendNext();

		addText("Alright, when you have made your decision, click on [I'll choose my occupation!] at the very bottom.\r\n");
		addText("#L0##bPlease explain to me what being the Hunter is all about.#k#l\r\n");
		addText("#L1##bPlease explain to me what being the Crossbowman is all about.#k#l\r\n");
		addText("#L2##bI'll choose my occupation!#k#l\r\n");
		choice = askChoice();

		if choice == 0 then
			addText("Ok. This is what being the Hunter is all about. Hunters have skills such as Bow Mastery and Bow Booster ");
			addText("that enables you to use bows well. There's also a skill called Soul Arrow : Bow for the Hunters that waste ");
			addText("quite a few arrows. It allows you to fire away arrows for a long period of time without actually wasting ");
			addText("the arrows, so if you may have spent some mesos before on arrows, this may be just for you...");
			sendNext();

			addText("I'll explain to you more about one of the skills of the Hunter, #bPower Knock-Back#k. No one beats Hunter ");
			addText("in terms of long-range attacks, but it's a whole different story when there's a lot of enemies or if you need ");
			addText("to attack them up close. Therefore, it makes this skill very important to acquire. It allows you not only to strike ");
			addText("the enemy up close, but also send multiple monsters far back. It's a very important skill to have to acquire ");
			addText("some much-needed space.");
			sendBackNext();

			addText("I'll explain to you the offensive skill of the Hunter, #bArrow Bomb : Bow#k. It's a skill that allows you to fire ");
			addText("away arrows with bombs. If struck just right, the bomb will go off on the enemy, damaging those around it and ");
			addText("temporarily knocking them out. Combine that skill with the Critical Shot, the first level skill, and the damage will be ");
			addText("incredible. You should try becoming the Hunter for your job advancement.");
			sendBackNext();
		elseif choice == 1 then
			addText("Ok. This is what being the Crossbowman is all about. For the Crossbowman, skills like Crossbow Mastery and Crossbow Booster ");
			addText("are available along with Soul Arrow : Bow for those who wastes the bows by shooting a lot and missing a lot. This skill enables ");
			addText("the player to shoot the arrows for a long period of time without wasting the bows, so if you have been spending a lot of mesos ");
			addText("on bows, you may want to check it out...");
			sendNext();

			addText("Ok. One of the skills that the Crossbowman can have is #bPower Knock-Back#k. No one can approach the long-distance attacks of ");
			addText("the Crossbowman, but it's a different story altogether when talking about close combats or facing lots of enemies at once. ");
			addText("For that, this is a very important skill to acquire. It allows you to strike down the enemy with full force, sending ");
			addText("a number of enemies far back in the process. A very important skill that provides you with some much-needed space.");
			sendBackNext();

			addText("Ok, I'll explain to you one of the attacking skills for the Crossbowman, #bIron Arrow : Crossbow#k. This skill enables ");
			addText("you to attack multiple enemies, as the arrow that hits a monster will go through it and hit another monster behind one. ");
			addText("The damage decreases an arrow goes through an enemy, but it can still attack a number of enemies at once, a very Threaten skill to have. ");
			addText("And...if it's combined with Critical Shot...that will be just incredible.");
			sendBackNext();
		elseif choice == 2 then
			addText("Hmmm, have you made up your mind? Then choose the 2nd job advancement of your liking.\r\n");
			addText("#L0##bHunter#k#l\r\n");
			addText("#L1##bCrossbowman#k#l\r\n");
			job = askChoice();

			if job == 0 then
				addText("So you want to make the second job advancement as the #bHunter#k? You know you won't be able to choose a different ");
				addText("job for the second job advancement once you make your decision here, right?");
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
						setJob(310);
						giveSP(1);
						giveItem(4031012, -1); -- Take The Proof of a Hero
						addSlots(4, 1); -- Add inventory slots
						newhp = 300 + getRandomNumber(50); -- Extra HP calculated
						setMaxHP(getRealMaxHP() + newhp); -- Add to current HP
						newmp = 150 + getRandomNumber(50); -- Extra MP calculated
						setMaxMP(getRealMaxMP() + newmp); -- Add to current MP
						addText("Alright, you're the #bHunter#k from here on out. Hunters are the intelligent bunch with incredible vision, able to ");
						addText("pierce the arrow through the heart of the monsters with ease...please train yourself each and everyday. ");
						addText("We'll help you become even stronger than you already are.");
						sendNext();

						addText("I have just given you a book that gives you the the list of skills you can acquire as a hunter. Also ");
						addText("your etc. inventory has expanded by adding another row to it. Your max HP and MP have also increased, too. ");
						addText("Go check and see for it yourself.");
						sendBackNext();

						addText("I have also given you a little bit of #bSP#k. Open the #bSkill Menu#k located at the bottomleft ");
						addText("corner. You'll be able to boost up the newly-acquired 2nd level skills. A word of warning ");
						addText("though: You can't boost them up all at once. Some of the skills are only available after ");
						addText("you have learned other skills. Make sure to remember that.");
						sendBackNext();

						addText("Hunters need to be strong. But remember that you can't abuse that power and use it on a ");
						addText("weakling. Please use your enormous power the right way, because...for you to use that the right way, ");
						addText("that is much harder than just getting stronger. Find me after you have advanced much further. ");
						addText("I'll be waiting for you.");
						sendBackNext();
					end
				end
			elseif job == 1 then
				addText("So you want to make the second job advancement as the #bCrossbowman#k? You know you won't be able to choose a different ");
				addText("job for the second job advancement once you make your decision here, right?");
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
						setJob(320);
						giveSP(1);
						giveItem(4031012, -1); -- Take The Proof of a Hero
						addSlots(4, 1); -- Add inventory slots
						newhp = 300 + getRandomNumber(50); -- Extra HP calculated
						setMaxHP(getRealMaxHP() + newhp); -- Add to current HP
						newmp = 150 + getRandomNumber(50); -- Extra MP calculated
						setMaxMP(getRealMaxMP() + newmp); -- Add to current MP
						addText("Alright, you're the #bCrossbowman#k from here on out. Crossbowman are the intelligent bunch with incredible vision, able to ");
						addText("pierce the arrow through the heart of the monsters with ease...please train yourself each and everyday. ");
						addText("We'll help you become even stronger than you already are.");
						sendNext();

						addText("I have just given you a book that gives you the the list of skills you can acquire as a hunter. Also ");
						addText("your etc. inventory has expanded by adding another row to it. Your max HP and MP have also increased, too. ");
						addText("Go check and see for it yourself.");
						sendBackNext();

						addText("I have also given you a little bit of #bSP#k. Open the #bSkill Menu#k located at the bottomleft ");
						addText("corner. You'll be able to boost up the newly-acquired 2nd level skills. A word of warning ");
						addText("though: You can't boost them up all at once. Some of the skills are only available after ");
						addText("you have learned other skills. Make sure to remember that.");
						sendBackNext();

						addText("Crossbowmen need to be strong. But remember that you can't abuse that power and use it on a ");
						addText("weakling. Please use your enormous power the right way, because...for you to use that the right way, ");
						addText("that is much harder than just getting stronger. Find me after you have advanced much further. ");
						addText("I'll be waiting for you.");
						sendBackNext();
					end
				end
			end
		end
	end
else
	addText("Don't you want to feel the excitement of hunting down the monsters from out of nowhere? Only ");
	addText("the Bowman can do that...");
	sendNext();
end
