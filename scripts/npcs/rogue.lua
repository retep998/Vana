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
-- Dark Lord - Thief Instructor

dofile("scripts/lua_functions/npcHelper.lua");
dofile("scripts/lua_functions/jobFunctions.lua");

questState = getPlayerVariable("third_job_advancement", type_int);
if questState == 1 then
	addText("Ah, I was waiting for you. ");
	addText("I heard about you from " .. blue("Arec") .. " in Ossyria, and I'd actually like to test your strength. ");
	addText("There is an opening in the middle of a deep swamp in Victoria Island, which will lead you to a secret passage. ");
	addText("Once inside, you'll face my clone. ");
	addText("Your task is to defeat him and bring the " .. blue(itemRef(4031059)) .. " back with you.");
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

		addText("Give this necklace to " .. blue("Arec") .. " in Ossyria and you will be able to take second test of the 3rd job advancement. ");
		addText("Good luck!");
		sendNext();
	else
		addText("Hmmm...weird. ");
		addText("Are you sure that you have the " .. blue(itemRef(4031059)) .. "? ");
		addText("If you do have it, make sure you have an empty slot in your Item Inventory.");
		sendNext();
	end
elseif isQuestActive(6141) then
	addText("Would you like to go to the training ground?");
	answer = askYesNo();

	if answer == answer_no then
		addText("Let me know when you want to enter.");
		sendOk();
	else
		if not isInstance("ninjaAmbush") then
			createInstance("ninjaAmbush", 5 * 60, true);
			addInstancePlayer(getId());
			setMap(910300000);
		else
			addText("Someone is already inside. ");
			addText("Try again later.");
			sendOk();
		end
	end
elseif getJob() == 0 then
	addText("Want to be a thief? ");
	addText("There are some standards to meet, because we can't just accept EVERYONE in ... ");
	addText(blue("Your level should be at least 10") .. ". ");
	addText("Let's see...");
	sendNext();

	if getLevel() >= 10 then
		addText("Oh...! ");
		addText("You look like someone that can definitely be a part of us...all you need is a little sinister mind, and...yeah...so, what do you think? ");
		addText("Wanna be the Rouge?");
		answer = askYesNo();

		if answer == answer_yes then
			addText("Alright, from here on out, you are part of us! ");
			addText("You'll be living the life of a wanderer at first, but just be patient and soon, you'll be living the high life. ");
			addText("Alright, it ain't much, but I'll give you some of my abilities...");
			addText("HAAAHHH!!");
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

			setJob(400);
			giveItem(1472061, 1);
			giveItem(1332063, 1);
			giveItem(2070015, 3);
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
			addSlots(4, 1);

			addText("I've just created more slots for your equipment and etc. storage. ");
			addText("Not only that, but you've also gotten stronger as well. ");
			addText("As you become part of us, and learn to enjoy life in different angles, you may one day be on top of this of darkness. ");
			addText("I'll be watching your every move, so don't let me down.");
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

			addText("OK! ");
			addText("This is all I can teach you. ");
			addText("Go to places, train and better yourself. ");
			addText("Find me when you feel like you've done all you can, and need something interesting. ");
			addText("Then, and only then, I'll hook you up with more interesting experiences...");
			sendBackNext();

			addText("Oh, and... if you have any questions about being the Thief, feel free to ask. ");
			addText("I don't know EVERYTHING, but I'll help you out with all that I know of. ");
			addText("Til then...");
			sendBackNext();
		else
			addText("I see...well, it's a very important step to take, choosing your job. ");
			addText("But don't you want to live the fun life? ");
			addText("let me know when you have made up your mind, ok?");
			sendNext();
		end
	else
		addText("Hmm...you're still a beginner...doesn't look like you can hang out with us, yet... ");
		addText("make yourself much stronger, THEN find me...");
		sendBackNext();
	end
elseif getJobLine() == line_thief and getJobTrack() > 0 then
	choices = {
		makeChoiceHandler("What are the basic characters of a Thief?", function()
			addText("Let me explain to you what being a thief means. ");
			addText("Thieves have just the right amount of stamina and strength to survive. ");
			addText("We don't recommend training for strength just like those warriors. ");
			addText("What we do need are LUK and DEX...");
			sendNext();

			addText("If you raise the level of luck and dexterity, that will increase the damage you'll dish out to the enemies. ");
			addText("Thieves also differentiate themselves from the rest by using such throwing weapons as throwing stars and throwing knives. ");
			addText("They can also avoid many attacks with high dexterity.");
			sendBackNext();
		end),
		makeChoiceHandler("What are the weapons that the Thieves use?", function()
			addText("Thieves use these weapons. ");
			addText("They have just the right amount of intelligence and power...what we do have are quick movements and even quick brain...");
			sendNext();

			addText("Because of that, we usually use daggers or throwing weapons. ");
			addText("Daggers are basically small swords but they are very quick, enabling you to squeeze in many attacks. ");
			addText("If you fight in a close combat, use the dagger to quickly eliminate the enemy before it reacts to your attack.");
			sendBackNext();

			addText("For throwing weapons there are throwing-stars and throwing-knives available. ");
			addText("You can't just use them by themselves, though. ");
			addText("Go to the weapon store at Kerning City, and they'll sell an claw call Garnier. ");
			addText("Equip yourself with it, then you'll be able to throw the throwing stars that's in the use inventory.");
			sendBackNext();

			addText("Not only is it important to choose the right Claw to use, but it's just as important to choose the right kind of throwing stars to use. ");
			addText("Do you want to know where to get those stars? ");
			addText("Go check out the armor store around this town...there's probably someone that handles those specifically...");
			sendBackNext();
		end),
		makeChoiceHandler("What are the armors that the Thieves use?", function()
			addText("Let me explain to you the armors that the thieves can wear. ");
			addText("Thieves value quick so you need clothes that fit you perfectly. ");
			addText("But then again, they don't need chained armors like the bowman, because it won't help you one bit.");
			sendNext();

			addText("Instead of flashy clothes or tough, hard gold-plated armor, try putting on simple, comfortable clothes that fit you perfectly and still do its job in the process. ");
			addText("It'll help you a lot in hunting the monsters.");
			sendBackNext();
		end),
		makeChoiceHandler("What are the skills available for Thieves?", function()
			addText("For thieves there are skills that supports high accuracy and dexterity we have in general. ");
			addText("A good mix of skills are available, for both the throwing stars and daggers. ");
			addText("Choose your weapon carefully, for skills are determined by it.");
			sendNext();

			addText("If you are using throwing-stars, skills like " .. blue("Keen Eyes") .. " or " .. blue("Lucky Seven") .. " are perfect. ");
			addText("Lucky Seven allows you to throw multiple throwing-stars at once, so it'll help you greatly in hunting down enemies.");
			sendBackNext();

			addText("If you are using daggers choose " .. blue("Disorder") .. " or " .. blue("Double Stab") .. " as skills. ");
			addText("Double Stab, in fact, will be a great skills to use in that it enables you to attack an enemy in a blinding flurry of stabs, a definate stunner indeed.");
			sendBackNext();
		end),
	};

	addText("Do you have anything you want to know about thieves?\r\n");
	addText(blue(choiceList(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
elseif getJob() == 400 and getLevel() >= 30 then
	if getItemAmount(4031011) == 0 and getItemAmount(4031012) == 0 then
		addText("Hmmm...you seem to have gotten a whole lot stronger. ");
		addText("You got rid of the old, weak self and and look much more like a thief now. ");
		addText("Well, what do you think? ");
		addText("Don't you want to get even more powerful than that? ");
		addText("Pass a simple test and I'll do just that for you. ");
		addText("Do you want to do it?");
		answer = askYesNo();

		if answer == answer_no then
			addText("Really? ");
			addText("It will help you out a great deal on your journey if you get stronger fast...if you choose to change your mind in the future, please feel free to come back. ");
			addText("Know that I'll make you much more powerful than you are right now.");
			sendNext();
		else
			addText("Good thinking. ");
			addText("But, I need to make sure you are as strong as you look. ");
			addText("It's not a hard test, one that should be easy for you to pass. ");
			addText("First, take this letter...make sure you don't lose it.");
			sendNext();

			giveItem(4031011, 1);
			addText("Please take this letter to " .. blue(npcRef(1072003)) .. " around " .. blue(mapRef(102040000)) .. " near Kerning City. ");
			addText("He's doing the job of an instructor in place of me. ");
			addText("Give him the letter and he'll give you the test for me. ");
			addText("If you want more details, hear it straight from him. ");
			addText("I'll be wishing you good luck.");
			sendBackNext();
		end
	elseif getItemAmount(4031012) == 0 and getItemAmount(4031011) > 0 then
		addText("Still haven't met the person yet? ");
		addText("Find " .. blue(npcRef(1072003)) .. " who's around " .. blue(mapRef(102040000)) .. " near Kerning City. ");
		addText("Give the letter to him and he may let you know what to do.");
		sendNext();
	elseif getItemAmount(4031012) > 0 and getItemAmount(4031011) == 0 then
		addText("Hmmm...so you got back here safely. ");
		addText("I knew that test would be too easy for you. ");
		addText("I admit, you are a great great thief. ");
		addText("Now...I'll make you even more powerful than you already are. ");
		addText("But, before all that...you need to choose one of two ways. ");
		addText("It'll be a difficult decision for you to make, but...if you have any questions, please ask.");
		sendNext();

		choices = {
			makeChoiceHandler("Please explain the characteristics of the Assassin.", function()
				addText("Let me explain the role of the Assassin. ");
				addText("Assassin is the Thief that uses throwing stars or daggers. ");
				addText("Skills like " .. blue("Claw Mastery") .. " and " .. blue("Critical Throw") .. " will help you use your throwing stars better. ");
				addText("Boost Claw Mastery up more and your maximum number of throwing stars increases, so it'll be best to learn it. ");
				addText("Please remember that.");
				sendNext();

				addText("I'll explain to you one of the skills of the Assassin, " .. blue("Haste") .. ". ");
				addText("It temporarily boost up you and your party members' abilities and moving speed, perfect when facing enemies that are really fast. ");
				addText("It's also useful when walking to a place far far away. ");
				addText("Wouldn't it be much nicer to get to your destination on time as opposed to taking a whole day just to get there?");
				sendBackNext();

				addText("And this is the over skill available for the Assassin, " .. blue("Drain") .. ". ");
				addText("It allows you to take back a portion of the damage you dished out on an enemy and absorb it as HP! ");
				addText("The more the damage, the more you'll regain health...how awesome is that? ");
				addText("Remember the most you can absorb at once is half of your maximum HP. ");
				addText("The higher the enemy's HP, the more you can take away.");
				sendBackNext();
			end),
			makeChoiceHandler("Please explain the characteristics of the Bandit.", function()
				addText("This is what being the Bandit is all about. ");
				addText("Bandits are thieves who specialize in using daggers. ");
				addText("Skills like " .. blue("Dagger Mastery") .. " and " .. blue("Dagger Booster") .. " will help you use your dagger better. ");
				addText("Daggers have quick attacking speed to begin with, and if you add that with a booster, then...oh my! ");
				addText("Fast enough to scare the crap out of the monsters!");
				sendNext();

				addText("I'll explain to you what " .. blue("Steal") .. " does for Bandits. ");
				addText("It gives you a certain probability to let you steal an item from an enemy. ");
				addText("You may only steal once from one enemy, but you can keep trying until you succeed from it. ");
				addText("The stolen item will be dropped onto the ground; make sure you pick it up first because it's anyone's to grab once it's dropped.");
				sendBackNext();

				addText("I'll explain to you what " .. blue("Savage Blow") .. " does for Bandits. ");
				addText("It uses up HP and MP to attack the enemy 6 TIMES with the dagger. ");
				addText("The higher the skill level, the more the attacks may occur. ");
				addText("You'll cut up the enemy to pieces with the dagger...ooooh, isn't it sweet? ");
				addText("What do you think? ");
				addText("Want to become a Bandit and feel the adrenaline rush that comes with it?");
				sendBackNext();
			end),
			makeChoiceHandler("I'll choose my occupation!", function()
				choices = {
					makeChoiceHandler("Assassin", function()
						addText("So you want to make the 2nd job advancement as the " .. blue("Assassin") .. "? ");
						addText("Once you have made the decision, you can't go back and change your mind. ");
						addText("You ARE sure about this, right?");
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
								setJob(410);
								giveSp(1);
								giveItem(4031012, -1);
								addSlots(2, 1);
								newHp = 300 + getRandomNumber(50);
								newMp = 150 + getRandomNumber(50);
								setMaxHp(getRealMaxHp() + newHp);
								setMaxMp(getRealMaxMp() + newMp);

								addText("Alright, from here on out you are the " .. blue("Assassin") .. ". ");
								addText("Assassins revel in shadows and darkness, waiting until the right time comes for them to stick a dagger through the enemy's heart, suddenly and swiftly...please keep training. ");
								addText("I'll make you even more powerful than you are right now!");
								sendNext();

								addText("I have just given you a book that gives you the the list of skills you can acquire as an assassin. ");
								addText("I have also added a whole row to your use inventory, along with boosting up your max HP and MP...go see for it yourself.");
								sendBackNext();

								addText("I have also given you a little bit of " .. blue("SP") .. ". ");
								addText("Open the " .. blue("Skill Menu") .. " located at the bottomleft corner. ");
								addText("You'll be able to boost up the newly-acquired 2nd level skills. ");
								addText("A word of warning though: You can't boost them up all at once. ");
								addText("Some of the skills are only available after you have learned other skills. ");
								addText("Make sure to remember that.");
								sendBackNext();

								addText("Assassins have to be strong. ");
								addText("But remember that you can't abuse that power and use it on a weakling. ");
								addText("Please use your enormous power the right way, because...for you to use that the right way, that is much harder than just getting stronger. ");
								addText("Find me after you have advanced much further.");
								sendBackNext();
							end
						end
					end),
					makeChoiceHandler("Bandit", function()
						addText("So you want to make the 2nd job advancement as the " .. blue("Bandit") .. "? ");
						addText("Once you have made the decision, you can't go back and change your mind. ");
						addText("You ARE sure about this, right?");
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
								setJob(420);
								giveSp(1);
								giveItem(4031012, -1);
								addSlots(2, 1);
								newHp = 300 + getRandomNumber(50);
								newMp = 150 + getRandomNumber(50);
								setMaxHp(getRealMaxHp() + newHp);
								setMaxMp(getRealMaxMp() + newMp);

								addText("Alright from here on out, you're the " .. blue("Bandit") .. ". ");
								addText("Bandits have quick hands and quicker feet to dominate the enemies. ");
								addText("Please keep training. ");
								addText("I'll make you even more powerful than you are right now.");
								sendNext();

								addText("I have just given you a book that gives you the the list of skills you can acquire as an assassin. ");
								addText("I have also added a whole row to your use inventory, along with boosting up your max HP and MP...go see for it yourself.");
								sendBackNext();

								addText("I have also given you a little bit of " .. blue("SP") .. ". ");
								addText("Open the " .. blue("Skill Menu") .. " located at the bottomleft corner. ");
								addText("You'll be able to boost up the newly-acquired 2nd level skills. ");
								addText("A word of warning though: You can't boost them up all at once. ");
								addText("Some of the skills are only available after you have learned other skills. ");
								addText("Make sure to remember that.");
								sendBackNext();

								addText("Assassins have to be strong. ");
								addText("But remember that you can't abuse that power and use it on a weakling. ");
								addText("Please use your enormous power the right way, because...for you to use that the right way, that is much harder than just getting stronger. ");
								addText("Find me after you have advanced much further.");
								sendBackNext();
							end
						end
					end),
				};

				addText("Hmmm, have you made up your mind? ");
				addText("Then choose the 2nd job advancement of your liking.\r\n");
				addText(blue(choiceList(choices)));
				choice = askChoice();

				selectChoice(choices, choice);
			end),
		};

		addText("Alright, when you have made your decision, click on [I'll choose my occupation!] at the very bottom.\r\n");
		addText(blue(choiceList(choices)));
		choice = askChoice();

		selectChoice(choices, choice);
	end
else
	addText("Exploring is good, and getting stronger is good and all... but don't you want to enjoy living the life as you know it? ");
	addText("How about becoming a Rouge like us and really LIVE the life? ");
	addText("Sounds fun, isn't it?");
	sendNext();
end