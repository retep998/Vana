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
-- Gritto

dofile("scripts/lua_functions/jobFunctions.lua");

jl = getJobLine();
jt = getJobTrack();
jp = getJobProgression();

title = "";
if jt == 3 then
	title = "Bishop";
else
	title = "Arch Mage";
end

if jl == 2 and jp == 1 then
	if getLevel() < 120 then
		addText("You're still weak to go to magician extreme road. If you get stronger, come back to me.");
		sendOK();
	elseif isQuestCompleted(6914) then
		addText("You're qualified to be a true magician. \r\nDo you want job advancement?\r\n");
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
						setMaxSkillLevel(2121001, 10);
						setMaxSkillLevel(2121002, 10);
						setMaxSkillLevel(2121006, 10);
					elseif jt == 2 then
						setMaxSkillLevel(2221001, 10);
						setMaxSkillLevel(2221002, 10);
						setMaxSkillLevel(2221006, 10);
					else
						setMaxSkillLevel(2321001, 10);
						setMaxSkillLevel(2321002, 10);
						setMaxSkillLevel(2321005, 10);
					end

					addText("You became the best magician, #b" .. title .. "#k. " .. title .. " can use its own power as well as Mana of nature just like \n#bInfinity#k or #bBig Bang#k");
					sendNext();

					if jt == 3 then
						addText("This is not all about Bishop. Bishop can borrow God's power. It may make strong castle element-based magic and even make the dead alive.");
						sendNext();
					else
						addText("This is not all about Arch Mage. Arch Mage is good at fire and poison element-based. It may change not only extreme element-based but also element-based of its own or enemies if you train.");
						sendNext();
					end

					addText("Don't forget that it all depends on how much you train.");
					sendOK();
				end
			end
		elseif choice == 1 then
			addText("You don't have to hesitate to be the best Magician..Whenever you decide, talk to me. If you're ready, I'll let you make the 4th job advancement.");
			sendOK();
		end
	else
		addText("You're not ready to make 4th job advancement. When you're ready, talk to me.");
		sendOK();
	end
elseif jl == 2 and jp == 2 then
	addText("You became the best magician, the position of #b" .. title .. "#k. Stronger power means more responsibility. Hope you get over all the tests you will have in future.");
	sendOK();
else
	addText("Why do you want to see me? There is nothing you want to ask me.");
	sendOK();
end