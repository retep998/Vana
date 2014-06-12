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
-- Samuel

dofile("scripts/lua_functions/npcHelper.lua");
dofile("scripts/lua_functions/jobFunctions.lua");

jobLine, jobTrack, jobProgression = getJobMeta();

title = nil;
if jobTrack == 1 then
	title = "Buccaneer";
elseif jobTrack == 2 then
	title = "Corsair";
end

if jobLine == line_pirate and jobProgression == progression_third then
	if getLevel() < 120 then
		addText("You're still weak to go to pirate extreme road. ");
		addText("If you get stronger, come back to me.");
		sendOk();
	elseif isQuestCompleted(6944) then
		choices = {
			makeChoiceHandler(" I want to advance to " .. title .. ".", function()
				if getSp() > ((getLevel() - 120) * 3) then
					addText("Hmm...You have too many " .. blue("SP") .. ". ");
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
							setMaxSkillLevel(5121001, 10);
							setMaxSkillLevel(5121002, 10);
							setMaxSkillLevel(5121007, 10);
							setMaxSkillLevel(5121009, 10);
						elseif jobTrack == 2 then
							setMaxSkillLevel(5220001, 10);
							setMaxSkillLevel(5220011, 10);
							setMaxSkillLevel(5220002, 10);
							setMaxSkillLevel(5221004, 10);
						end

						addText("You became the best pirate " .. blue(title) .. ". ");
						sendNext();

						if jobTrack == 1 then
							addText("This is not all about Buccaneer. ");
							addText("Buccaneers can ride " .. blue("battleships") .. " and shoot cannonballs, which are much stronger than normal bullets. ");
							addText("Corsairs can also use " .. blue("min control") .. " to blind the enemy and lead their ship to victory.");
							sendNext();
						elseif jobTrack == 2 then
							addText("This is not all about Corsair. ");
							addText("Corsairs can ride " .. blue("battleships") .. " and shoot cannonballs, which are much stronger than normal bullets. ");
							addText("Corsairs can also use " .. blue("min control") .. " to blind the enemy and lead their ship to victory.");
							sendNext();
						end

						addText("Don't forget that it all depends on how much you train.");
						sendOk();
					end
				end
			end),
			makeChoiceHandler("  Let me think for a while.", function()
				addText("You don't have to hesitate.... ");
				addText("Whenever you decide, talk to me. ");
				addText("If you're ready, I'll let you make the 4th job advancement.");
				sendOk();
			end),
		};

		addText("You're qualified to be a true thief. \r\n");
		addText("Do you want job advancement?\r\n");
		addText(blue(choiceList(choices)));
		choice = askChoice();

		selectChoice(choices, choice);
	else
		addText("You're not ready to make 4th job advancement. ");
		addText("When you're ready, talk to me.");
		sendOk();
	end
elseif jobLine == line_pirate and jobProgression == progression_fourth then
	addText("You became the best pirate, the position of " .. blue(title) .. ". ");
	addText("Stronger power means more responsibility. ");
	addText("Hope you get over all the tests you will have in future.");
	sendOk();
else
	addText("Why do you want to see me? ");
	addText("There is nothing you want to ask me.");
	sendOk();
end