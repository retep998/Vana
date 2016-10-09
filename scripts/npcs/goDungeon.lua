--[[
Copyright (C) 2008-2016 Vana Development Team

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
-- Jeff - Ice Valley II

dofile("scripts/utils/npcHelper.lua");

if getItemAmount(4031450) > 0 then
	addText("I see you have a " .. blue(itemRef(4031450)) .. ". ");
	addText("This is great quality from Vogen. ");
	addText("Would you like to enter " .. mapRef(921100100) .. "?");
	answer = askYesNo();

	if answer == answer_yes then
		if not isInstance("iceDemon") then
			createInstance("iceDemon", 5 * 60, true);
			addInstancePlayer(getId());
			setMap(921100100);
		else
			addText("Someone is currently using the room. ");
			addText("Try again later.");
			sendNext();
		end
	else
		addText("Come back when you're ready.");
		sendNext();
	end
else
	addText("Hey, you look like you want to go farther and deeper past this place. ");
	addText("Over there, though, you'll find yourself surrounded by aggressive, dangerous monsters, so even if you feel that you're ready to go, please be careful. ");
	addText("Long ago, a few brave men from our town went in wanting to eliminate anyone threatening the town, but never came back out...");
	sendNext();

	addText("If you are thinking of going in, I suggest you change your mind. ");
	addText("But if you really want to go in...");
	addText("I'm only letting in the ones that are strong enough to stay alive in there. ");
	addText("I do not wish to see anyone else die. ");
	addText("Let's see ... Hmmm ...");

	if not isGm() and getLevel() < 50 then
		addText(" you haven't reached Level 50 yet. ");
		addText("I can't let you in, then, so forget it.");
		sendBackOk();
	else
		addText("! You look pretty strong. ");
		addText("All right, do you want to go in?");
		answer = askYesNo();

		if answer == answer_yes then
			setMap(211040300, "under00");
		else
			addText("Even if your level's high it's hard to actually go in there, but if you ever change your mind please find me. ");
			addText("After all, my job is to protect this place.");
			sendNext();
		end
	end
end