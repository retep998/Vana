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
-- Pison (Tour Guide, teleports from Florina Beach)

dofile("scripts/lua_functions/npcHelper.lua");

origin = getPlayerVariable("florina_origin", type_int);
if origin == nil then
	origin = 104000000;
end

addText("So you want to leave " .. blue(mapRef(110000000)) .. "? ");
addText("If you want, I can take you back to " .. blue(mapRef(origin)) .. ".\r\n");
addText(blue(choiceRef("I would like to go back now.")));
askChoice();

addText("Are you sure you want to return to " .. blue(mapRef(origin)) .. "? ");
addText("Alright, we'll have to get going fast. ");
addText("Do you want to head back to " .. mapRef(origin) .. " now?");
answer = askYesNo();

if answer == answer_yes then
	deletePlayerVariable("florina_origin");
	setMap(origin);
else
	addText("You must have some business to take care of here. ");
	addText("It's not a bad idea to take some rest at " .. mapRef(origin) .. ". ");
	addText("Look at me; I love it here so much that I wound up living here. ");
	addText("Hahaha ¡¦ anyway, talk to me when you feel like going back.");
	sendNext();
end