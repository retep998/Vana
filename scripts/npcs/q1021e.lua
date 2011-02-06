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
-- Roger: Apple Quest End

addText("How easy is it to consume the item?  Simple, right? You can set a #bhotkey#k on the right bottom slot.  Haha you didn't know that! right?  Oh, and if you are a beginner, HP will automatically recover itself as time goes by.  Well it takes time but this is one of the strategies for the beginners.");
sendNext();

addText("Alright!  Now that you have learned alot, I will give you a present.  This is a must for your travel in Maple World, so thank me!  Please use this under emergency cases!");
sendBackNext();

addText("Okay, this is all I can teach you.  I know it's sad but it is time to say good bye.  Well take care of yourself and Good luck my friend!\r\n");
addText("#fUI/UIWindow.img/QuestIcon/4/0#\r\n");
addText("#v2010000# 3 #t2010000#\r\n");
addText("#v2010009# 3 #t2010009#\r\n");
addText("#fUI/UIWindow.img/QuestIcon/8/0# 10 exp");
sendBackNext();

endQuest(1021);
giveItem(2010000, 3);
giveItem(2010009, 3);
giveEXP(10);