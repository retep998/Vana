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
-- Holy Stone for third job advancements

dofile("scripts/lua_functions/miscFunctions.lua");

if questions == nil then
	questions = {
		[1] = {
			["question"] = "Which of these NPCs will you see FIRST at MapleStory...?#b\r\n#L0# #p2000##l\r\n#L1# #p1010100##l\r\n#L2# #p2102##l\r\n#L3# #p2001##l\r\n#L4# #p2101##l",
			["answer"] = 0
		},
		[2] = {
			["question"] = "Which of these EXP points is required to go from Lv. 1 to 2...?#b\r\n#L0# 10#l\r\n#L1# 15#l\r\n#L2# 20#l\r\n#L3# 25#l\r\n#L4# 30#l",
			["answer"] = 1
		},
		[3] = {
			["question"] = "Which of these NPCs will you NOT see at El Nath of Ossyria...?#b\r\n#L0# #p2020000##l\r\n#L1# #p2020003##l\r\n#L2# #p2012010##l\r\n#L3# #p2020006##l\r\n#L4# #p2020007##l",
			["answer"] = 2
		},
		[4] = {
			["question"] = "Which of these jobs will you NOT get after the 2nd job advancement...?#b\r\n#L0# Page#l\r\n#L1# Assassin#l\r\n#L2# Bandit#l\r\n#L3# Cleric#l\r\n#L4#Buccaneer#l\r\n",
			["answer"] = 4
		},
		[5] = {
			["question"] = "Which of these quests can be repeated...?#b\r\n#L0# Maya and the Weird Medicine#l\r\n#L1# Alex the Runaway Kid#l\r\n#L2# Pia and the Blue Mushroom#l\r\n#L3# Arwen and the Glass Shoes#l\r\n#L4# Alpha Platoon's Network of Communication#l\r\n",
			["answer"] = 3
		}
	};
end

if questionRecord == nil then
	questionRecord = {};
end

function question(id)
	local questionId = 0;
	repeat
		questionId = getRandomNumber(#questions + 1);
	until questionRecord[questionId] == nil

	local q = questions[questionId];
	addText("Here's the " .. id .. getOrdinal(id) .. " question. ");
	addText(q["question"]);
	local choice = askChoice();

	if choice == q["answer"] then
		questionRecord[questionId] = true
		return true;
	end
	return false;
end

questState = getPlayerVariable("third_job_advancement", true);
if questState == nil or questState ~= 3 then
	-- TODO: Text?
else
	addText("... ... ...\r\n");
	addText("If you want to test out your wisdom, then you'll have to offer a #b#t4005004##k as a sacrifice ...\r\n");
	addText("Are you ready to offer a #t4005004# and answer a set of questions from me?");
	local ready = askYesNo();

	if ready == 0 then
		addText("Come back when you're ready.");
		sendNext();
	else
		if getItemAmount(4031058) > 0 then
			addText("You already have #b#t4031058##k with you...\r\n");
			addText("Take the necklace and go back...");
			sendNext();
		else
			if getItemAmount(4005004) == 0 then
				addText("If you want me to test your wisdom, you will have to provide a #b#t4005004##k as a sacrifice.");
				sendNext();
			else
				giveItem(4005004, -1);

				addText("Alright ... I'll be testing your wisdom here. ");
				addText("Answer all the questions correctly, and you will pass the test. ");
				addText("But, if you are wrong just once, then you'll have to start over again... ");
				addText("Okay, here we go.");
				sendNext();

				for i = 1, 5 do
					if not question(i) then
						addText("Wrong... Start again...");
						sendNext();
						return;
					end
				end

				addText("Alright. All your answers have been proven as the truth...\r\n");
				addText("Your wisdom has been proven.\r\n");
				addText("Take this necklace and go back...");
				sendNext();

				giveItem(4031058, 1);
			end
		end
	end
end