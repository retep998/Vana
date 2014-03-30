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
-- Adobis that runs the quests

dofile("scripts/lua_functions/npcHelper.lua");
dofile("scripts/lua_functions/miscFunctions.lua");

choices = {
	makeChoiceHandler(" Explore the Dead Mine. (Level 1)", function()

	end),
	makeChoiceHandler(" Observe the Zakum Dungeon. (Level 2)", function()

	end),
	makeChoiceHandler(" Request for a refinery. (Level 3)", function()

	end),
	makeChoiceHandler(" Get briefed for the quest.", function()

	end),
};

if getItemAmount(4001109) > 0 then
	append(choices, makeChoiceHandler(" Enter the center of Lava.", function()
		if not isInstance("fireDemon") then
			createInstance("fireDemon", 5 * 60, true);
			addInstancePlayer(getId());
			setMap(921100000);
		else
			addText("Someone is currently using the room. ");
			addText("Try again later.");
			sendNext();
		end
	end));
end

addText("Well ... alright. ");
addText("You seem more than qualified for this. ");
addText("Which of these tasks do want to tackle on?\r\n");
addText(blue(choiceList(choices)));
choice = askChoice();

selectChoice(choices, choice);