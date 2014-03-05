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
-- Konpei (warps to Bodyguard room)

dofile("scripts/lua_functions/npcHelper.lua");

if getItemAmount(4000138) == 0 then
	addText("So you've made it here. ");
	addText("Not bad. ");
	addText("You'll be taking on the boss now! ");
	addText("I'm concerned as to whether you take on the mighty boss with your abilities ... don't get me wrong, our Boss couldn't handle her either. ");
	addText("If you, by any chance, take down the boss and bring back her comb with you, then I'll take you to the next stage. ");
	sendOk();
else
	addText("Hey hey! ");
	addText("That item you have there...isn't that our boss's comb!? ");
	addText("Holy cow! ");
	addText("I knew it! ");
	addText("As soon as I saw you, I knew you would be the one defiant enough to take on the Boss. ");
	addText("Are you sure? ");
	addText("He's not going to give up without a fight--the evil spirit within him will ensure that. ");
	addText("Do you want to take on the Boss?");
	answer = askYesNo();

	if answer == answer_no then
		addText("Really? ");
		addText("Then let me know if you ever change your mind. ");
		sendNext();
	else
		addText("Are you sure you're going to enter this room? ");
		addText("Just remember, you can't stay here forever, and if you place our boss's comb on top of the treasure chest, the thugs will pounce on you, so be careful! ");
		answer = askYesNo();

		if answer == answer_no then
			addText("Really? ");
			addText("Then let me know if you ever change your mind. ");
			sendNext();
		else
			setMap(801040100);
		end
	end
end