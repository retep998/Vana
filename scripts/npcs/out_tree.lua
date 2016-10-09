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
-- Scarf Snowman (Hill of Christmas exit)

dofile("scripts/utils/npcHelper.lua");

addText("Have you decorated your tree nicely? ");
addText("It's an interesting experience, to say the least, decorating a Christmas tree with other people. ");
addText("Don¡¯t cha think?  ");
addText("Oh yeah ... are you suuuuure you want to leave this place?");
answer = askYesNo();

if answer == answer_yes then
	setMap(209000000);
else
	addText("You need more time decorating trees, huh? ");
	addText("If you ever feel like leaving this place, feel free to come talk to me~");
	sendNext();
end