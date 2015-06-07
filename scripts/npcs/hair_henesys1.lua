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
-- Natalie (VIP hair and color - Henesys)

dofile("scripts/utils/beautyHelper.lua");
dofile("scripts/utils/npcHelper.lua");

validHairs = getGenderStyles({
	["male"] = {30410, 30200, 30310, 30330, 30060, 30150, 30210, 30140, 30120, 30030, 30020, 30000},
	["female"] = {31150, 31310, 31160, 31300, 31100, 31410, 31030, 31080, 31070, 31050, 31040, 31000},
});
validColors = { hair_black, hair_brown, hair_green, hair_orange, hair_purple, hair_red };

vipHairItem = 5150001;
vipDyeItem = 5151001;
vipMembershipItem = 5420002;

choices = {
	makeChoiceHandler(" Haircut(VIP coupon)", function()
		addText("I can totally change up your hairstyle and make it look so good. ");
		addText("Why don't you change it up a bit? ");
		addText("If you have " .. blue(itemRef(vipHairItem)) .. " I'll change it for you. ");
		addText("Choose the one to your liking~");
		return {getHairStyles(validHairs), vipHairItem, "give you a haircut "};
	end),
	makeChoiceHandler(" Dye your hair(VIP coupon)", function()
		addText("I can totally change your haircolor and make it look so good. ");
		addText("Why don't you change it up a bit? ");
		addText("With " .. blue(itemRef(vipDyeItem)) .. " I'll change it for you. ");
		addText("Choose the one to your liking.");
		return {getHairColours(validColors), vipDyeItem, "dye your hair "};
	end),
	-- TODO FIXME beauty
	-- Verify the choices for this option if possible, might be lost data but I'm not sure that the choices would be consistent with regular VIP
	makeChoiceHandler(" Change Hairstyle (VIP Membership Coupon)", function()
		addText("I can totally change up your hairstyle and make it look so good. ");
		addText("Why don't you change it up a bit? ");
		addText("If you have " .. blue(itemRef(vipMembershipItem)) .. " I'll change it for you. ");
		addText("With this coupon, you have the power to change your hairstyle to something totally new, as often as you want, for ONE MONTH! ");
		addText("Now, please choose the hairstyle of your liking.");
		return {getHairStyles(validHairs), vipMembershipItem, "give you a haircut "};
	end),
};

addText("I'm the head of this hair salon Natalie. ");
addText("If you have " .. blue(itemRef(vipHairItem)) .. ", " .. blue(itemRef(vipDyeItem)) .. " or " .. blue(itemRef(vipMembershipItem)) .. ", allow me to take care of your hairdo. ");
addText("Please choose the one you want.\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

data = selectChoice(choices, choice);
validStyles, itemId, actionText = data[1], data[2], data[3];

choice = askStyle(validStyles);

if (itemId == vipMembershipItem and getItemAmount(itemId) > 0) or giveItem(itemId, -1) then
	setStyle(selectChoice(validStyles, choice));
	addText("Enjoy your new and improved hairstyle!");
	sendOk();
else
	addText("Hmmm...it looks like you don't have our designated coupon...");
	addText("I'm afraid I can't " .. actionText .. "without it. ");
	addText("I'm sorry.");
	sendNext();
end