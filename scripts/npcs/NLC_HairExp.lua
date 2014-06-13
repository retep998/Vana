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
-- Ari (EXP hair and color - NLC)

dofile("scripts/utils/beautyFunctions.lua");
dofile("scripts/utils/npcHelper.lua");

validHairs = getGenderStyles({
	["male"] = {30400, 30360, 30440, 30410, 30200, 30050, 30230, 30160, 30110, 30250},
	["female"] = {31560, 31720, 31450, 31150, 31160, 31300, 31260, 31220, 31410, 31270},
});

expHairItem = 5150030;
expColourItem = 5151025;

choices = {
	makeChoiceHandler(" Haircut(EXP coupon)", function()
		addText("If you use the EXP coupon your hair will change RANDOMLY with a chance to obtain a new experimental style that even you didn't think was possible. ");
		addText("Are you going to use " .. blue(itemRef(expHairItem)) .. " and really change your hairstyle?");
		return {getHairStyles(validHairs), expHairItem, false};
	end),
	makeChoiceHandler(" Dye your hair(REG coupon)", function()
		addText("If you use a regular coupon your hair will change RANDOMLY. ");
		addText("Do you still want to use " .. blue(itemRef(expColourItem)) .. " and change it up?");
		return {getHairColours(hair_all), expColourItem, true};
	end),
};

addText("I'm Brittany the assistant. ");
addText("If you have " .. blue(itemRef(expHairItem)) .. " or " .. blue(itemRef(expColourItem)) .. " by any chance, then how about letting me change your hairdo?\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

data = selectChoice(choices, choice);
validStyles, itemId, isColour = data[1], data[2], data[3];

answer = askYesNo();

if answer == answer_yes then
	if giveItem(item, -1) then
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