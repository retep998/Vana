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
-- Exit Signs in Shumi's jump quests

dofile("scripts/lua_functions/npcHelper.lua");

addText("This device is connected to outside. ");
addText("Are you going to give up and leave this place? ");
addText("You'll have to start from scratch the next time you come in...");
answer = askYesNo();

if answer == answer_yes then
	setMap(103000100);
end