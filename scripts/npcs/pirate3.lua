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
-- Pedro (3rd Job - Pirate Instructor)

dofile("scripts/lua_functions/jobFunctions.lua");

zakum = getPlayerVariable("zakum_quest_status", true);

if getLevel() >= 50 then
	addText("Is there something that you want from me?\r\n");
	if getLevel() >= 70 then
		-- Third job
	end
	addText("#b#L1# I want your permission to attempt the Zakum Dungeon Quest!#l");
	choice = askChoice();

	if choice == 1 then
		if zakum ~= 1 then
			local job = getJobLine();
			if job == 0 or job == 5 then
				setPlayerVariable("zakum_quest_status", "1");
				addText("You want to be permitted to do the Zakum Dungeon Quest, right? Must be #b#p2030008##k ... ok, alright! I'm sure you'll be fine roaming around the dungeon. Don't get yourself killed ...");
			else
				addText("So you want me to give you my permission to go on the Zakum Dungeon Quest? But you don't seem to be a pirate under my jurisdiction, so please look for the Trainer that oversees your job.");
			end
		else
			addText("So how is the Zakum Dungeon Quest going? I hear there's an extremely scary monster in the deepest part of that dungeon... Well anyways, press on. I know you can handle it!");
		end
		sendNext();
	end
else
	addText("I don't think there's much help I could give you right now. Why don't you come back to see me after you've grown stronger? You can go attempt the Zakum Dungeon Quest once you reach #bLevel 50#k and make your 3rd job advancement when you reach #bLevel 70#k.");
	sendOK();
end
