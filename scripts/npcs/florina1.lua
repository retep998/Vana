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
-- Pison (Tour Guide, teleports from Florina Beach)

origin = getPlayerVariable("florina_origin", true);
addText("So you want to leave #b#m110000000##k? If you want, I can take you back to #b#m" .. origin .. "##k.\r\n");
addText("#b#L0# #b I would like to go back now.");
askChoice(); -- We don't care about the result O_o

addText("Are you sure you want to return to #b#m" .. origin .. "##k? Alright, we'll have to get going fast. Do you want to head back to #m" .. origin .. "# now?");
yes = askYesNo();

if yes == 1 then
	deletePlayerVariable("florina_origin");
	setMap(origin);
else
	addText("You must have some business to take care of here. It's not a bad idea to take some rest at #m" .. origin .. "#. Look at me; I love it here so much that I wound up living here. Hahaha ¡¦ anyway, talk to me when you feel like going back.");
	sendNext();
end