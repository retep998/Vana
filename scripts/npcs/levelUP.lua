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
-- KIN

if not isGm() then
	addText("You shouldn't be here, " .. playerRef() .. "~");
	sendNext();
	return;
end

dofile("scripts/utils/beautyHelper.lua");
dofile("scripts/utils/npcHelper.lua");

choices = {
	makeChoiceData("Skin", getSkinStyles()),
	makeChoiceData("Hair", getHairStyles()),
	makeChoiceData("Hair Color", getHairColours()),
	makeChoiceData("Face", getFaceStyles()),
	makeChoiceData("Eye Color", getFaceColours()),
	makeChoiceHandler("Random New Look", function()
		setStyle(getRandomFace(getAllFaces()));
		setStyle(getRandomHair(getAllHairs()));
	end),
};

addText("Hello, what can I do for you today?\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

data = selectChoice(choices, choice);
if data ~= nil then
	choice = askStyle(data);
	setStyle(selectChoice(data, choice));
end