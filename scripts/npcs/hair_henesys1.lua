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
-- Natalie (VIP hair and color - Henesys)

dofile("scripts/lua_functions/beautyFunctions.lua");
dofile("scripts/lua_functions/npcHelper.lua");

if getGender() == gender_male then
	hairs = {30030, 30020, 30000, 30310, 30330, 30060, 30150, 30410, 30210, 30140, 30120, 30200};
elseif getGender() == gender_female then
	hairs = {31050, 31040, 31000, 31150, 31310, 31300, 31160, 31100, 31410, 31030, 31080, 31070};
end

addText("I'm the head of this hair salon Natalie. ");
addText("If you have " .. blue(itemRef(5150001)) .. ", " .. blue(itemRef(5151001)) .. " or " .. blue(itemRef(5420002)) .. ", allow me to take care of your hairdo. ");
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
	addText("If you have " .. blue(itemRef(5150001)) .. " I'll change it for you. ");
	addText("Choose the one to your liking~");
	getHairs(newHair, hairs);
elseif choice == 1 then
	addText("I can totally change your haircolor and make it look so good. ");
	addText("Why don't you change it up a bit? ");
	addText("With " .. blue(itemRef(5151001)) .. " I'll change it for you. ");
	addText("Choose the one to your liking.");
	getHairColours(newHair);
elseif choice == 2 then
	addText("I can totally change up your hairstyle and make it look so good. ");
	addText("Why don't you change it up a bit? ");
	addText("If you have " .. blue(itemRef(5420002)) .. " I'll change it for you. ");
	addText("With this coupon, you have the power to change your hairstyle to something totally new, as often as you want, for ONE MONTH! ");
	addText("Now, please choose the hairstyle of your liking.");
	getHairs(newHair, hairs)
end
style = askStyle(newHair) + 1;

if choice == 0 then
	item = 5150001;
elseif choice == 1 then
	item = 5151001;
elseif choice == 2 then
	item = 5420002;
end
if getItemAmount(item) > 0 then
	giveItem(item, -1);
	if newHair[style] > 0  then
		setStyle(newHair[style]);
	end
	addText("Enjoy your new and improved hairstyle!");
	sendOk();
else
	text = nil;
	if choice == 0 or choice == 2 then
		text = "give you a haircut ";
	elseif choice == 1 then
		text = "dye your hair ";
	end
	addText("Hmmm...it looks like you don't have our designated coupon...");
	addText("I'm afraid I can't " .. text .. "without it. ");
	addText("I'm sorry.");
	sendNext();
end