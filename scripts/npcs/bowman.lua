--[[
Copyright (C) 2008-2014 Vana Development Team

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

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/jobHelper.lua");

questState = getPlayerVariable("third_job_advancement", type_int);
if questState == 1 then
	addText("Ah, I was waiting for you. ");
	addText("I heard about you from " .. blue("Rene") .. " in Ossyria, and I'd actually like to test your strength. ");
	addText("You will find a Door of Dimension deep inside the Cursed Temple in the heart of Victoria Island. ");
	addText("Nobody but you can go into that passage. ");
	addText("If you go into the passage, you'll face a clone of myself. ");
	addText("Beat her and bring " .. blue(itemRef(4031059)) .. " to me.");
	sendNext();

	addText("Since she is a clone of myself, you can expect a tough battle ahead. ");
	addText("She uses a number of special attacking skills unlike any you have ever seen, and it is your task to successfully take her down. ");
	addText("There is a time limit in the secret passage, so it is crucial that you defeat her fast. ");
	addText("I wish you the best of luck, and I hope you bring the " .. blue(itemRef(4031059)) .. " with you.");
	sendBackNext();

	setPlayerVariable("third_job_advancement", 2);
elseif questState == 2 then
	if getItemAmount(4031059) > 0 then
		addText("Great work there. ");
		addText("You have defeated my clone and brought " .. blue(itemRef(4031059)) .. " back safely. ");
		addText("I'm impressed. ");
		addText("This surely proves your strength. ");
		addText("In terms of strength, you are ready to advance to 3rd job. ");
		addText("As I promised, I will give " .. blue(itemRef(4031057)) .. " to you. ");
		addText("Give this necklace to " .. blue("Rene") .. " in Ossyria and you will be able to take second the test of 3rd job advancement. ");
		addText("Good luck~.");
		sendNext();

		giveItem(4031059, -1);
		giveItem(4031057, 1);
	else
		addText("You will find a Door of Dimension deep inside the Cursed Temple in the heart of Victoria Island. ");
		addText("Nobody but you can go into that passage. ");
		addText("If you go into the passage, you will meet my clone. ");
		addText("Beat her and bring " .. blue(itemRef(4031059)) .. " to me.");
		sendNext();
	end
elseif getJob() == 0 then
	addText("So you want to become the Bowman??? ");
	addText("Well...you need to meet some requirements to do so...at least " .. blue("Level10") .. ". ");
	addText("Let's see...hmm...");
	sendNext();

	if getLevel() >= 10 then
		addText("You look qualified for this. ");
		addText("With a great pair of eyes being able to spot the real monsters and have the coldhearted skills to shoot the arrow through them...we needed someone like that. ");
		addText("Do you want to become a Bowman?");
		answer = askYesNo();

		if answer == answer_yes then
			addText("Alright! ");
			addText("You are the Bowman from here on out, because I said so...haha here's a little bit of my power to you...");
			addText("Haahhhh!");
			sendNext();

			if getLevel() >= 30 then
				addText("I think you've made the job advancement way too late. ");
				addText("Usually, for beginners under Level 29 that were late in making job advancements, we compensate them with lost Skill Points, that weren't rewarded, but...");
				addText("I think you're a little too late for that. ");
				addText("I am so sorry, but there's nothing I can do.");
				sendBackNext();

				giveSp(1);
			else
				-- Make up any SP for over-leveling
				giveSp((getLevel() - 10) * 3 + 1);
			end

			setJob(300);
			giveItem(1452051, 1);
			giveItem(2060000, 6000);
			hpInc = 100 + getRandomNumber(50);
			mpInc = 25 + getRandomNumber(25);
			setMaxHp(getRealMaxHp() + hpInc);
			setMaxMp(getRealMaxMp() + mpInc);
			setStr(4);
			setDex(25);
			setInt(4);
			setLuk(4);
			setAp((getLevel() - 1) * 5 - 12);
			addSlots(1, 1);
			addSlots(2, 1);

			addText("I have added slots for your equipment and etc. inventory. ");
			addText("You have also gotten much stronger. ");
			addText("Train harder, and you may one day reach the very top of the bowman. ");
			addText("I'll be watching you from afar. ");
			addText("Please work hard.");
			sendNext();

			addText("I just gave you a little bit of " .. blue("SP") .. ". ");
			addText("When you open up the " .. blue("Skill menu") .. " on the lower left corner of the screen, there are skills you can learn by using SP's. ");
			addText("One warning, though: You can't raise it all together all at once. ");
			addText("There are also skills you can acquire only after having learned a couple of skills first.");
			sendNext();

			addText("One more warning. ");
			addText("Once you have chosen your job, try to stay alive as much as you can. ");
			addText("Once you reach that level, when you die, you will lose your experience level. ");
			addText("You wouldn't want to lose your hard-earned experience points, do you?");
			sendBackNext();

			addText("OK! This is all I can teach you. ");
			addText("Go to places, train and better yourself. ");
			addText("Find me when you feel like you've done all you can, and need something interesting. ");
			addText("I'll be waiting for you.");
			sendBackNext();

			addText("Oh, and... if you have any other questions about being the Bowman, feel free to ask. ");
			addText("I don't every single thing about being the bowman, but I'll answer as many questions as I can. ");
			addText("Til then...");
			sendBackNext();
		else
			addText("Really? ");
			addText("Have to give more though to it, huh? ");
			addText("Take your time, take your time. ");
			addText("This is not something you should take lightly...come talk to me once you have made your decision.");
			sendNext();
		end
	else
		addText("You need to train more. ");
		addText("Don't think being the Bowman is a walk in the park...");
		sendBackNext();
	end
elseif getJobLine() == line_bowman and getJobTrack() > 0 then
	choices = {
		makeChoiceHandler("What are the basic characters of a Bowman?", function()
			addText("This is what being a bowman is all about. ");
			addText("The bowman possesses just enough stamina and strength. ");
			addText("Their most important ability to use is DEX. ");
			addText("They don't have much of a stamina, so please avoid close combat if possible.");
			sendNext();

			addText("The main advantage is that you can attack from afar, enabling you to avoid many close attacks by the monsters. ");
			addText("Not only that, but with high dexterity, you can avoid quite attacks up close. ");
			addText("The higher the DEX, the more damage you can dish out.");
			sendBackNext();
		end),
		makeChoiceHandler("What are the weapons that the Bowman use?", function()
			addText("I'll explain the weapons that bowman use. ");
			addText("Instead of using weapons to strike or slash the opponents, they use long-distance weapons such as bows and rockbows to kill the monsters. ");
			addText("They both have their share of advantages and disadvantages.");
			sendNext();

			addText("Bows aren't as powerful as the rockbows, but they are much quicker to attack with. ");
			addText("Rockbows, on the other hand, are more powerful with less quickness. ");
			addText("It'll be hard for you to make a decision on this...");
			sendBackNext();

			addText("Good arrows and rockbows are available through monsters, so it's a must that you hunt often. ");
			addText("It won't be easy to obtain, however. ");
			addText("Train yourself harder each and everyday, and you'll see some success coming your way ...");
			sendBackNext();
		end),
		makeChoiceHandler("What are the armors that the bowman can wear?", function()
			addText("I'll explain the armors the bowman use. ");
			addText("They need to move around quickly so it won't be any good to put on huge, elaborate armor. ");
			addText("Clothes with long cumbersome laces are definitely off limits.");
			sendNext();

			addText("But if you wear huge stiff armor that the warriors don, you'll be surrounded by the enemies in no time. ");
			addText("Equip yourself with simple, comfortable armor that fits you just fine and still does the job. ");
			addText("It'll help you a great deal when hunting down the monsters.");
			sendBackNext();
		end),
		makeChoiceHandler("What are the skills of the Bowman?", function()
			addText("For bowman, skills that are available are the ones that puts their high accuracy and dexterity to good use. ");
			addText("It's a must for the Bowman to acquire skills that allows them to attack the enemies accurately.");
			sendNext();

			addText("There are two kinds of offensive skills for the bowman: " .. blue("Arrow Blow") .. " and " .. blue("Double Shot") .. ". ");
			addText("Arrow Blow is a nice, basic skills that allows you to highly damage the enemy with minimal use of MP.");
			sendBackNext();

			addText("On the other hand, Double Shot allows you to attack the enemy twice using some MP. ");
			addText("You'll only be able to get it after boosting Arrow Blow to at least higher than 1, so remember that. ");
			addText("Whatever the choice, make it your own.");
			sendBackNext();
		end),
	};

	addText("Do you have any questions regarding the life of the Bowman?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
elseif getJob() == 300 and getLevel() >= 30 then
	if getItemAmount(4031010) == 0 and getItemAmount(4031012) == 0 then
		addText("Hmmm...you have grown a lot since I last saw you. ");
		addText("I don't see the weakling I saw before, and instead, look much more like a bowman now. ");
		addText("Well, what do you think? ");
		addText("Don't you want to get even more powerful than that? ");
		addText("Pass a simple test and I'll do just that for you. ");
		addText("Do you want to do it?");
		answer = askYesNo();

		if answer == answer_no then
			addText("Really? ");
			addText("Have to give it more thought, huh? ");
			addText("Take your time, take your time. ");
			addText("This is not something you should take lightly...come talk to me once you have made your decision.");
			sendNext();
		else
			addText("Good decision. ");
			addText("You look strong, but I need to see if you really are strong enough to pass the test. ");
			addText("It's not a difficult test, so you'll do just fine. ");
			addText("Here, take my letter first...make sure you don't lose it!");
			sendNext();

			giveItem(4031010, 1);

			addText("Please get this letter to " .. blue(npcRef(1072002)) .. " who's around " .. blue(mapRef(106010000)) .. " near Henesys. ");
			addText("She's taking care of the the job of an instructor in place of me. ");
			addText("Give her the letter and she'll test you in place of me. ");
			addText("Best of luck to you.");
			sendBackNext();
		end
	elseif getItemAmount(4031010) == 1 and getItemAmount(4031012) == 0 then
		addText("Still haven't met the person yet? ");
		addText("Find " .. blue(npcRef(1072002)) .. " who's around " .. blue(mapRef(106010000)) .. " near Henesys. ");
		addText("Give the letter to her and she may let you know what to do.");
		sendNext();
	elseif getItemAmount(4031010) == 0 and getItemAmount(4031012) == 1 then
		addText("Haha...I knew you'd breeze through that test. ");
		addText("I'll admit, you are a great bowman. ");
		addText("I'll make you much stronger than you are right now. ");
		addText("Before that, however...you'll need to choose one of two ppaths given to you. ");
		addText("It'll be a difficult decision for you to make, but...if there's any question to ask, please do so.");
		sendNext();

		choices = {
			makeChoiceHandler("Please explain to me what being the Hunter is all about.", function()
				addText("Ok. This is what being the Hunter is all about. ");
				addText("Hunters have skills such as Bow Mastery and Bow Booster that enables you to use bows well. ");
				addText("There's also a skill called Soul Arrow : Bow for the Hunters that waste quite a few arrows. ");
				addText("It allows you to fire away arrows for a long period of time without actually wasting the arrows, so if you may have spent some mesos before on arrows, this may be just for you...");
				sendNext();

				addText("I'll explain to you more about one of the skills of the Hunter, " .. blue("Power Knock-Back") .. ". ");
				addText("No one beats Hunter in terms of long-range attacks, but it's a whole different story when there's a lot of enemies or if you need to attack them up close. ");
				addText("Therefore, it makes this skill very important to acquire. It allows you not only to strike the enemy up close, but also send multiple monsters far back. ");
				addText("It's a very important skill to have to acquire some much-needed space.");
				sendBackNext();

				addText("I'll explain to you the offensive skill of the Hunter, " .. blue("Arrow Bomb : Bow") .. ". ");
				addText("It's a skill that allows you to fire away arrows with bombs. ");
				addText("If struck just right, the bomb will go off on the enemy, damaging those around it and temporarily knocking them out. ");
				addText("Combine that skill with the Critical Shot, the first level skill, and the damage will be incredible. ");
				addText("You should try becoming the Hunter for your job advancement.");
				sendBackNext();
			end),
			makeChoiceHandler("Please explain to me what being the Crossbowman is all about.", function()
				addText("Ok. This is what being the Crossbowman is all about. ");
				addText("For the Crossbowman, skills like Crossbow Mastery and Crossbow Booster are available along with Soul Arrow : Bow for those who wastes the bows by shooting a lot and missing a lot. ");
				addText("This skill enables the player to shoot the arrows for a long period of time without wasting the bows, so if you have been spending a lot of mesos on bows, you may want to check it out...");
				sendNext();

				addText("Ok. One of the skills that the Crossbowman can have is " .. blue("Power Knock-Back") .. ". ");
				addText("No one can approach the long-distance attacks of the Crossbowman, but it's a different story altogether when talking about close combats or facing lots of enemies at once. ");
				addText("For that, this is a very important skill to acquire. ");
				addText("It allows you to strike down the enemy with full force, sending a number of enemies far back in the process. ");
				addText("A very important skill that provides you with some much-needed space.");
				sendBackNext();

				addText("Ok, I'll explain to you one of the attacking skills for the Crossbowman, " .. blue("Iron Arrow : Crossbow") .. ". ");
				addText("This skill enables you to attack multiple enemies, as the arrow that hits a monster will go through it and hit another monster behind one. ");
				addText("The damage decreases an arrow goes through an enemy, but it can still attack a number of enemies at once, a very Threaten skill to have. ");
				addText("And...if it's combined with Critical Shot...that will be just incredible.");
				sendBackNext();
			end),
			makeChoiceHandler("I'll choose my occupation!", function()
				choices = {
					makeChoiceHandler("Hunter", function()
						addText("So you want to make the second job advancement as the " .. blue("Hunter") .. "? ");
						addText("You know you won't be able to choose a different job for the second job advancement once you make your decision here, right?");
						answer = askYesNo();

						if answer == answer_no then
							addText("Really? ");
							addText("Have to give more thought to it, huh? ");
							addText("Take your time, take your time. ");
							addText("This is not something you should take lightly ... come talk to me once you have made your decision.");
							sendNext();
						else
							if getSp() > (getLevel() - 30) * 3 then
								addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many SP in store. ");
								addText("Use more SP on the skills on the 1st level and then come back.");
								sendNext();
							else
								setJob(310);
								giveSp(1);
								giveItem(4031012, -1);
								addSlots(4, 1);
								newHp = 300 + getRandomNumber(50);
								newMp = 150 + getRandomNumber(50);
								setMaxHp(getRealMaxHp() + newHp);
								setMaxMp(getRealMaxMp() + newMp);

								addText("Alright, you're the " .. blue("Hunter") .. " from here on out. ");
								addText("Hunters are the intelligent bunch with incredible vision, able to pierce the arrow through the heart of the monsters with ease...please train yourself each and everyday. ");
								addText("We'll help you become even stronger than you already are.");
								sendNext();

								addText("I have just given you a book that gives you the the list of skills you can acquire as a hunter. ");
								addText("Also your etc. inventory has expanded by adding another row to it. ");
								addText("Your max HP and MP have also increased, too. ");
								addText("Go check and see for it yourself.");
								sendBackNext();

								addText("I have also given you a little bit of " .. blue("SP") .. ". ");
								addText("Open the " .. blue("Skill Menu") .. " located at the bottomleft corner. ");
								addText("You'll be able to boost up the newly-acquired 2nd level skills. ");
								addText("A word of warning though: You can't boost them up all at once. ");
								addText("Some of the skills are only available after you have learned other skills. ");
								addText("Make sure to remember that.");
								sendBackNext();

								addText("Hunters need to be strong. ");
								addText("But remember that you can't abuse that power and use it on a weakling. ");
								addText("Please use your enormous power the right way, because...for you to use that the right way, that is much harder than just getting stronger. ");
								addText("Find me after you have advanced much further. ");
								addText("I'll be waiting for you.");
								sendBackNext();
							end
						end
					end),
					makeChoiceHandler("Crossbowman", function()
						addText("So you want to make the second job advancement as the " .. blue("Crossbowman") .. "? ");
						addText("You know you won't be able to choose a different job for the second job advancement once you make your decision here, right?");
						answer = askYesNo();

						if answer == answer_no then
							addText("Really? ");
							addText("Have to give more thought to it, huh? ");
							addText("Take your time, take your time. ");
							addText("This is not something you should take lightly ... come talk to me once you have made your decision.");
							sendNext();
						else
							if getSp() > (getLevel() - 30) * 3 then
								addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many SP in store. ");
								addText("Use more SP on the skills on the 1st level and then come back.");
								sendNext();
							else
								setJob(320);
								giveSp(1);
								giveItem(4031012, -1);
								addSlots(4, 1);
								newHp = 300 + getRandomNumber(50);
								newMp = 150 + getRandomNumber(50);
								setMaxHp(getRealMaxHp() + newHp);
								setMaxMp(getRealMaxMp() + newMp);

								addText("Alright, you're the " .. blue("Crossbowman") .. " from here on out. ");
								addText("Crossbowman are the intelligent bunch with incredible vision, able to pierce the arrow through the heart of the monsters with ease...please train yourself each and everyday. ");
								addText("We'll help you become even stronger than you already are.");
								sendNext();

								addText("I have just given you a book that gives you the the list of skills you can acquire as a hunter. ");
								addText("Also your etc. inventory has expanded by adding another row to it. ");
								addText("Your max HP and MP have also increased, too. ");
								addText("Go check and see for it yourself.");
								sendBackNext();

								addText("I have also given you a little bit of " .. blue("SP") .. ". ");
								addText("Open the " .. blue("Skill Menu") .. " located at the bottomleft corner. ");
								addText("You'll be able to boost up the newly-acquired 2nd level skills. ");
								addText("A word of warning though: You can't boost them up all at once. ");
								addText("Some of the skills are only available after you have learned other skills. ");
								addText("Make sure to remember that.");
								sendBackNext();

								addText("Crossbowmen need to be strong. ");
								addText("But remember that you can't abuse that power and use it on a weakling. ");
								addText("Please use your enormous power the right way, because...for you to use that the right way, that is much harder than just getting stronger. ");
								addText("Find me after you have advanced much further. ");
								addText("I'll be waiting for you.");
								sendBackNext();
							end
						end
					end),
				};

				addText("Hmmm, have you made up your mind? ");
				addText("Then choose the 2nd job advancement of your liking.\r\n");
				addText(blue(choiceRef(choices)));
				choice = askChoice();

				selectChoice(choices, choice);
			end),
		};

		addText("Alright, when you have made your decision, click on [I'll choose my occupation!] at the very bottom.\r\n");
		addText(blue(choiceRef(choices)));
		choice = askChoice();

		selectChoice(choices, choice);
	end
else
	addText("Don't you want to feel the excitement of hunting down the monsters from out of nowhere? ");
	addText("Only the Bowman can do that...");
	sendNext();
end