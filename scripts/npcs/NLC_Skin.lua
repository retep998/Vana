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
-- Miranda (NLC Dermatologist)

dofile("scripts/lua_functions/beautyFunctions.lua");
dofile("scripts/lua_functions/npcHelper.lua");

item = 5153009;

addText("Well, hello! ");
addText("Welcome to the NLC Skin-Care! ");
addText("Would you like to have a firm, tight, healthy looking skin like mine?  ");
addText("With " .. blue(itemRef(item)) .. ", you can let us take care of the rest and have the kind of skin you've always wanted~!");
sendNext();

addText("With our specialized machine, you can see your expected results after treatment in advance. ");
addText("What kind of skin-treatment would you like to do? ");
addText("Choose your preferred style.");
choices = getSkinStyles();
choice = askStyle(choices);

if giveItem(item, -1) then
	setStyle(selectChoice(choices, choice));
	addText("Enjoy your new and improved skin!");
	sendNext();
else
	addText("Um...you don't have the skin-care coupon you need to receive the treatment. ");
	addText("Sorry, but I am afraid we can't do it for you.");
	sendNext();
end