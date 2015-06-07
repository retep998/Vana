--[[
Copyright (C) 2008-2015 Vana Development Team

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
-- Dances With Balrog - Warrior Instructor

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/jobHelper.lua");

questState = getPlayerVariable("third_job_advancement", type_int);
if questState == 1 then
	addText("Ah, I was waiting for you. ");
	addText("I heard about you from " .. blue("Tylus") .. " in Ossyria, and I'd actually like to test your strength. ");
	addText("You will find a Door of Dimension deep inside the Cursed Temple in the heart of Victoria Island. ");
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
		addText("Wow! You beat my other self and brought the " .. blue(itemRef(4031059)) .. " to me. ");
		addText("This surely proves your strength and that you are ready to advance to the 3rd job. ");
		addText("As I promised, I will give " .. blue(itemRef(4031057)) .. " to you. ");
		sendNext();

		giveItem(4031059, -1);
		giveItem(4031057, 1);

		addText("Give this necklace to " .. blue("Tylus") .. " in Ossyria and you will be able to take second test of the 3rd job advancement. ");
		addText("Good luck!");
		sendNext();
	else
		addText("Hmmm...weird. ");
		addText("Are you sure that you have the " .. blue(itemRef(4031059)) .. "? ");
		addText("If you do have it, make sure you have an empty slot in your Item Inventory.");
		sendNext();
	end
elseif getJob() == 0 then
	addText("Do you wish to be a Warrior? ");
	addText("You need to meet some criteria in order to do so. ");
	addText(blue("You need to be at least in Level 10") .. ". ");
	addText("Let's see...");
	sendNext();

	if getLevel() >= 10 then
		addText("You definitely have the look of a Warrior. ");
		addText("You may not be there just yet, but I can see the Warrior in you. ");
		addText("What do you think? ");
		addText("Do you want to become a Warrior?");
		answer = askYesNo();

		if answer == answer_yes then
			addText("From here on out, you are going to be the Warrior! ");
			addText("Please continue working hard...");
			addText("I'll enhance your abilities a bit with the hope of you training yourself to be even stronger than you're now. ");
			addText("Haaaaaap!!");
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
				giveSp((getLevel() - 10) * 3 + 1);
			end

			setJob(100);
			giveItem(1302077, 1);
			hpInc = 200 + getRandomNumber(50);
			setMaxHp(getRealMaxHp() + hpInc);
			setStr(35);
			setDex(4);
			setInt(4);
			setLuk(4);
			setAp((getLevel() - 1) * 5 - (22));
			addSlots(1, 1);
			addSlots(2, 1);
			addSlots(3, 1);
			addSlots(4, 1);

			addText("You've gotten much stronger now. ");
			addText("Plus every single one of your inventories have added slots. ");
			addText("A whole row, to be exact. ");
			addText("Go see for it yourself. ");
			addText("I just gave you a little bit of " .. blue("SP") .. ". ");
			addText("When you open up the " .. blue("Skill menu") .. " on the lower left corner of the screen, there are skills you can learn by using SP's. ");
			addText("One warning, though: You can't raise it all together all at once. ");
			addText("There are also skills you can accquire only after having learned a couple of skills first.");
			sendNext();

			addText("One more warning. ");
			addText("Once you have chosen your job, try to stay alive as much as you can. ");
			addText("Once you reach that level, when you die, you will lose your experience level. ");
			addText("You wouldn't want to lose your hard-earned experience points, do you? ");
			addText("This is all i can teach you...from here on out, it's all about pushing yourself harder and become better. ");
			addText("See me after you feel that you have gotten much more powerful than you are right now.");
			sendBackNext();

			addText("Oh, and... if you have questions about being a Warrior, feel free to ask. ");
			addText("I don't know EVERYTHING, but I'll help you out with all that I know of. ");
			addText("Til then...");
			sendBackNext();
		else
			addText("Really? ");
			addText("Do you need more time to give more thought to it? ");
			addText("By all means... this is not something you should take lightly. ");
			addText("Come talk to me once your have made your decision.");
			sendNext();
		end
	else
		addText("You need more training to be a Warrior. ");
		addText("In order to be one, you need to train yourself to be more powerful than you are right now. ");
		addText("Please come back much stronger.");
		sendBackNext();
	end
elseif getJobLine() == line_warrior and getJobTrack() > 0 then
	choices = {
		makeChoiceHandler("What are the general characteristics of being a Warrior?", function()
			addText("Let me explain the role of a Warrior. ");
			addText("Warriors possess awesome physical strength and power. ");
			addText("They can also defende monsters' attacks, so they are the best when fighting up close with the monsters. ");
			addText("With a high level of stamina, you won't be dying easily either.");
			sendNext();

			addText("To accurately attack the monster, however, you need a healthy dose of DEX, so don't just concentrate on boosting up the STR. ");
			addText("If you want to improve rapidly, I recommend that you face stronger monsters.");
			sendBackNext();
		end),
		makeChoiceHandler("What are the weapons that the Warriors use?", function()
			addText("Let me explain the weapons Warriors use. ");
			addText("They can use weapons that allow them to slash, stab or strike. ");
			addText("You won't be able to use weapons like bows and projectile weapons. ");
			addText("Same with the small canes. ");
			sendNext();

			addText("The most common weapons are sword, blunt weapon, polearm, speak, axe, and etc...");
			addText("Every weapon has its advantages and disadvantages, so please take a close look at them before choosing one. ");
			addText("For now, try using the ones with high attack rating.");
			sendBackNext();
		end),
		makeChoiceHandler("What are the armors that the Warriors can wear?", function()
			addText("Let me explain the armors Warriors wear. ");
			addText("Warriors are strong with high stamine, so they are able to wear tough, strong armor. ");
			addText("It's not the greatest looking ones...but it serves its purpose well, the best of the armors.");
			sendNext();

			addText("Especially the shields, they are perfect for the Warriors. ");
			addText("Remember, though, that you won't be able to use the shield if you are using the weapon that requires both hands. ");
			addText("I know it's going to be a hard decision for you...");
			sendBackNext();
		end),
		makeChoiceHandler("What are the skills available for the Warriors?", function()
			addText("For the Warriors, the skills available are geared towards their awesome physical strength and power. ");
			addText("The skill that helps you in close combats will help you the most. ");
			addText("There's also a skill that allows you to recover your HP. ");
			addText("Make sure to master that.");
			sendNext();

			addText("The two attacking skills available are " .. blue("Power Strike") .. " and " .. blue("Slash Blast") .. ". ");
			addText("Power Strike is the one that applies a lot of damage to a single enemy. ");
			addText("You can boost this skills up from the beginning.");
			sendBackNext();

			addText("On the other hand, Slash Blast does not apply much damage, but instead attacks multiple enemies around the area at once. ");
			addText("You can only use this once you have 1 Power Strike boosted up. ");
			addText("Its up to you.");
			sendBackNext();
		end),
	};
	addText("Oh, you have a question?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
elseif getJob() == 100 and getLevel() >= 30 then
	if getItemAmount(4031012) == 0 and getItemAmount(4031008) == 0 then
		addText("Whoa! ");
		addText("You have definitely grown up! ");
		addText("You don't look small and weak anymore...rather, now I can feel your presence as the Warrior! ");
		addText("Impressive...so, what do you think? ");
		addText("Do you want to get even stronger than you are right now? ");
		addText("Pass a simple test and I'll do just that! ");
		addText("Wanna do it?");
		answer = askYesNo();

		if answer == answer_no then
			addText("Really? ");
			addText("It will help you out a great deal on your journey if you get stronger fast...if you choose to change your mind in the future, please feel free to come back. ");
			addText("Know that I'll make you much more powerful than you are right now.");
			sendNext();
		else
			addText("Good thinking. ");
			addText("You look strong, don't get me wrong, but there's still a need to test your strength and see if you are for real. ");
			addText("The test isn't too difficult, so you'll do just fine...");
			addText("Here, take this letter first. ");
			addText("Make sure you don't lose it.");
			sendNext();

			giveItem(4031008, 1);

			addText("Please get this letter to " .. blue(npcRef(1072000)) .. " who may be around " .. blue(mapRef(102020300)) .. " that's near Perion. ");
			addText("He's the one being the instructor now in place of me, as I am busy here. ");
			addText("Get him the letter and he'll give the test in place of me. ");
			addText("For more details, hear it straight from him. ");
			addText("Best of luck to you.");
			sendBackNext();
		end
	elseif getItemAmount(4031012) == 0 and getItemAmount(4031008) > 0 then
		addText("Still haven't met the person yet? ");
		addText("Find " .. blue(npcRef(1072000)) .. " who's around " .. blue(mapRef(102020300)) .. " near Perion. ");
		addText("Give the letter to him and he may let you know what to do.");
		sendNext();
	elseif getItemAmount(4031012) > 0 and getItemAmount(4031008) == 0 then
		addText("OHH...you came back safe! ");
		addText("I knew you'd breeze through...");
		addText("I'll admit you are a strong, formidable warrior...");
		addText("alright, I'll make you an even strong Warrior than you already are right now...");
		addText("Before THAT! ");
		addText("you need to choose one of the three paths that you'll be given...it isn't going to be easy, so if you have any questions, feel free to ask.");
		sendNext();

		choices = {
			makeChoiceHandler("Please explain the role of the Fighter.", function()
				addText("Let me explain the role of the fighter. ");
				addText("It's the most common kind of Warriors. ");
				addText("The weapons they use are " .. blue("sword") .. " and " .. blue("axe") .. " , because there will be advanced skills available to acquired later on. ");
				addText("I strongly recommend you avoid using both weapons, but rather stick to the one of your liking...");
				sendNext();

				addText("Other than that, there are also skills such as " .. blue("Rage") .. " and " .. blue("Power Guard") .. " available for fighters. ");
				addText(blue("Rage") .. " is the kind of an ability that allows you and your party to temporarily enhance your weapon power. ");
				addText("With that you can take out the enemies with a sudden surge of power, so it'll come very handy for you. ");
				addText("The downside to this is that your guarding ability (defense) goes down a bit.");
				sendBackNext();

				addText(blue("Power Guard") .. " is an ability that allows you to return a portion of the damage that you take from a weapon hit by an enemy. ");
				addText("The harder the hit, the harder the damage they'll get in return. ");
				addText("It'll help those that prefer close combat. ");
				addText("What do you think? ");
				addText("Isn't being the Fighter pretty cool?");
				sendBackNext();
			end),
			makeChoiceHandler("Please explain the role of the Page.", function()
				addText("Let me explain the role of the Page. ");
				addText("Page is a knight-in-training, taking its first steps to becoming an actual knight. ");
				addText("They usually use " .. blue("swords") .. " and/or " .. blue("blunt weapons") .. ". ");
				addText("It's not wise to use both weapons so it'll be best for you to stick to one or the other.");
				sendNext();

				addText("Other than that, there are also skills such as " .. blue("Threaten") .. " and " .. blue("Power Guard") .. " to learn. ");
				addText(blue("Threaten") .. " makes every opponent around you lose some attacking and defending abilities for a time being. ");
				addText("It's very useful against powerful monsters with good attacking abilities. ");
				addText("It also works well in party play.");
				sendBackNext();

				addText(blue("Power Guard") .. " is an ability that allows you to return a portion of the damage that you take from a weapon hit by an enemy. ");
				addText("The harder the hit, the harder the damage they'll get in return. ");
				addText("It'll help those that prefer close combat. ");
				addText("What do you think? ");
				addText("Isn't being the Page pretty cool?");
				sendBackNext();
			end),
			makeChoiceHandler("Please explain the role of the Spearman.", function()
				addText("Let me explain the role of the Spearman. ");
				addText("It's a job that specializes in using long weapons such as " .. blue("spears") .. " and " .. blue("polearms") .. ". ");
				addText("There are lots of useful skills to acquire with both of the weapons, but I strongly recommend you stick to one and focus on it.");
				sendNext();

				addText("Other than that, there are also skills such as " .. blue("Iron Will") .. " and " .. blue("Hyper Body") .. " to learn. ");
				addText(blue("Iron Will") .. " allows you and the members of your party to increase attack and magic defense for a period of time. ");
				addText("It's the skill that's very useful for Spearmen with weapons that require both hands and can't guard themselves as well.");
				sendBackNext();

				addText(blue("Hyper Body") .. " is a skill that allows you and your party to temporarily improve the max HP and MP. ");
				addText("You can improve almost 160% so it'll help you and your party especially when going up against really tough opponents. ");
				addText("What do you think? ");
				addText("Don't you think being the Spearman can be pretty cool?");
				sendBackNext();
			end),
			makeChoiceHandler("I'll choose my occupation!", function()
				choices = {
					makeChoiceHandler("Fighter", function()
						addText("So you want to make the 2nd job advancement as the " .. blue("Fighter") .. "? ");
						addText("Once you make that decision you can't go back and choose another job...do you still wanna do it?");
						answer = askYesNo();

						if answer == answer_no then
							addText("Really? ");
							addText("So you need to think about it a little more? ");
							addText("Take your time...this is not something that you should take lightly...let me know when you have made your decision, okay?");
							sendNext();
						else
							if getSp() > (getLevel() - 30) * 3 then
								addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many ");
								addText("SP in store. Use more SP on the skills on the 1st level and then come back.");
								sendNext();
							else
								setJob(110);
								giveSp(1);
								giveItem(4031012, -1);
								addSlots(2, 1);
								addSlots(4, 1);
								newHp = 300 + getRandomNumber(50);
								setMaxHp(getRealMaxHp() + newHp);

								addText("Alright! ");
								addText("You have now become the " .. blue("Fighter") .. "! ");
								addText("A fighter strives to become the strongest of the strong, and never stops fighting. ");
								addText("Don't ever lose that will to fight, and push forward 24/7. ");
								addText("It'll help you become even stronger than you already are.");
								sendNext();

								addText("I have just given you a book that gives you the list of skills you can acquire as the Fighter. ");
								addText("In that book, you'll find a bunch of skills the Fighter can learn. ");
								addText("Your use and etc. inventories have also been expanded with an additional row of slots also available. ");
								addText("Your max MP has also been increased...go check and see for it yourself.");
								sendBackNext();

								addText("I have also given you a little bit of " .. blue("SP") .. ". ");
								addText("Open the " .. blue("Skill Menu") .. " located at the bottomleft corner. ");
								addText("You'll be able to boost up the newly-acquired 2nd level skills. ");
								addText("A word of warning though: You can't boost them up all at once. ");
								addText("Some of the skills are only available after you have learned other skills. ");
								addText("Make sure to remember that.");
								sendBackNext();

								addText("Fighters have to be strong. ");
								addText("But remember that you can't abuse that power and use it on a weakling. ");
								addText("Please use your enormous power the right way, because...for you to use that the right way,that is much harder than just getting stronger. ");
								addText("Find me after you have advanced much further.");
								sendBackNext();
							end
						end
					end),
					makeChoiceHandler("Page", function()
						addText("So you want to make the 2nd job advancement as the " .. blue("Page") .. "? ");
						addText("Once you make that decision you can't go back and choose another job...do you still wanna do it?");
						answer = askYesNo();

						if answer == answer_no then
							addText("Really? ");
							addText("So you need to think about it a little more? ");
							addText("Take your time...this is not something that you should take lightly...let me know when you have made your decision, okay?");
							sendNext();
						else
							if getSp() > (getLevel() - 30) * 3 then
								addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many SP in store. ");
								addText("Use more SP on the skills on the 1st level and then come back.");
								sendNext();
							else
								setJob(120);
								giveSp(1);
								giveItem(4031012, -1);
								addSlots(2, 1);
								addSlots(4, 1);
								newMp = 100 + getRandomNumber(50);
								setMaxMp(getRealMaxMp() + newMp);

								addText("Alright! ");
								addText("You have now become the " .. blue("Page") .. "! ");
								addText("The Pages have high intelligence and bravery for a Warrior...here's hoping that you'll take the right path with the right mindset...");
								addText("I'll help you become much stronger than you are right now.");
								sendNext();

								addText("I have just given you a book that gives you the list of skills you can acquire as the Page. ");
								addText("In that book, you'll find a bunch of skills the Fighter can learn. ");
								addText("Your use and etc. inventories have also been expanded with an additional row of slots also available. ");
								addText("Your max MP has also been increased...go check and see for it yourself.");
								sendBackNext();

								addText("I have also given you a little bit of " .. blue("SP") .. ". ");
								addText("Open the " .. blue("Skill Menu") .. " located at the bottomleft corner. ");
								addText("You'll be able to boost up the newly-acquired 2nd level skills. ");
								addText("A word of warning though: You can't boost them up all at once. ");
								addText("Some of the skills are only available after you have learned other skills. ");
								addText("Make sure to remember that.");
								sendBackNext();

								addText("Pages have to be strong. ");
								addText("But remember that you can't abuse that power and use it on a weakling. ");
								addText("Please use your enormous power the right way, because...for you to use that the right way, that is much harder than just getting stronger. ");
								addText("Find me after you have advanced much further.");
								sendBackNext();
							end
						end
					end),
					makeChoiceHandler("Spearman", function()
						addText("So you want to make the 2nd job advancement as the " .. blue("Spearman") .. "? ");
						addText("Once you make that decision you can't go back and choose another job...do you still wanna do it?");
						answer = askYesNo();

						if answer == answer_no then
							addText("Really? ");
							addText("So you need to think about it a little more? ");
							addText("Take your time...this is not something that you should take lightly...let me know when you have made your decision, okay?");
							sendNext();
						else
							if getSp() > (getLevel() - 30) * 3 then
								addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many SP in store. ");
								addText("Use more SP on the skills on the 1st level and then come back.");
								sendNext();
							else
								setJob(130);
								giveSp(1);
								giveItem(4031012, -1);
								addSlots(2, 1);
								addSlots(4, 1);
								newMp = 100 + getRandomNumber(50);
								setMaxMp(getRealMaxMp() + newMp);

								addText("Alright! ");
								addText("You have now become the " .. blue("Spearman") .. "! ");
								addText("The spearman use the power of darkness to take out the enemies, always in shadows...please believe in yourself and your awesome power as you go on in your journey...");
								addText("I'll help you become much stronger than you are right now.");
								sendNext();

								addText("I have just given you a book that gives you the list of skills you can acquire as the Spearman. ");
								addText("In that book, you'll find a bunch of skills the Fighter can learn. ");
								addText("Your use and etc. inventories have also been expanded with an additional row of slots also available. ");
								addText("Your max MP has also been increased...go check and see for it yourself.");
								sendBackNext();

								addText("I have also given you a little bit of " .. blue("SP") .. ". ");
								addText("Open the " .. blue("Skill Menu") .. " located at the bottomleft corner. ");
								addText("You'll be able to boost up the newly-acquired 2nd level skills. ");
								addText("A word of warning though: You can't boost them up all at once. ");
								addText("Some of the skills are only available after you have learned other skills. ");
								addText("Make sure to remember that.");
								sendBackNext();

								addText("Spearmen have to be strong. ");
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
	addText("Awesome body! ");
	addText("Awesome power! ");
	addText("Warriors are they way to go!!!! ");
	addText("What do you think? ");
	addText("Want to make the job advancement as a Warrior??");
	sendNext();
end