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
-- Dr Lenu (Eyes - Henesys)

dofile("scripts/lua_functions/beautyFunctions.lua");
dofile("scripts/lua_functions/npcHelper.lua");

addText("Hi, there~! ");
addText("I'm Dr. Lenu, in charge of the cosmetic lenses here at the Henesys Plastic Surgery Shop! ");
addText("With " .. blue(itemRef(5152010)) .. " or " .. blue(itemRef(5152013)) .. ", you can let us take care of the rest and have the kind of beautiful look you've always craved~! ");
addText("Remember, the first thing everyone notices about you is the eyes, and we can help you find the cosmetic lens that most fits you! ");
addText("Now, what would you like to use?\r\n");
addText(blue(choiceList({
	" Cosmetic Lenses at Henesys (Reg. coupon)",
	" Cosmetic Lenses at Henesys (VIP coupon)",
})));
choice = askChoice();

newEyes = {};
getEyeColour(newEyes);
if choice == 0 then
	addText("If you use the regular coupon, you'll be awarded a random pair of cosmetic lenses. ");
	addText("Are you going to use " .. blue(itemRef(5152010)) .. " and really make the change to your eyes?");
	answer = askYesNo();

	if answer == answer_yes then
		if getItemAmount(5152010) > 0 then
			giveItem(5152010, -1);
			setStyle(newEyes[getRandomNumber(#newEyes)]);
			addText("Enjoy your new and improved cosmetic lenses!");
			sendNext();
		else
			addText("I'm sorry, but I don't think you have our cosmetic lens coupon with you right now. ");
			addText("Without the coupon, I'm afraid I can't do it for you");
			sendNext();
		end
	else
		addText("I see. ");
		addText("That's understandable, since you're unsure whether you'll get the cosmetic lenses of your liking. ");
		addText("We're in no hurry, whatsoever, so take your time! ");
		addText("Please let me know when you decide to make the change~!");
		sendNext();
	end
elseif choice == 1 then
	addText("With our specialized machine, you can see yourself after the treatment in advance. ");
	addText("What kind of lens would you like to wear? ");
	addText("Choose the style of your liking...");
	style = askStyle(newEyes) + 1;

	if getItemAmount(5152013) > 0 then
		giveItem(5152013, -1);
		if newEyes[style] > 0 then
			setStyle(newEyes[style]);
		end
		addText("Enjoy your new and improved cosmetic lenses!");
		sendNext();
	else
		addText("I'm sorry, but I don't think you have our cosmetic lens coupon with you right now. ");
		addText("Without the coupon, I'm afraid I can't do it for you");
		sendNext();
	end
end