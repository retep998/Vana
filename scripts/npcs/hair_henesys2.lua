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
-- Brittany (EXP hair and color - Henesys)

dofile("scripts/utils/beautyHelper.lua");
dofile("scripts/utils/npcHelper.lua");

validHairs = getGenderStyles({
	["male"] = {30410, 30200, 30310, 30330, 30060, 30150, 30210, 30140, 30120, 30560, 30510, 30610},
	["female"] = {31150, 31310, 31160, 31300, 31100, 31410, 31030, 31080, 31070, 31610, 31350, 31510},
});
validColors = { hair_black, hair_brown, hair_green, hair_orange, hair_purple, hair_red };

expHairItem = 5150010;
expDyeItem = 5150000;

choices = {
	makeChoiceHandler(" Haircut(EXP coupon)", function()
		addText("If you use the EXP coupon your hair will change RANDOMLY with a chance to obtain a new experimental style that even you didn't think was possible. ");
		addText("Are you going to use " .. blue(itemRef(expHairItem)) .. " and really change your hairstyle?");
		return {getHairStyles(validHairs), expHairItem, false};
	end),
	makeChoiceHandler(" Dye your hair(REG coupon)", function()
		addText("If you use a regular coupon your hair will change RANDOMLY. ");
		addText("Do you still want to use " .. blue(itemRef(expDyeItem)) .. " and change it up?");
		return {getHairColours(validColors), expDyeItem, true};
	end),
};

addText("I'm Brittany the assistant. ");
addText("If you have " .. blue(itemRef(expHairItem)) .. " or " .. blue(itemRef(expDyeItem)) .. " by any chance, then how about letting me change your hairdo?\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

data = selectChoice(choices, choice);
validStyles, itemId, isColour = data[1], data[2], data[3];
answer = askYesNo();

if answer == answer_yes then
	if giveItem(itemId, -1) then
		if isColour then
			setStyle(getRandomHairColour(validStyles));
		else
			setStyle(getRandomHair(validStyles));
		end

		addText("Enjoy!");
		sendOk();
	else
		addText("Hmmm...it looks like you don't have our designated coupon...");
		addText("I'm afraid I can't give you a haircut without it. ");
		addText("I'm sorry.");
		sendNext();
	end
else
	addText("I understand...think about it, and if you still feel like changing come talk to me.");
	sendNext();
end