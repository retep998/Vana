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
-- Tylus (3rd Job - Warrior Instructor)

dofile("scripts/lua_functions/npcHelper.lua");
dofile("scripts/lua_functions/jobFunctions.lua");
dofile("scripts/lua_functions/miscFunctions.lua");

zakum = getPlayerVariable("zakum_quest_status", true);

if getLevel() >= 50 then
	job = getJobLine();
	jobLevel = getJobProgression();
	jobType = getJobTrack();
	choices = {};

	if getLevel() >= 70 and jobLevel == 0 and job == 1 then
		append(choices, makeChoiceHandler(" I want to make the 3rd job advancement", function()
			questState = getPlayerVariable("third_job_advancement", true);

			if questState == nil then
				addText("Welcome. ");
				addText("I'm " .. blue("Tylus") .. " the chief of all warriors, in charge of bringing out the best in each and every warrior that needs my guidance. ");
				addText("You seem ready to take on the challenges of the 3rd job advancement, but I advise you not to get too confident. ");
				addText("I've seen too many warriors come and go, failing to live up to expectations. ");
				addText("As for you, I don't know... ");
				addText("Are you ready to be tested and make the 3rd job advancement?");
				answer = askYesNo();

				if answer == answer_no then
					addText("I don't think you are ready to face the tough challenges that lie ahead. ");
					addText("Come see me only after you have convinced yourself that you're ready to make the next leap forward.");
					sendNext();
				else
					addText("Good. You will be tested on two important aspects of the warrior: strength and wisdom. ");
					addText("I'll now explain to you the physical half of the test. ");
					addText("Remember " .. blue(npcRef(1022000)) .. " from Perion? ");
					addText("Go see him, and he'll give you the details on the first half of the test. ");
					addText("Please complete the mission, and get " .. blue(itemRef(4031057)) .. " from " .. npcRef(1022000) .. ".");
					sendNext();

					addText("The mental half of the test can only start after you pass the physical part. ");
					addText(blue(itemRef(4031057)) .. " will be the proof that you have indeed passed the test. ");
					addText("I'll let " .. blue(npcRef(1022000)) .. " know in advance that you're making your way there, so get ready. ");
					addText("It won't be easy, but I have faith in you. Good luck.");
					sendBackNext();

					setPlayerVariable("third_job_advancement", 1);
				end
			elseif questState == 1 or questState == 2 then
				if getItemAmount(4031057) > 0 then
					addText("Great job completing the physical part of the test. ");
					addText("I knew you could do it. ");
					addText("Now that you have passed the first half of the test, here's the second half. ");
					addText("Please give me the necklace first.");
					sendNext();

					giveItem(4031057, -1);
					setPlayerVariable("third_job_advancement", 3);

					addText("Here's the second half of the test. ");
					addText("This test will determine whether you are smart enough to take the next step towards greatness. ");
					addText("There is a dark, snow-covered area called the Holy Ground at the snowfield in Ossyria, where even the monsters can't reach. ");
					addText("In the center of the area lies a huge stone called the Holy Stone. ");
					addText("You'll need to offer a special item as a sacrifice. ");
					addText("Then the Holy Stone will test your wisdom right there on the spot.");
					sendBackNext();

					addText("You'll need to answer each and every question given to you with honesty and conviction. ");
					addText("If you correctly answer all the questions, then the Holy Stone will formally accept you and hand you " .. blue(itemRef(4031058)) .. ". ");
					addText("Bring back the necklace, and I will help you make the next leap forward. ");
					addText("Good luck.");
					sendBackNext();
				else
					addText("You don't have " .. blue(itemRef(4031057)) .. " with you. ");
					addText("Please go see " .. blue(npcRef(1022000)) .. " of Perion, pass the test, and bring " .. blue(itemRef(4031057)) .. " back with you. ");
					addText("Then, and only then, will you be given the second test. ");
					addText("Best of luck to you.");
					sendNext();
				end
			elseif questState == 3 or questState == 4 then
				if getItemAmount(4031058) > 0 or questState == 4 then
					if questState == 3 then
						addText("Great job completing the mental part of the test. ");
						addText("You have wisely and correctly answered all the questions. ");
						addText("I must say, I am quite impressed with the level of wisdom you displayed. ");
						addText("Please hand me the necklace first, before we take the next step.");
						sendNext();

						giveItem(4031058, -1);
						setPlayerVariable("third_job_advancement", 4);
					end

					addText("Okay! Now, I'll transform you into a much more powerful warrior. ");
					addText("Before that, however, please make sure your SP has been thoroughly used. ");
					addText("You'll need to use at least all the SP gained until Lv. 70 to make the 3rd job advancement. ");
					addText("Oh, and since you have already chosen your occupation path from the 2nd job advancement, you won't have to choose again for the 3rd job advancement. ");
					addText("So, do you want to do it right now?");
					answer = askYesNo();

					if answer == answer_no then
						addText("You've already passed the test and all, so don't hesitate. ");
						addText("Anyway, come talk to me once you have made up your mind. ");
						addText("As long as you're ready for it, I'll get you through the 3rd job advancement.");
						sendNext();
					else
						if getSp() > (getLevel() - 70) * 3 then
							addText("Hmmm... you seem to have too much SP. ");
							addText("You can't make the 3rd job advancement with so much SP. ");
							addText("Use more in the 1st and 2nd advancement before returning here.");
							sendNext();
						else
							setPlayerVariable("third_job_advancement", 5);
							setJob(getJob() + 1);
							giveSp(1);
							giveAp(5);

							if jobType == 1 then
								addText("You have just become the " .. blue("Crusader") .. ". ");
								addText("You have a number of new devastating offensive skills such as " .. blue("Shout") .. ", " .. blue("Combo Attack") .. " and " .. blue("Armor Crush") .. ". ");
							elseif jobType == 2 then
								addText("You have just become the " .. blue("White Knight") .. ". ");
								addText("You'll be introduced to a new skill book featuring various new attacking skills as well as element-based attacks. ");
								addText("I suggest that you continue to use a weapon complementary to the Page, whether it be a sword or a blunt weapon. ");
								addText("There's a skill called " .. blue("Charge") .. ", which adds an element of fire, ice, or lightning to the weapon, making the White Knight the only warrior that can perform element-based attacks. ");
								addText("Charge your weapon with an element that weakens the monster, and then apply massive damage with the " .. blue("Charged Blow") .. ". ");
								addText("You would definitely be a devastating force.");
							elseif jobType == 3 then
								addText("You're a " .. blue("Dragon Knight") .. " from here on out. ");
								addText("You'll be introduced to a row of new offensive skills for spears and polearms. ");
								addText("Skills such as " .. blue("Dragon Buster") .. " (heavy damage to one monster) and " .. blue("Dragon Fury") .. " (damage to multiple monsters) are recommended as main attacking skills, while a skill called " .. blue("Dragon Roar") .. " will damage everything on screen with devastating force. ");
								addText("The downside is that the skill consumes a considerate amount of HP.");
							end
							sendNext();

							addText("I've also given you a little bit of SP and AP, which will be beneficial to you. ");
							addText("You have now become a powerful warrior, indeed. ");
							addText("Remember, though, that the real world will be awaiting with even tougher obstacles. ");
							addText("Once you feel like you cannot reach a higher place, then come see me. ");
							addText("I'll be here waiting.");
							sendBackNext();
						end
					end
				else
					addText("You don't have " .. blue(itemRef(4031058)) .. " with you. ");
					addText("Please find the dark, snow-covered area called the Holy Ground at the snowfield in Ossyria, offer the special item as a sacrifice, and answer each and every question asked with honesty and conviction to receive " .. blue(itemRef(4031058)) .. ". ");
					addText("Bring that back to me to complete the 3rd job advancement test. ");
					addText("Best of luck to you.");
					sendOk();
				end
			end
		end));
	end

	append(choices, makeChoiceHandler(" Please allow me to do the Zakum Dungeon Quest", function()
		if zakum == nil then
			if job == 0 or job == 1 then
				setPlayerVariable("zakum_quest_status", "1");
				addText("You want to be permitted to do the Zakum Dungeon Quest, right? ");
				addText("Must be " .. blue(npcRef(2030008)) .. " ... ok, alright! ");
				addText("I'm sure you'll be fine roaming around the dungeon. ");
				addText("Here's hoping you'll be careful there ...");
			else
				addText("So you want me to give you my permission to go on the Zakum Dungeon Quest? ");
				addText("But you don't seem to be a warrior under my jurisdiction, so please look for the Trainer that oversees your job.");
			end
		else
			addText("How are you along with the Zakum Dungeon Quest? ");
			addText("From what I've heard, there's this incredible monster at the innermost part of that place ... anyway, good luck. ");
			addText("I'm sure you can do it.");
		end
		sendNext();
	end));

	addText("Can I help you?\r\n");
	addText(blue(choiceList(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
else
	addText("Hmm... It seems like there is nothing I can help you with. ");
	addText("Come to me again when you get stronger. ");
	addText("Zakum Dungeon quest is available from " .. blue("Level 50") .. " and 3rd Job Advancement at " .. blue("Level 70"));
	sendOk();
end