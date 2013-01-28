--[[
Copyright (C) 2008-2013 Vana Development Team

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

dofile("scripts/lua_functions/jobFunctions.lua");

jl = getJobLine();
jt = getJobTrack();
jp = getJobProgression();

title = "";
if jt == 1 then
	title = "Buccaneer";
elseif jt == 2 then
	title = "Corsair";
end

if jl == 5 and jp == 1 then
	if getLevel() < 120 then
		addText("You're still weak to go to pirate extreme road. If you get stronger, come back to me.");
		sendOk();
	elseif isQuestCompleted(6944) then
		addText("You're qualified to be a true thief. \r\nDo you want job advancement?\r\n");
		addText("#b#L0# I want to advance to " .. title .. ".#l\r\n");
		addText("#b#L1#  Let me think for a while.#l");
		choice = askChoice();

		if choice == 0 then
			if getSp() > ((getLevel() - 120) * 3) then
				addText("Hmm...You have too many #bSP#k. You can't make the 4th job advancement with too many SP left.");
				sendOk();
			else
				if getOpenSlots(2) < 1 then
					addText("You can't proceed as you don't have an empty slot in your inventory. Please clear your inventory and try again.");
					sendOk();
				else
					giveItem(2280003, 1);
					giveSp(3);
					giveAp(5);
					setJob(getJob() + 1);

					if jt == 1 then
						setMaxSkillLevel(5121001, 10);
						setMaxSkillLevel(5121002, 10);
						setMaxSkillLevel(5121007, 10);
						setMaxSkillLevel(5121009, 10);
					elseif jt == 2 then
						setMaxSkillLevel(5220001, 10);
						setMaxSkillLevel(5220011, 10);
						setMaxSkillLevel(5220002, 10);
						setMaxSkillLevel(5221004, 10);
					end

					addText("You became the best pirate #b" .. title .. "#k. ");
					sendNext();

					if jt == 1 then
						addText("This is not all about Buccaneer. Buccaneers can ride #bbattleships#k and shoot cannonballs, which are much stronger than normal bullets. Corsairs can also use #bmin control to blind the enemy and lead their ship to victory.");
						sendNext();
					elseif jt == 2 then
						addText("This is not all about Corsair. Corsairs can ride #bbattleships#k and shoot cannonballs, which are much stronger than normal bullets. Corsairs can also use #bmin control to blind the enemy and lead their ship to victory.");
						sendNext();
					end

					addText("Don't forget that it all depends on how much you train.");
					sendOk();
				end
			end
		elseif choice == 1 then
			addText("You don't have to hesitate.... Whenever you decide, talk to me. If you're ready, I'll let you make the 4th job advancement.");
			sendOk();
		end
	else
		addText("You're not ready to make 4th job advancement. When you're ready, talk to me.");
		sendOk();
	end
elseif jl == 5 and jp == 2 then
	addText("You became the best pirate, the position of #b" .. title .. "#k. Stronger power means more responsibility. Hope you get over all the tests you will have in future.");
	sendOk();
else
	addText("Why do you want to see me? There is nothing you want to ask me.");
	sendOk();
end