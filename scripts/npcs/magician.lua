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
-- Grendel the Really Old - Magician Instructor

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/jobFunctions.lua");

questState = getPlayerVariable("third_job_advancement", type_int);
if questState == 1 then
	addText("Ah, I was waiting for you. ");
	addText("I heard about you from " .. blue("Robeira") .. " in Ossyria, and I'd actually like to test your strength. ");
	addText("There is a secret passage near the forest of Ellinia, in which only you are allowed to enter. ");
	addText("There, you will meet my other self. ");
	addText("Defeat him and bring the " .. blue(itemRef(4031059)) .. " to me.");
	sendNext();

	addText("My the other self is quite strong. ");
	addText("He uses many special skills and you should fight with him 1 on 1. ");
	addText("However, people cannot stay long in the secret passage, so it is important to beat him ASAP. ");
	addText("Well... ");
	addText("Good luck! ");
	addText("I will look forward to you bringing " .. blue(itemRef(4031059)) .. " to me.");
	sendBackNext();

	setPlayerVariable("third_job_advancement", 2);
elseif questState == 2 then
	if getItemAmount(4031059) > 0 then
		addText("Wow! ");
		addText("You beat my other self and brought the " .. blue(itemRef(4031059)) .. " to me. ");
		addText("This surely proves your strength and that you are ready to advance to the 3rd job. ");
		addText("As I promised, I will give " .. blue(itemRef(4031057)) .. " to you. ");
		sendNext();

		giveItem(4031059, -1);
		giveItem(4031057, 1);

		addText("Give this necklace to " .. blue("Robeira") .. " in Ossyria and you will be able to take second test of the 3rd job advancement. ");
		addText("Good luck!");
		sendNext();
	else
		addText("Hmmm...weird. ");
		addText("Are you sure that you have the " .. blue(itemRef(4031059)) .. "? ");
		addText("If you do have it, make sure you have an empty slot in your Item Inventory.");
		sendNext();
	end
elseif getJob() == 0 then
	addText("Do you want to be a Magician? ");
	addText("You need to meet some requirements in order to do so. ");
	addText("You need to be at least at " .. blue("Level 8") .. ". ");
	addText("Let's see if you have what it takes to become a Magician...");
	sendNext();

	if getLevel() >= 8 then
		addText("You definitely have the look of a Magician. ");
		addText("You may not be there yet, but I can see the Magician in you...what do you think? ");
		addText("Do you want to become the Magician?");
		answer = askYesNo();

		if answer == answer_yes then
			addText("Alright, you're a Magician from here on out, since I, Grendel the Really old, the head Magician, allow you so. ");
			addText("It isn't much, but I'll give you a little bit of what I have...");
			sendNext();

			if getLevel() >= 30 then
				addText("I think you've made the job advancement way too late. ");
				addText("Usually, for beginners under Level 29 that were late in making job advancements, we compensate them with lost Skill Points, that weren't rewarded, but...");
				addText("I think you're a little too late for that. ");
				addText("I am so sorry, but there's nothing I can do.");
				sendBackNext();

				giveSp(1);
			else
				-- Make up SP for any over-leveling
				giveSp((getLevel() - 8) * 3 + 1);
			end

			setJob(200);
			giveItem(1372043, 1);
			mpInc = 100 + getRandomNumber(50);
			setMaxMp(getRealMaxMp() + mpInc);
			setStr(4);
			setDex(4);
			setInt(20);
			setLuk(4);
			setAp((getLevel() - 1) * 5 - 7);

			addText("You have just equipped yourself with much more magicial power. ");
			addText("Please keep training and make yourself much better...");
			addText("I'll be watching you from here and there...");
			sendNext();

			addText("I just gave you a little bit of " .. blue("SP") .. ". ");
			addText("When you open up the " .. blue("Skill menu") .. " on the lower left corner of the screen, there are skills you can learn by using SP's. ");
			addText("One warning, though: You can't raise it all together all at once. ");
			addText("There are also skills you can acquire only after having ");
			sendNext();

			addText("One more warning. ");
			addText("Once you have chosen your job, try to stay alive as much as you can. ");
			addText("Once you reach that level, when you die, you will lose your experience level. ");
			addText("You wouldn't want to lose your hard-earned experience points, do you?");
			sendBackNext();

			addText("OK! ");
			addText("This is all I can teach you. ");
			addText("Go to places, train and better yourself. ");
			addText("Find me when you feel like you've done all you can, and need something interesting. ");
			addText("I'll be waiting for you here...");
			sendBackNext();

			addText("Oh, and... if you have any questions about being the Magician, feel free to ask. ");
			addText("I don't know EVERYTHING, per se, but I'll help you out with all that I know of. ");
			addText("Til then...");
			sendBackNext();
		else
			addText("Really? ");
			addText("Have to give more thought to it, huh? ");
			addText("Take your time, take your time. ");
			addText("This is not something you should take lightly...come talk to me once your have made your decision...");
			sendNext();
		end
	else
		addText("You need more training to be a Magician. ");
		addText("In order to be one, you need to train yourself to be more powerful than you are right now. ");
		addText("Please come back much stronger.");
		sendBackNext();
	end
elseif getJobLine() == line_magician and getJobTrack() > 0 then
	choices = {
		makeChoiceHandler("What are the basic characteristics of being a Magician?", function()
			addText("I'll tell you more about being a Magician. ");
			addText("Magicians put high levels of magic and intelligence to good use. ");
			addText("They can use the power of nature all around us to kill the enemies, but they are very weak in close combats. ");
			addText("The stamina isn't high, either, so be careful and avoid death at all cost.");
			sendNext();

			addText("Since you can attack the monsters from afar, that'll help you quite a bit. ");
			addText("Try boosting up the level of INT if you want to attack the enemies accurately with your magic. ");
			addText("The higher your intelligence, the better you'll be able to handle your magic...");
			sendBackNext();
		end),
		makeChoiceHandler("What are the weapons that the Magicians use?", function()
			addText("I'll tell you more about the weapons that Magicians use. ");
			addText("Actually, it doesn't mean much for Magicians to attack the opponents with weapons. ");
			addText("Magicians lack power and dexterity, so you will have a hard time even defeating a snail.");
			sendNext();

			addText("If we're talking about the magicial powers, then THAT's a whole different story. ");
			addText("The weapons that Magicians use are blunt weapons, staff, and wands. ");
			addText("Blunt weapons are good for, well, blunt attacks, but...");
			addText("I would not recommend that on Magicians, period...");
			sendBackNext();

			addText("Rather, staffs and wands are the main weaponry of choice. ");
			addText("These weapons have special magicial powers in them, so it enhances the Magicians' effectiveness. ");
			addText("It'll be wise for you to carry a weapon with a lot of magicial powers in it...");
			sendBackNext();
		end),
		makeChoiceHandler("What are the armors the Magicians can wear?", function()
			addText("I'll tell you more about the armors that Magicians can wear. ");
			addText("Honestly, the Magicians don't have much armor to wear since they are weak in physiical strength and low in stamina. ");
			addText("Its defensive abilities isn't great either, so I don't know if it helps a lot or not...");
			sendNext();

			addText("Some armors, however, have the ability to eliminate the magicial power, so it can guard you from magic attacks. ");
			addText("It won't help much, but still better than not warning them at all...so buy them if you have time...");
			sendBackNext();
		end),
		makeChoiceHandler("What are the skills available for Magicians?", function()
			addText("The skills available for Magicians use the high levels of intelligence and magic that Magicians have. ");
			addText("Also available are Magic Guard and Magic Armor, which help Magicians with weak stamina prevent from dying.");
			sendNext();

			addText("The offensive skills are " .. blue("Energy Bolt") .. " and " .. blue("Magic Claw") .. ". ");
			addText("First, Energy Bolt is a skill that applies a lot of damage to the opponent with minimal use of MP.");
			sendBackNext();

			addText("Magic Claw, on the other hand, uses up a lot of MP to attack one opponent TWICE. ");
			addText("But, you can only use Energy Bolt once it's more than 1, so keep that in mind. ");
			addText("Whatever you choose to do, it's all upto you...");
			sendBackNext();
		end),
	};

	addText("Any questions about being a Magician?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
elseif getJob() == 200 and getLevel() >= 30 then
	if getItemAmount(4031009) == 0 and getItemAmount(4031012) == 0 then
		addText("Hmmm...you have grown quite a bit since last time. ");
		addText("You look much different from before, where you looked weak and small...instead now I can definitely feel you presence as the Magician...so...what do you think? ");
		addText("Do you want to get even stronger than you are right now? ");
		addText("Pass a simple test and I can do that for you...do you want to do it?");
		answer = askYesNo();

		if answer == answer_no then
			addText("Really? ");
			addText("It will help you out a great deal on your journey if you get stronger fast...if you choose to change your mind in the future, please feel free to come back. ");
			addText("Know that I'll make you much more powerful than you are right now.");
			sendNext();
		else
			addText("Good...you look strong, alright, but I need to see if it is for real. ");
			addText("The test isn't terribly difficult and you should be able to pass it. ");
			addText("Here, take my letter first. ");
			addText("Make sure you don't lose it.");
			sendNext();

			giveItem(4031009, 1);
			addText("Please get this letter to " .. blue(npcRef(1072001)) .. " around " .. blue(mapRef(101020000)) .. " near Ellinia. ");
			addText("He's doing the role of an instructor in place of me. ");
			addText("He'll give you all the details about it. ");
			addText("Best of luck to you...");
			sendBackNext();
		end
	elseif getItemAmount(4031009) > 0 and getItemAmount(4031012) == 0 then
		addText("Still haven't met the person yet? ");
		addText("Find " .. blue(npcRef(1072001)) .. " who's around " .. blue(mapRef(101020000)) .. " near Ellinia. ");
		addText("Give the letter to him and he may let you know what to do.");
		sendNext();
	elseif getItemAmount(4031009) == 0 and getItemAmount(4031012) > 0 then
		addText("You got back here safely. ");
		addText("Well done. ");
		addText("I knew you'd pass the tests very easily...alright, I'll make you much stronger now. ");
		addText("Before that, though...you need to choose one of the three paths that will be given to you. ");
		addText("It will be a tough decision for you to make, but...if you have any questions about it, feel free to ask.");
		sendNext();

		choices = {
			makeChoiceHandler("Please explain the characteristics of the Wizard of Fire and Poison.", function()
				addText("Allow me to explain the Wizard of Fire and Poison. ");
				addText("They specialize in fire and poision magic. ");
				addText("Skills like " .. blue("Meditation") .. ", that allows you and your whole party's magic ability to increase for a time being, and " .. blue("MP Eater") .. ", which allows you a certain probability of absorbing some of your enemy's MP, are essential to all the attacking Magicians.");
				sendNext();

				addText("I'll explain to you a magic attack called " .. blue("Fire Arrow") .. ". ");
				addText("It fires away flamearrows to the enemies, making it the most powerful skill available for the skills in the 2nd level. ");
				addText("It'll work best on enemies that are weak against fire in general, for the damage will be much bigger. ");
				addText("On the other hand, if you use them on enemies that are strong against fire, the damage will only be half of what it usually is, so keep that in mind.");
				sendBackNext();

				addText("I'll explain to you a magic attack called " .. blue("Poison Breath") .. ". ");
				addText("It fires away venomous bubbles on the enemies, poisoning them in the process. ");
				addText("Once poisoned, the enemy's HP will decrease little by little over time. ");
				addText("If the magic doesn't work too well or the monster has high HP, it may be a good idea to fire enough to kill them with the overdose of poison.");
				sendBackNext();
			end),
			makeChoiceHandler("Please explain the characteristics of the Wizard of Ice and Lightning.", function()
				addText("Allow me to explain the Wizard of Ice and Lightning. ");
				addText("They specialize in ice and lightning magic. ");
				addText("Skills like " .. blue("Meditation") .. ", that allows you and your whole party's magic ability to increase for a time being, and " .. blue("MP Eater") .. ", which allows you a certain probability of absorbing some of your enemy's MP, are essential to all the attacking Magicians.");
				sendNext();

				addText("I'll explain to you a magic attack called " .. blue("Cold Beam") .. ". ");
				addText("It fires away pieces of ice at the enemies, and although not quite as powerful as Fire Arrow, whoever's struck by it will be frozen for a short period of time. ");
				addText("The damage increases much more if the enemy happens to be weak against ice. ");
				addText("The opposite holds true, too, in that if the enemy is used to ice, the damage won't quite be as much, so keep that in mind.");
				sendBackNext();

				addText("I'll explain to you a magic attack called " .. blue("Thunder Bolt") .. ". ");
				addText("It's the only 2nd-level skill for Magicians that can be considered the Total Spell, affecting a lot of monsters at once. ");
				addText("It may not dish out a lot of damage, but the advantage is that it damages all the monsters around you. ");
				addText("You can only attack upto six monsters at once, though. ");
				addText("Still, it's a pretty incredible attack.");
				sendBackNext();
			end),
			makeChoiceHandler("Please explain the characteristics of the Cleric.", function()
				addText("Allow me to explain the Cleric. ");
				addText("Clerics use religious magic on monsters through prayers and incantation. ");
				addText("Skills like " .. blue("Bless") .. ", which temporarily improves the weapon def., magic def., accuracy, avoidability, and " .. blue("Invincible") .. ", which decreases the weapon damage for a certain amount, help magicians overcome their shortcomings ...");
				sendNext();

				addText("Cleric is the only Wizard that can perform recovering magic. ");
				addText("Clerics are the only one that can do recovery magic. ");
				addText("It's called " .. blue("Heal") .. ", and the more MP, INT's, and the skill level for this skill you have, the more HP you may recover. ");
				addText("It also affects your party close by so it's a very useful skill, enabling you to continue to hunt without the help of the potion.");
				sendBackNext();

				addText("Clerics also have a magic attack called " .. blue("Holy Arrow") .. ". ");
				addText("It's a spell that allows the Cleric to fire away phantom arrows at the monsters. ");
				addText("The damage isn't too great, but it can apply tremendous damage to the undead's and other evil-based monsters. ");
				addText("Those monsters are very weak against holy attack. ");
				addText("What do you think, isn't it interesting, right?");
				sendBackNext();
			end),
			makeChoiceHandler("I'll choose my occupation!", function()
				choices = {
					makeChoiceHandler("The Wizard of Fire and Poison", function()
						addText("So you want to make the 2nd job advancement as the " .. blue("Wizard of Fire and Poison") .. "? ");
						addText("Once you have made your decision, you can't go back and change your job anymore. ");
						addText("Are you sure about the decision?");
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
								setJob(210);
								giveSp(1);
								giveItem(4031012, -1);
								addSlots(4, 1);
								newMp = 450 + getRandomNumber(50);
								setMaxMp(getRealMaxMp() + newMp);

								addText("From here on out, you have become the " .. blue("Wizard of Fire and Poison") .. "... ");
								addText("Wizards use high intelligence and the power of nature all around us to take down the enemies...please continue your studies, for one day I may make you much more powerful with my own power...");
								sendNext();

								addText("I have just given you a book that gives you the list of skills you can acquire as the Wizard of Fire and Poison...");
								addText("I've also extended your etc. inventory by added a whole row to it, along with your maximum MP...go see it for yourself.");
								sendBackNext();

								addText("I have also given you a little bit of " .. blue("SP") .. ". ");
								addText("Open the " .. blue("Skill Menu") .. " located at the bottomleft corner. ");
								addText("You'll be able to boost up the newly-acquired 2nd level skills. ");
								addText("A word of warning though: You can't boost them up all at once. ");
								addText("Some of the skills are only available after you have learned other skills. ");
								addText("Make sure to remember that.");
								sendBackNext();

								addText("The Wizards have to be strong. ");
								addText("But remember that you can't abuse that power and use it on a weakling. ");
								addText("Please use your enormous power the right way, because...for you to use that the right way, that is much harder than just getting stronger. ");
								addText("Find me after you have advanced much further ...");
								sendBackNext();
							end
						end
					end),
					makeChoiceHandler("The Wizard of Ice and Lightning", function()
						addText("So you want to make the 2nd job advancement as the " .. blue("Wizard of Ice and Lightning") .. "? ");
						addText("Once you have made your decision, you can't go back and change your job anymore. ");
						addText("Are you sure about the decision?");
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
								setJob(220);
								giveSp(1);
								giveItem(4031012, -1);
								addSlots(4, 1);
								newMp = 450 + getRandomNumber(50);
								setMaxMp(getRealMaxMp() + newMp);

								addText("From here on out, you have become the " .. blue("Wizard of Ice and Lightning") .. "... ");
								addText("Wizards use high intelligence and the power of nature all around us to take down the enemies...please continue your studies, for one day I may make you much more powerful with my own power...");
								sendNext();

								addText("I have just given you a book that gives you the list of skills you can acquire as the Wizard of Ice and Lightning...");
								addText("I've also extended your etc. inventory by added a whole row to it. ");
								addText("Your maximum MP has gone up, too. ");
								addText("Go see for it yourself.");
								sendBackNext();

								addText("I have also given you a little bit of " .. blue("SP") .. ". ");
								addText("Open the " .. blue("Skill Menu") .. " located at the bottomleft corner. ");
								addText("You'll be able to boost up the newly-acquired 2nd level skills. ");
								addText("A word of warning though: You can't boost them up all at once. ");
								addText("Some of the skills are only available after you have learned other skills. ");
								addText("Make sure to remember that.");
								sendBackNext();

								addText("The Wizards have to be strong. ");
								addText("But remember that you can't abuse that power and use it on a weakling. ");
								addText("Please use your enormous power the right way, because...for you to use that the right way, that is much harder than just getting stronger. ");
								addText("Find me after you have advanced much further. ");
								addText("I'll be waiting ...");
								sendBackNext();
							end
						end
					end),
					makeChoiceHandler("Cleric", function()
						addText("So you want to make the 2nd job advancement as the " .. blue("Cleric") .. "? ");
						addText("Once you have made your decision, you can't go back and change your job anymore. ");
						addText("Are you sure about the decision?");
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
								setJob(230);
								giveSp(1);
								giveItem(4031012, -1);
								addSlots(4, 1);
								newMp = 450 + getRandomNumber(50);
								setMaxMp(getRealMaxMp() + newMp);

								addText("Alright, you're a " .. blue("Cleric") .. " from here on out. ");
								addText("Clerics blow life into every living organism here with their undying faith in God. ");
								addText("Never stop working on your faith...then one day, I'll help you become much more powerful...");
								sendNext();

								addText("I have just given you a book that gives you the list of skills you can acquire as the Cleric...");
								addText("I've also extended your etc. inventory by added a whole row to it, along with your maximum MP...go see it for yourself.");
								sendBackNext();

								addText("I have also given you a little bit of " .. blue("SP") .. ". ");
								addText("Open the " .. blue("Skill Menu") .. " located at the bottomleft corner. ");
								addText("You'll be able to boost up the newly-acquired 2nd level skills. ");
								addText("A word of warning though: You can't boost them up all at once. ");
								addText("Some of the skills are only available after you have learned other skills. ");
								addText("Make sure to remember that.");
								sendBackNext();

								addText("The Cleric needs more faith than anything else. ");
								addText("Keep your strong faith in God and treat everyone with respect and dignity they deserve. ");
								addText("Keep working hard and you may one day earn more religious magic power...alright...please find me after you have made more strides. ");
								addText("I'll be waiting for you...");
								sendBackNext();
							end
						end
					end),
				};

				addText("Hmmm, have you made up your mind? ");
				addText("Please select your occupation for the 2nd job advancement.\r\n");
				addText(blue(choiceRef(choices)));
				choice = askChoice();

				selectChoice(choices, choice);
			end),
		};

		addText("Alright, when you have made your decision, click on [I'll choose my occupation!] at the very bottom...\r\n");
		addText(blue(choiceRef(choices)));
		choice = askChoice();

		selectChoice(choices, choice);
	end
else
	addText("Would you like to have the power of nature in itself in your hands? ");
	addText("It may be a long, hard road to be on, but you'll surely be rewarded in the end, reaching the very top of wizardry...");
	sendNext();
end