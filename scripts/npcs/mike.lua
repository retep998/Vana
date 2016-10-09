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
-- Mike at the dungeon entrance from Perion (1040001)

dofile("scripts/utils/npcHelper.lua");

addText("Go through here and you will find the Dungeon of Victoria. ");
addText("Take care...");
sendNext();

if isQuestActive(2048) then
	addText("Hmmm... so you want to know how to get " .. blue(itemRef(4021009)) .. ", " .. blue(itemRef(4003002)) .. ", " .. blue(itemRef(4001005)) .. " and " .. blue(itemRef(4001006)) .. "? ");
	addText("What you plan to do with these precious materials? ");
	addText("I've heard... ");
	addText("I've studied a little about the island before working as a guard...");
	sendNext();

	addText(blue(itemRef(4021009)) .. " and " .. blue(itemRef(4003002)) .. " um... ");
	addText("I think the fairies of " .. mapRef(101000000) .. " should know something about them. ");
	addText("The story about " .. itemRef(4003002) .. " is that it never melts. ");
	addText("They probably have " .. itemRef(4003002) .. " too.");
	sendNext();

	addText(blue(itemRef(4001005)) .. " and " .. blue(itemRef(4001006)) .. " are the problem. ");
	addText("The monsters probably have them, they've been around here for a long time... ");
	addText("Regarding " .. itemRef(4001005) .. "... ");
	addText("Oh, yes! ");
	addText("The Golem should have it, since it was created by wizards long ago...");
	sendNext();

	addText(blue(itemRef(4001006)) .. " ... I've heard of it, a flame that looks like a pen... ");
	addText("It has something to do with a dragon blowing fire or something like that. ");
	addText("Anyway, if it's cruel, it will be difficult for you to take the " .. itemRef(4001006) .. ". ");
	addText("Good luck!");
	sendNext();
end