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
-- J.J. (NLC Cosmetic Eyes, VIP)

dofile("scripts/utils/beautyHelper.lua");
dofile("scripts/utils/npcHelper.lua");

choices = getFaceColours({ face_amethyst, face_black, face_blue, face_green, face_hazel, face_red, face_sapphire, face_violet });

itemId = 5152036;

addText("Hi there! ");
addText("I'm J.J., the one in charge of the cosmetic lenses here at NLC Shop! ");
addText("With our specialized machine, you can see the results of your potential treatment in advance. ");
addText("What kind of lens would you like to wear? ");
addText("Please choose the style of your liking.");
choice = askStyle(choices);

if giveItem(itemId, -1) then
	setStyle(selectChoice(choices, choice));
	addText("Enjoy your new and improved cosmetic lenses!");
	sendNext();
else
	addText("I'm sorry, but I don't think you have our cosmetic lens coupon with you. ");
	addText("We can't proceed without the coupon.");
	sendNext();
end