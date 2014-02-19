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
-- Rene (3rd Job - Bowman Instructor)

dofile("scripts/lua_functions/npcHelper.lua");
dofile("scripts/lua_functions/jobFunctions.lua");
dofile("scripts/lua_functions/miscFunctions.lua");

zakum = getPlayerVariable("zakum_quest_status", true);

if getLevel() >= 50 then
	job = getJobLine();
	jobLevel = getJobProgression();
	jobType = getJobTrack();
	choices = {};

	if getLevel() >= 70 and jobLevel == 0 and job == 3 then
		-- TODO FIXME: Implement
	end

	append(choices, makeChoiceHandler(" Please allow me to do the Zakum Dungeon Quest", function()
		if zakum == nil then
			if job == 0 or job == 1 then
				setPlayerVariable("zakum_quest_status", "1");
				addText("You want to be permitted to do the Zakum Dungeon Quest, right? ");
				addText("Must be " .. blue(npcRef(2030008)) .. " ... ok, alright! ");
				addText("I'm sure you'll be fine roaming around the dungeon. ");
				addText("Here's hoping you'll be careful there ...");
			else
				addText("So you want me to give you my permission to go on the Zakum Dungeon Quest? ");
				addText("But you don't seem to be a warrior under my jurisdiction, so please look for the Trainer that oversees your job.");
			end
		else
			addText("How are you along with the Zakum Dungeon Quest? ");
			addText("From what I've heard, there's this incredible monster at the innermost part of that place ... anyway, good luck. ");
			addText("I'm sure you can do it.");
		end
		sendNext();
	end));
	
	addText("Can I help you?\r\n");
	addText(blue(choiceList(choices)));
	choice = askChoice();

	selectChoice(choices, choice);
else
	addText("Hmm... It seems like there is nothing I can help you with. ");
	addText("Come to me again when you get stronger. ");
	addText("Zakum Dungeon quest is available from " .. blue("Level 50") .. " and 3rd Job Advancement at " .. blue("Level 70"));
	sendOk();
end