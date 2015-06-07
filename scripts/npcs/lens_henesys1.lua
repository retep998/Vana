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
-- Dr Lenu (Face - Henesys)

dofile("scripts/utils/beautyHelper.lua");
dofile("scripts/utils/npcHelper.lua");

expItem = 5152010;
vipItem = 5152013;
validFaceColours = {face_amethyst, face_black, face_blue, face_hazel, face_sapphire, face_violet};

choices = {
	makeChoiceHandler(" Cosmetic Lenses at Henesys (Reg. coupon)", function()
		addText("If you use the regular coupon, you'll be awarded a random pair of cosmetic lenses. ");
		addText("Are you going to use " .. blue(itemRef(expItem)) .. " and really make the change to your eyes?");
		answer = askYesNo();

		if answer == answer_yes then
			if giveItem(expItem, -1) then
				setStyle(getRandomFaceColour(validFaceColours));
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
	end),
	makeChoiceHandler(" Cosmetic Lenses at Henesys (VIP coupon)", function()
		validFaceColours = getFaceColours(validFaceColours);

 		addText("With our specialized machine, you can see yourself after the treatment in advance. ");
		addText("What kind of lens would you like to wear? ");
		addText("Choose the style of your liking...");
		choice = askStyle(validFaceColours);

		if giveItem(vipItem, -1) then
			setStyle(selectChoice(validFaceColours, choice));

			addText("Enjoy your new and improved cosmetic lenses!");
			sendNext();
		else
			addText("I'm sorry, but I don't think you have our cosmetic lens coupon with you right now. ");
			addText("Without the coupon, I'm afraid I can't do it for you");
			sendNext();
		end
	end),
};

addText("Hi, there~! ");
addText("I'm Dr. Lenu, in charge of the cosmetic lenses here at the Henesys Plastic Surgery Shop! ");
addText("With " .. blue(itemRef(expItem)) .. " or " .. blue(itemRef(vipItem)) .. ", you can let us take care of the rest and have the kind of beautiful look you've always craved~! ");
addText("Remember, the first thing everyone notices about you is the eyes, and we can help you find the cosmetic lens that most fits you! ");
addText("Now, what would you like to use?\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);