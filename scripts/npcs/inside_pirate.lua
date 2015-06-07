--[[
Copyright (C) 2008-2015 Vana Development Team

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
-- Kyrin - Pirate Job Instructor

dofile("scripts/utils/npcHelper.lua");

m = getMap();
if m ~= 108000500 and m ~= 108000502 then
	consoleOutput("Unsupported inside_pirate map: " .. m);
	return;
else
	item = nil;
	if m == 108000500 then
		item = 4031857;
	else
		item = 4031856;
	end

	if getItemAmount(item) >= 15 then
		addText("Ohhh... So you managed to gather up 15 " .. itemRef(item) .. "s! ");
		addText("Wasn't it tough? ");
		addText("That's amazing... alright then, now let's talk about The Nautilus.");
		sendNext();

		addText("These crystals can only be used here, so I'll just take them back.");
		sendBackNext();

		setMap(120000101);
	else
		addText("Hmmm... ");
		addText("What is it? ");
		addText("I don't think you have been able to gather up all " .. blue("15 " .. itemRef(item) .. "s") .. " yet... ");
		addText("If it's too hard for you, then you can step out and try again later. ");
		addText("Do you want to give up and step outside right now?");
		answer = askYesNo();

		if answer == answer_yes then
			addText("Really? ");
			addText("Ok, I'll take you outside right now. ");
			addText("Please don't give up, though. ");
			addText("You'll get the opportunity to try this again. ");
			addText("Hopefully by then, you'll be ready to handle this with ease...");
			sendNext();

			setMap(120000101);
		else
			addText("Good. ");
			addText("You're showing me you don't want to give up this great opportunity. ");
			addText("When you collect " .. blue("15 " .. itemRef(item) .. "s") ..", then talk to me.");
			sendNext();
		end
	end
end