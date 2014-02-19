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
-- Louis

dofile("scripts/lua_functions/npcHelper.lua");

addText("You want to get out of here? ");
addText("Well ... this place can really wear you down ... ");
addText("I'm used to it so I'm fine, though. ");
addText("Anyway make sure you remember that if you leave this place through me, you'll have to restart the mission. ");
addText("Do you still want to?");
answer = askYesNo();

if answer == answer_no then
	addText("Isn't it awful that you have to restart the whole thing? ");
	addText("Keep trying...the more you go through it, the more you'll know about this place in and out. ");
	addText("Pretty soon you'll be able to go through this with your eyes closed hehe.");
	sendNext();
else
	setMap(101000000);
end