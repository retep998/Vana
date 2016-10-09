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
-- Cliff

dofile("scripts/utils/npcHelper.lua");

addText("Do you see the group of snowman standing over there? ");
addText("Go talk to one of them, and it'll take you to the famous christmas tree--it's just humongous! ");
addText("While there, you can decorate the tree using various kinds of ornaments. ");
addText("What do you think? ");
addText("Sounds fun, right?");
sendNext();

addText("Only 6 can be in the map at one time, and you can't " .. blue("trade or open a store") .. " there. ");
addText("Also, only you can pick up the ornaments that you have dropped, so don't worry about losing your ornaments here.");
sendBackNext();

addText("Of course, the items you drop in the map will never disappear. ");
addText("When you are ready, you can leave through the snowman that is inside. ");
addText("You will recover all the items that you dropped inside the map when you leave, so don't worry about picking up all your items. ");
addText("Isn't that sweet?");
sendBackNext();

addText("Well then, go see " .. npcRef(2002001) .. ", buy some christmas ornaments there, and then decorate the tree with those~ ");
addText("Oh yeah! ");
addText("The biggest and most beautiful ornament can't be bought from him. ");
addText("It's probably been... taken by a monster! ");
addText("Dum dum dum~");
sendBackOk();