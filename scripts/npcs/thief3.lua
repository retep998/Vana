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
-- Arec (3rd Job - Thief Instructor)

dofile("scripts/lua_functions/jobFunctions.lua");

zakum = getPlayerVariable("zakum_quest_status", true);

if getLevel() >= 50 then
	addText("Can I help you?\r\n");
	if getLevel() >= 70 then
		-- Third job
	end
	addText("#b#L1# Please allow me to do the Zakum Dungeon Quest.#l");
	choice = askChoice();

	if choice == 1 then
		if zakum == nil then
			local job = getJobLine();
			if job == 0 or job == 4 then
				setPlayerVariable("zakum_quest_status", "1");
				addText("You want to be permitted to do the Zakum Dungeon Quest, right? Must be #b#p2030008##k ... ok, alright! I'm sure you'll be fine roaming around the dungeon. Here's hoping you'll be careful there ...");
			else
				addText("So you want me to give you my permission to go on the Zakum Dungeon Quest? But you don't seem to be a thief under my jurisdiction, so please look for the Trainer that oversees your job.");
			end
		else
			addText("How are you along with the Zakum Dungeon Quest? From what I've heard, there's this incredible monster at the innermost part of that place ... anyway, good luck. I'm sure you can do it.");
		end
		sendNext();
	end
else
	addText("Hmm... It seems like there is nothing I can help you with. Come to me again when you get stronger. Zakum Dungeon quest is available from #bLevel 50#k and 3rd Job Advancement at #bLevel 70#k");
	sendOK();
end
