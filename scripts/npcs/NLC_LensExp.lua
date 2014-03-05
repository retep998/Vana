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
-- Bomack (NLC Cosmetic Eyes, Regular)

dofile("scripts/lua_functions/beautyFunctions.lua");
dofile("scripts/lua_functions/npcHelper.lua");

addText("What's up! ");
addText("I'm Bomack. ");
addText("If you use the regular coupon, you'll be hooked up with a random pair of cosmetic lenses. ");
addText("You wanna use " .. blue(itemRef(5152035)) .. " and go forward with the procedure?");
answer = askYesNo();

if answer == answer_yes then
	newEyes = {};
	getEyeColour(newEyes);
	if getItemAmount(5152035) > 0 then
		giveItem(5152035, -1);
		setStyle(newEyes[getRandomNumber(#newEyes)]);
		addText("Enjoy your new and improved cosmetic lenses!");
		sendNext();
	else
		addText("Ah, it looks like you don't have the right coupon for this place. ");
		addText("Sorry, but that means it's a no-go with the procedure. ");
		sendNext();
	end
else
	addText("For real? ");
	addText("Nah, that's fine. ");
	addText("Some people get the cold feet, I can sympathize. ");
	addText("When you decide to make the change, you just let me know.");
	sendNext();
end