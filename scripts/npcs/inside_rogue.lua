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
-- Thief Job Instructor (Inside)

dofile("scripts/utils/npcHelper.lua");

if getItemAmount(4031013) >= 30 then
	addText("Ohhhhh...you collected all 30 Dark Marbles!! ");
	addText("Wasn't it difficult?? ");
	addText("Alright. ");
	addText("You've passed the test and for that, I'll reward you " .. blue(itemRef(4031012)) .. ". ");
	addText("Take that item and go back to Kerning City.");
	sendNext();

	giveItem(4031013, -getItemAmount(4031013));
	giveItem(4031011, -getItemAmount(4031011));
	giveItem(4031012, 1);
	setMap(103000000);
elseif getItemAmount(4031013) < 30 then
	addText("What's going on? ");
	addText("Doesn't look like you have collected 30 " .. blue(itemRef(4031013)) .. ", yet...");
	addText("If you're having problems with it, then you can leave, come back and try it again. ");
	addText("So...wanna give up and get out of here?");
	answer = askYesNo();

	if answer == answer_no then
		addText("That's right! ");
		addText("Stop acting weak and start collecting the marbles. ");
		addText("Talk to me when you have collected 30 " .. blue(itemRef(4031013)) .. ".");
		sendNext();
	else
		addText("Really... alright, I'll let you out. ");
		addText("Please don't give up, though. ");
		addText("You can always try again, so do not give up. ");
		addText("Until then, bye...");
		sendNext();

		setMap(102040000);
	end
end