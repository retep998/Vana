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
-- Roger: Apple Quest End

dofile("scripts/utils/npcHelper.lua");

addText("How easy is it to consume the item?  ");
addText("Simple, right? ");
addText("You can set a " .. blue("hotkey") .. " on the right bottom slot.  ");
addText("Haha you didn't know that! right?  ");
addText("Oh, and if you are a beginner, HP will automatically recover itself as time goes by.  ");
addText("Well it takes time but this is one of the strategies for the beginners.");
sendNext();

addText("Alright!  ");
addText("Now that you have learned alot, I will give you a present.  ");
addText("This is a must for your travel in Maple World, so thank me!  ");
addText("Please use this under emergency cases!");
sendBackNext();

addText("Okay, this is all I can teach you.  ");
addText("I know it's sad but it is time to say good bye.  ");
addText("Well take care of yourself and Good luck my friend!\r\n");
addText(questCompleteIcon() .. "\r\n");
addText(questItemIcon(2010000, 3) .. "\r\n");
addText(questItemIcon(2010009, 3) .. "\r\n");
addText(questExpIcon(10) .. "\r\n");
sendBackNext();

endQuest(1021);
giveItem(2010000, 3);
giveItem(2010009, 3);
giveExp(10);