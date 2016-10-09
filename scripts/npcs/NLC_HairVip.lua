--[[
Copyright (C) 2008-2016 Vana Development Team

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

dofile("scripts/utils/beautyHelper.lua");
dofile("scripts/utils/npcHelper.lua");

validHairs = getHairStyles(getGenderStyles({
	["male"] = {30730, 30280, 30220, 30410, 30200, 30050, 30230, 30160, 30110, 30250},
	["female"] = {31730, 31310, 31470, 31150, 31160, 31300, 31260, 31220, 31410, 31270},
}));

vipHairItem = 5150031;
vipHairColour = 5151026;
vipMembershipItem = 5420001;

choices = {
	makeChoiceHandler(" Haircut(VIP coupon)", function()
		addText("I can totally change up your hairstyle and make it look so good. ");
		addText("Why don't you change it up a bit? ");
		addText("If you have " .. blue(itemRef(vipHairItem)) .. " I'll change it for you. ");
		addText("Choose the one to your liking~");
		return {validHairs, vipHairItem, true};
	end),
	makeChoiceHandler(" Dye your hair(VIP coupon)", function()
		addText("I can totally change your haircolor and make it look so good. ");
		addText("Why don't you change it up a bit? ");
		addText("With " .. blue(itemRef(vipHairColour)) .. " I'll change it for you. ");
		addText("Choose the one to your liking.");
		return {getHairColours(hair_all), vipHairColour, false};
	end),
	-- TODO FIXME beauty
	-- Verify the choices for this option if possible, might be lost data but I'm not sure that the choices would be consistent with regular VIP
	makeChoiceHandler(" Change Hairstyle (VIP Membership Coupon)", function()
		addText("I can totally change up your hairstyle and make it look so good. ");
		addText("Why don't you change it up a bit? ");
		addText("If you have " .. blue(itemRef(vipMembershipItem)) .. " I'll change it for you. ");
		addText("With this coupon, you have the power to change your hairstyle to something totally new, as often as you want, for ONE MONTH! ");
		addText("Now, please choose the hairstyle of your liking.");
		return {validHairs, vipMembershipItem, true};
	end),
};

addText("I'm the head of this hair salon Mani. ");
addText("If you have " .. blue(itemRef(vipHairItem)) .. ", " .. blue(itemRef(vipHairColour)) .. " or " .. blue(itemRef(vipMembershipItem)) .. ", allow me to take care of your hairdo. ");
addText("Please choose the one you want.\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

data = selectChoice(choices, choice);
choices, itemId, isHaircut = data[1], data[2], data[3];

choice = askStyle(choices);

if (itemId == vipMembershipItem and getItemAmount(itemId) > 0) or giveItem(itemId, -1) then
	setStyle(selectChoice(choices, choice));
	addText("Enjoy your new and improved hairstyle!");
	sendOk();
else
	addText("Hmmm...it looks like you don't have our designated coupon...");

	addText("I'm afraid I can't ");
	if isHaircut then
		addText("give you a haircut ");
	else
		addText("dye your hair ");
	end
	addText("without it. ");
	addText("I'm sorry.");
	sendNext();
end