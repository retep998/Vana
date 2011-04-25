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
-- Hellin

dofile("scripts/lua_functions/jobFunctions.lua");

jl = getJobLine();
jt = getJobTrack();
jp = getJobProgression();

title = "";
if jt == 1 then
	title = "Night Lord";
elseif jt == 2 then
	title = "Shadower";
end

if jl == 4 and jp == 1 then
	if getLevel() < 120 then
		addText("You're still weak to go to thief extreme road. If you get stronger, come back to me.");
		sendOK();
	elseif isQuestCompleted(6934) then
		addText("You're qualified to be a true thief. \r\nDo you want job advancement?\r\n");
		addText("#b#L0# I want to advance to " .. title .. ".#l\r\n");
		addText("#b#L1#  Let me think for a while.#l");
		choice = askChoice();

		if choice == 0 then
			if getSP() > ((getLevel() - 120) * 3) then
				addText("Hmm...You have too many #bSP#k. You can't make the 4th job advancement with too many SP left.");
				sendOK();
			else
				if getOpenSlots(2) < 1 then
					addText("You can't proceed as you don't have an empty slot in your inventory. Please clear your inventory and try again.");
					sendOK();
				else
					giveItem(2280003, 1);
					giveSP(3);
					giveAP(5);
					setJob(getJob() + 1);

					if jt == 1 then
						setMaxSkillLevel(4121006, 10);
						setMaxSkillLevel(4120002, 10);
						setMaxSkillLevel(4120005, 10);
					elseif jt == 2 then
						setMaxSkillLevel(4221007, 10);
						setMaxSkillLevel(4220002, 10);
						setMaxSkillLevel(4220005, 10);
					end

					addText("You became the best thief #b" .. title .. "#k. " .. title .. " is good at using #bFake#k to avoid enemy's attack and #bNinja Ambush#k to call hidden colleagues. It attacks the blind side of enemy.");
					sendNext();

					if jt == 1 then
						addText("This is not all about Night Lord. Night Lord is good at fast war. It can throw many stars at one time and may beat off plenty of enemies at once.");
						sendNext();
					elseif jt == 2 then
						addText("This is not all about Shadower. Shadower is good at sudden attack. It can attack enemies before they notice and even beat them locked in the darkness.");
						sendNext();
					end

					addText("Don't forget that it all depends on how much you train.");
					sendOK();
				end
			end
		elseif choice == 1 then
			addText("You don't have to hesitate.... Whenever you decide, talk to me. If you're ready, I'll let you make the 4th job advancement.");
			sendOK();
		end
	else
		addText("You're not ready to make 4th job advancement. When you're ready, talk to me.");
		sendOK();
	end
elseif jl == 4 and jp == 2 then
	addText("You became the best thief, the position of #b" .. title .. "#k. Stronger power means more responsibility. Hope you get over all the tests you will have in future.");
	sendOK();
else
	addText("Why do you want to see me? There is nothing you want to ask me.");
	sendOK();
end