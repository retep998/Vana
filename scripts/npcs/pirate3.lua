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
-- Pedro (3rd Job - Pirate Instructor)

dofile("scripts/utils/jobFunctions.lua");
dofile("scripts/utils/npcHelper.lua");

zakum = getPlayerVariable("zakum_quest_status", type_int);

if getLevel() >= 50 then
	jobLine, jobTrack, jobProgression = getJobMeta();
	choices = {};

	if getLevel() >= 70 and jobProgression == progression_second and jobLine == line_pirate then
		append(choices, makeChoiceHandler(" I want to make the 3rd job advancement", function()
			questState = getPlayerVariable("third_job_advancement", type_int);

			if questState == nil then
				addText("Hmm... so you want to be a stronger pirate by making the 3rd job advancement? ");
				addText("First I can say congrats--few have this level of dedication. ");
				addText("I can certainly make you stronger with my powers, but I'll need to test your strength to see if your training has been adequate. ");
				addText("Many come professing their strength, few are actually able to prove it. ");
				addText("Are you ready for me to test your strength?");
				answer = askYesNo();

				if answer == answer_no then
					addText("I don't think you are ready to face the tough challenges that lie ahead. ");
					addText("Come see me only after you have convinced yourself that you're ready to make the next leap forward.");
					sendNext();
				else
					addText("Great! ");
					addText("Now, you will have to prove your strength and intelligence. ");
					addText("Let me first explain the strength test. ");
					addText("Do you remember " .. blue(npcRef(1090000)) .. " from The Nautilus who helped you make the 1st and 2nd advancements? ");
					addText("Go see her and she will give you a task to fulfill. ");
					addText("Complete that task and you will receive " .. blue(itemRef(4031057)) .. " from " .. npcRef(1090000) .. ".");
					sendNext();

					addText("The second part will be to test your intelligence, however, you must pass the strength test first. ");
					addText("After that, you will be qualified for the second test. ");
					addText("Now... I will let " .. blue(npcRef(1090000)) .. " know that you are on your way, so follow this road and go see her right away. ");
					addText("It won't be easy, but I trust that you will be able to handle it.");
					sendBackNext();

					setPlayerVariable("third_job_advancement", 1);
				end
			elseif questState == 1 or questState == 2 then
				if getItemAmount(4031057) > 0 then
					addText("So you were able to complete the task given to you by " .. blue(npcRef(1090000)) .. ". ");
					addText("Nice! ");
					addText("I knew you would be able to handle it. ");
					addText("But it's not over yet. ");
					addText("You do remember that there's a second part of the test, right? ");
					addText("Before we move on, why don't I take that necklace from you now.");
					sendNext();

					giveItem(4031057, -1);
					setPlayerVariable("third_job_advancement", 3);

					addText("Now, the second test remains. ");
					addText("If you pass this test, you'll become a more powerful Pirate. ");
					addText("When you take the secret portal found in Sharp Cliff I in the El Nath Dungeon, you will find a small Holy Stone, which the monsters don't dare approach. ");
					addText("It may appear to be an average Holy Stone, but if you offer a special item, it will test your wisdom.");
					sendNext();

					addText("Find the Holy Ground and offer a special item. ");
					addText("If you are able to answer the questions that are asked of you truthfully and sincerely, you will receive " .. blue(itemRef(4031058)) .. ". ");
					addText("If you bring the necklace to me, I will advance you into an even stronger pirate. ");
					addText("Best of luck to you and dress warm! ");
					addText("Pirate gear isn't exactly built for the cold.");
					sendBackNext();
				else
					addText("You don't have " .. blue(itemRef(4031057)) .. " with you. ");
					addText("Please go see " .. blue(npcRef(1090000)) .. " of The Nautilus, pass the test, and bring " .. blue(itemRef(4031057)) .. " back with you. ");
					addText("Then, and only then, will you be given the second test. ");
					addText("Best of luck to you.");
					sendNext();
				end
			elseif questState == 3 or questState == 4 then
				if getItemAmount(4031058) > 0 or questState == 4 then
					if questState == 3 then
						addText("Ah... You found the Holy Ground, I see. ");
						addText("I... I didn't think you would pass the second test. ");
						addText("I will take the necklace now before your advance to your 3rd job.");
						sendNext();

						giveItem(4031058, -1);
						setPlayerVariable("third_job_advancement", 4);
					end

					addText("Great! ");
					addText("You may now become the kind of Pirate you've always dreamed of! ");
					addText("With newfound power and stellar new skills, your power has endless possibilities! ");
					addText("Before we proceed, however, please check and see if you have used up your Skill Points. ");
					addText("You must use up all the SPs you've earned up to Level 70 in order for you to make the 3rd job advancement. ");
					addText("Since you've already chosen which of the two Pirate paths you wanted to take in the 2nd job advancement, this will not require much thought. ");
					addText("Do you wish to make the job advancement right now?");
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

							if jobTrack == 1 then
								addText("Great! ");
								addText("You are now a " .. blue("Marauder") .. ". ");
								addText("As a Marauder, you will learn some of the most sophisticated skills related to melee-based attacks. ");
								addText(blue("Energy Charge") .. " is a skill that allows you to store your power and the damage you receive into a special form of energy. ");
								addText("Once this ball of energy is charged, you may use " .. blue("Energy Blast") .. " to apply maximum damage against your enermies, and also use " .. blue("Energy Drain") .. " to steal your enemy's HP to recover your own. ");
								addText(blue("Transformation") .. " will allow you to transform into a superhuman being with devastating melee attacks, and while transformed, you can use " .. blue("Shockwave") .. " to cause a mini-earthquake and inflict massive damage to your enemies.");
							elseif jobTrack == 2 then
								addText("Great! ");
								addText("You are now an " .. blue("Outlaw") .. ". ");
								addText("As an Outlaw, you will become a true pistolero, a master of every known Gun attack, as well as a few other skills to help you vanquish evil. ");
								addText(blue("Burst Fire") .. " is a more powerful version of Double Shot, shooting more bullets and causing more damage at the same time. ");
								addText("You also no have the ability to summon a loyal " .. blue("Octopus") .. " and the swooping " .. blue("Gaviota") .. " as your trusty allies, while attacking your enemies using " .. blue("Bullseye") .. ". ");
								addText("You can also use element-based attacks by using " .. blue("Flamethrower") .. " and " .. blue("Ice Splitter") .. ".");
							end
							sendNext();

							addText("I've also given you some SP and AP, which will help you get started. ");
							addText("You have now become a powerful pirate, indeed. ");
							addText("Remember, though, that the real world will be awaiting with even tougher obstacles. ");
							addText("Once you feel like you cannot reach a higher place, then come see me. ");
							addText("I'll be here waiting.");
							sendBackNext();
						end
					end
				else
					addText("You don't have " .. blue(itemRef(4031058)) .. " with you. ");
					addText("Please go see " .. blue("Holy Stone") .. " in the Holy Ground, pass the test and bring " .. blue(itemRef(4031058)) .. " back with you. ");
					addText("Then, and only then, I will help you make the next leap forward. ");
					addText("Best of luck to you.");
					sendOk();
				end
			end
		end));
	end

	append(choices, makeChoiceHandler(" I want your permission to attempt the Zakum Dungeon Quest!", function()
		if zakum == nil then
			if jobLine == line_beginner or jobLine == line_pirate then
				setPlayerVariable("zakum_quest_status", 1);
				addText("You want to be permitted to do the Zakum Dungeon Quest, right? ");
				addText("Must be " .. blue(npcRef(2030008)) .. " ... ok, alright! ");
				addText("I'm sure you'll be fine roaming around the dungeon. ");
				addText("Don't get yourself killed ...");
			else
				addText("So you want me to give you my permission to go on the Zakum Dungeon Quest? ");
				addText("But you don't seem to be a pirate under my jurisdiction, so please look for the Trainer that oversees your job.");
			end
		else
			addText("How are you along with the Zakum Dungeon Quest? ");
			addText("I hear there's an extremely scary monster in the deepest part of that dungeon... ");
			addText("Well anyways, press on. ");
			addText("I know you can handle it!");
		end
		sendNext();
	end));

	addText("Is there something that you want from me?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
else
	addText("I don't think there's much help I could give you right now. ");
	addText("Why don't you come back to see me after you've grown stronger? ");
	addText("You can go attempt the Zakum Dungeon Quest once you reach " .. blue("Level 50") .. " and make your 3rd job advancement when you reach " .. blue("Level 70") .. ".");
	sendOk();
end