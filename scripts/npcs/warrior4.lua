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
-- Harmonia

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/jobHelper.lua");

jobLine, jobTrack, jobProgression = getJobMeta();

title = nil;
if jobTrack == 1 then
	title = "Hero";
elseif jobTrack == 2 then
	title = "Paladin";
else
	title = "Dark Knight"
end

if jobLine == line_warrior and jobProgression == progression_third then
	if getLevel() < 120 then
		addText("You're still weak to go to warrior extreme road. ");
		addText("If you get stronger, come back to me.");
		sendOk();
	elseif isQuestCompleted(6904) then
		choices = {
			makeChoiceHandler(" I want to advance to " .. title .. ".", function()
				if getSp() > ((getLevel() - 120) * 3) then
					addText("Hmm...");
					addText("You have too many " .. blue("SP") .. ". ");
					addText("You can't make the 4th job advancement with too many SP left.");
					sendOk();
				else
					if getOpenSlots(2) < 1 then
						addText("You can't proceed as you don't have an empty slot in your inventory. ");
						addText("Please clear your inventory and try again.");
						sendOk();
					else
						giveItem(2280003, 1);
						giveSp(3);
						giveAp(5);
						setJob(getJob() + 1);

						if jobTrack == 1 then
							setMaxSkillLevel(1120004, 10);
							setMaxSkillLevel(1121001, 10);
							setMaxSkillLevel(1121008, 10);
						elseif jobTrack == 2 then
							setMaxSkillLevel(1220005, 10);
							setMaxSkillLevel(1220001, 10);
							setMaxSkillLevel(1221009, 10);
						else
							setMaxSkillLevel(1320005, 10);
							setMaxSkillLevel(1320001, 10);
							setMaxSkillLevel(1321007, 10);
						end

						addText("You have become the best of warriors, my " .. blue(title) .. ".");
						addText("You will gain the " .. blue("Rush") .. " Skill which makes you attack mutiple enemies and give you indomitable will along with " .. blue("Stance") .. " and " .. blue("Achilles"));
						sendNext();

						if jobTrack == 1 then
							addText("This is not all about Hero. ");
							addText("Hero is a well-balanced warrior who has excellent attack and defense power. ");
							addText("It can learn various attack skills as well as combo attack if he trains himself.");
							sendNext();
						elseif jobTrack == 2 then
							addText("This is not all about Paladin. ");
							addText("Paladin is good at element-based attack and defense. ");
							addText("It can use a new element-based and may break the limit of charge blow if you train yourself.");
							sendNext();
						else
							addText("This is not all about Dark Knight. ");
							addText("Dark Knight can use the power of darkness. ");
							addText("It can attack with power of darkness which is unbelievably strong and may summon the figure of darkness.");
							sendNext();
						end

						addText("Don't forget that it all depends on how much you train.");
						sendOk();
					end
				end
			end),
			makeChoiceHandler("  Let me think for a while.", function()
				addText("You don't have to hesitate to be the best Warrior..");
				addText("Whenever you decide, talk to me. ");
				addText("If you're ready, I'll let you make the 4th job advancement.");
				sendOk();
			end),
		};

		addText("You're qualified to be a true warrior \r\n");
		addText("Do you want job advancement?\r\n");
		addText(blue(choiceRef(choices)));
		choice = askChoice();

		selectChoice(choices, choice);
	else
		addText("You're not ready to make 4th job advancement. ");
		addText("When you're ready, talk to me.");
		sendOk();
	end
elseif jobLine == line_warrior and jobProgression == progression_fourth then
	addText("You became the best warrior, the position of " .. blue(title) .. ". ");
	addText("Stronger power means more responsibility. ");
	addText("Hope you get over all the tests you will have in future.");
	sendOk();
else
	addText("Why do you want to see me? ");
	addText("There is nothing you want to ask me.");
	sendOk();
end