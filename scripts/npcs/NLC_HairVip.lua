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
-- Mani (VIP hair and color - NLC)

dofile("scripts/lua_functions/beautyFunctions.lua");
dofile("scripts/lua_functions/npcHelper.lua");

if getGender() == gender_male then
	hairs = {30730, 30280, 30220, 30410, 30200, 30050, 30230, 30160, 30110, 30250};
elseif getGender() == gender_female then
	hairs = {31730, 31310, 31470, 31150, 31160, 31300, 31260, 31220, 31410, 31270};
end

addText("I'm the head of this hair salon Mani. ");
addText("If you have " .. blue(itemRef(5150031)) .. ", " .. blue(itemRef(5151031)) .. " or " .. blue(itemRef(5420001)) .. ", allow me to take care of your hairdo. ");
addText("Please choose the one you want.\r\n");
addText(blue(choiceList({
	" Haircut(VIP coupon)",
	" Dye your hair(VIP coupon)",
	" Change Hairstyle (VIP Membership Coupon)",
})));
choice = askChoice();

newHair = {};
if choice == 0 then
	addText("I can totally change up your hairstyle and make it look so good. ");
	addText("Why don't you change it up a bit? ");
	addText("If you have " .. blue(itemRef(5150031)) .. " I'll change it for you. ");
	addText("Choose the one to your liking~");
	getHairs(newHair, hairs);
elseif choice == 1 then
	addText("I can totally change your haircolor and make it look so good. ");
	addText("Why don't you change it up a bit? ");
	addText("With " .. blue(itemRef(5151026)) .. " I'll change it for you. ");
	addText("Choose the one to your liking.");
	getHairColours(newHair);
elseif choice == 2 then
	addText("I can totally change up your hairstyle and make it look so good. ");
	addText("Why don't you change it up a bit? ");
	addText("If you have " .. blue(itemRef(5420001)) .. " I'll change it for you. ");
	addText("With this coupon, you have the power to change your hairstyle to something totally new, as often as you want, for ONE MONTH! ");
	addText("Now, please choose the hairstyle of your liking.");
	getHairs(newHair, hairs)
end
style = askStyle(newHair) + 1;

if choice == 0 then item = 5150031;
elseif choice == 1 then item = 5151026;
elseif choice == 2 then item = 5420001;
end

if getItemAmount(item) > 0 then
	giveItem(item, -1);
	if newHair[style] > 0  then
		setStyle(newHair[style]);
	end
	addText("Enjoy your new and improved hairstyle!");
	sendOk();
else
	addText("Hmmm...it looks like you don't have our designated coupon...");

	addText("I'm afraid I can't ");
	if choice == 0 or choice == 2 then
		addText("give you a haircut ");
	elseif choice == 1 then
		addText("dye your hair ");
	end
	addText("without it. ");
	addText("I'm sorry.");
	sendNext();
end