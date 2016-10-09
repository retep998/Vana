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
-- Ms. Tan (Henesys skin care)

dofile("scripts/utils/beautyHelper.lua");
dofile("scripts/utils/npcHelper.lua");

item = 5153000;

addText("Well, hello! ");
addText("Welcome to the Henesys Skin-Care! ");
addText("Would you like to have a firm, tight, healthy looking skin like mine?  ");
addText("With " .. blue(itemRef(item)) .. ", you can let us take care of the rest and have the kind of skin you've always wanted~!");
sendNext();

addText("With our specialized machine, you can see yourself after the treatment in advance. ");
addText("What kind of skin-treatment would you like to do? ");
addText("Choose the style of your liking...");
validSkins = getSkinStyles({ skin_light, skin_tanned, skin_dark, skin_pale });

choice = askStyle(validSkins);

if giveItem(item, -1) then
	setStyle(selectChoice(validSkins, choice));
	addText("Enjoy your new and improved skin!");
	sendNext();
else
	addText("Um...you don't have the skin-care coupon you need to receive the treatment. ");
	addText("Sorry, but I am afraid we can't do it for you.");
	sendNext();
end