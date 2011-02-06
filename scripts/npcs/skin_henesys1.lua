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
-- Ms. Tan (Henesys skin care)

dofile("scripts/lua_functions/beautyFunctions.lua");

addText("Well, hello! Welcome to the Henesys Skin-Care! Would you like to have a firm, tight, healthy looking skin like mine?  With #b#t5153000##k, you can let us take care of the rest and have the kind of skin you've always wanted~!");
sendNext();

addText("With our specialized machine, you can see yourself after the treatment in advance. What kind of skin-treatment would you like to do? Choose the style of your liking...");
styles = {};
getSkins(styles);
style = askStyle(styles) + 1;

if getItemAmount(5153000) > 0 then
	giveItem(5153000, -1);
	setStyle(styles[style]);
	addText("Enjoy your new and improved skin!");
	sendNext();
else
	addText("Um...you don't have the skin-care coupon you need to receive the treatment. Sorry, but I am afraid we can't do it for you.");
	sendNext();
end
