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
-- Dr Lenu (Eyes - Henesys)

dofile("scripts/lua_functions/beautyFunctions.lua");

addText("Hi, there~! I'm Dr. Lenu, in charge of the cosmetic lenses here at the Henesys Plastic Surgery Shop! With #b#t5152010##k or #b#t5152013##k, you can let us take care of the rest and have the kind of beautiful look you've always craved~! Remember, the first thing everyone notices about you is the eyes, and we can help you find the cosmetic lens that most fits you! Now, what would you like to use?\r\n");
addText("#b#L0# Cosmetic Lenses at Henesys (Reg. coupon)#l\r\n");
addText("#L1# Cosmetic Lenses at Henesys (VIP coupon)#l");
what = askChoice();

newEyes = {};
getEyeColour(newEyes);
if what == 0 then
	addText("If you use the regular coupon, you'll be awarded a random pair of cosmetic lenses. Are you going to use #b#t5152010##k and really make the change to your eyes?");
	yes = askYesNo();
	
	if yes == 1 then
		if getItemAmount(5152010) > 0 then
			giveItem(5152010, -1);
			setStyle(newEyes[getRandomNumber(#newEyes)]);
			addText("Enjoy your new and improved cosmetic lenses!");
			sendNext();
		else
			addText("I'm sorry, but I don't think you have our cosmetic lens coupon with you right now. Without the coupon, I'm afraid I can't do it for you");
			sendNext();
		end
	else
		addText("I see. That's understandable, since you're unsure whether you'll get the cosmetic lenses of your liking. We're in no hurry, whatsoever, so take your time! Please let me know when you decide to make the change~!");
		sendNext();
	end
elseif what == 1 then
	addText("With our specialized machine, you can see yourself after the treatment in advance. What kind of lens would you like to wear? Choose the style of your liking...");
	style = askStyle(newEyes) + 1;
	
	if getItemAmount(5152013) > 0 then
		giveItem(5152013, -1);
		if newEyes[style] > 0 then
			setStyle(newEyes[style]);
		end
		addText("Enjoy your new and improved cosmetic lenses!");
		sendNext();
	else
		addText("I'm sorry, but I don't think you have our cosmetic lens coupon with you right now. Without the coupon, I'm afraid I can't do it for you");
		sendNext();
	end
end
