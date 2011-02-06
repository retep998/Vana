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
-- J.J. (NLC Cosmetic Eyes, VIP)

dofile("scripts/lua_functions/beautyFunctions.lua");
newEyes = {};
getEyeColour(newEyes);

addText("Hi there! I'm J.J., the one in charge of the cosmetic lenses here at NLC Shop! With our specialized machine, you can see the results of your potential treatment in advance. What kind of lens would you like to wear? Please choose the style of your liking.");
style = askStyle(newEyes) + 1;

if getItemAmount(5152036) > 0 then
	giveItem(5152036, -1);
	if newEyes[style] > 0 then
		setStyle(newEyes[style]);
	end
	addText("Enjoy your new and improved cosmetic lenses!");
	sendNext();
else
	addText("I'm sorry, but I don't think you have our cosmetic lens coupon with you. We can't proceed without the coupon.");
	sendNext();
end