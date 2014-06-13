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
-- Gritto

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/jobFunctions.lua");

jobLine, jobTrack, jobProgression = getJobMeta();

title = "";
if jobTrack == 3 then
	title = "Bishop";
else
	title = "Arch Mage";
end

if jobLine == line_magician and jobProgression == progression_third then
	if getLevel() < 120 then
		addText("You're still weak to go to magician extreme road. If you get stronger, come back to me.");
		sendOk();
	elseif isQuestCompleted(6914) then
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
							setMaxSkillLevel(2121001, 10);
							setMaxSkillLevel(2121002, 10);
							setMaxSkillLevel(2121006, 10);
						elseif jobTrack == 2 then
							setMaxSkillLevel(2221001, 10);
							setMaxSkillLevel(2221002, 10);
							setMaxSkillLevel(2221006, 10);
						else
							setMaxSkillLevel(2321001, 10);
							setMaxSkillLevel(2321002, 10);
							setMaxSkillLevel(2321005, 10);
						end

						addText("You became the best magician, " .. blue(title) .. ". ");
						addText(title .. " can use its own power as well as Mana of nature just like \n" .. blue("Infinity") .. " or " .. blue("Big Bang"));
						sendNext();

						if jobTrack == 1 then
							addText("This is not all about Arch Mage. ");
							addText("Arch Mage is good at fire and poison element-based. ");
							addText("It may change not only extreme element-based but also element-based of its own or enemies if you train.");
							sendNext();
						elseif jobTrack == 2 then
							addText("This is not all about Arch Mage. ");
							addText("Arch Mage is good at fire and poison element-based. ");
							addText("It may change not only extreme element-based but also element-based of its own or enemies if you train.");
							sendNext();
						elseif jobTrack == 3 then
							addText("This is not all about Bishop. ");
							addText("Bishop can borrow God's power. ");
							addText("It may make strong castle element-based magic and even make the dead alive.");
							sendNext();
						end

						addText("Don't forget that it all depends on how much you train.");
						sendOk();
					end
				end
			end),
			makeChoiceHandler("  Let me think for a while.", function()
				addText("You don't have to hesitate to be the best Magician..");
				addText("Whenever you decide, talk to me. ");
				addText("If you're ready, I'll let you make the 4th job advancement.");
				sendOk();
			end),
		};

		addText("You're qualified to be a true magician. \r\n");
		addText("Do you want job advancement?\r\n");
		addText(blue(choiceRef(choices)));
		choice = askChoice();

		selectChoice(choices, choice);
	else
		addText("You're not ready to make 4th job advancement. ");
		addText("When you're ready, talk to me.");
		sendOk();
	end
elseif jobLine == line_magician and jobProgression == progression_fourth then
	addText("You became the best magician, the position of " .. blue(title) .. ". ");
	addText("Stronger power means more responsibility. ");
	addText("Hope you get over all the tests you will have in future.");
	sendOk();
else
	addText("Why do you want to see me? ");
	addText("There is nothing you want to ask me.");
	sendOk();
end