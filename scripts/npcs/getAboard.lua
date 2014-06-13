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
-- Platform Usher (Orbis)

dofile("scripts/lua_functions/npcHelper.lua");

choices = {
	makeChoiceData("The platform to the ship that heads to Ellinia", {200000110,  blue("platform to the ship that heads to Ellinia") .. "?", false, true}),
	makeChoiceData("The platform to the ship that heads to Ludibrium", {200000120,  blue("platform to the ship that heads to Ludibrium") .. "?", true, true}),
	makeChoiceData("The platform to Hak that heads to Leafre", {200000130,  blue("platform to the ship that heads to Leafre") .. "?", false, true}),
	makeChoiceData("The platform to Hak that heads to Mu Lung", {200000140,  blue("platform to Hak that heads to Mu Lung") .. "?"}),
	makeChoiceData("The platform to Geenie that heads to Ariant", {200000150,  blue("platform to Geenie that heads to Ariant")}),
	makeChoiceData("The platform to Geenie that heads to Ereve", {200000160,  blue("platform to Geenie that heads to Ereve")}),
};

addText("Orbis Station has lots of platforms available to choose from. ");
addText("You need to choose the one that'll take you to the destination of your choice. ");
addText("Which platform will you take?\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

data = selectChoice(choices, choice);
mapId, blueText, extraNewline, extraScheduleText = data[1], data[2], data[3], data[4];

addText("Even if you took the wrong passage you can get back here using the portal, so no worries.");
if extraNewline then
	addText("\r\n");
else
	addText(" ");
end
addText("Will you move to the " .. blueText);
answer = askYesNo();

if answer == answer_yes then
	setMap(mapId, "west00");
else
	addText("Please make sure you know where you are going and then go to the platform through me.");
	if extraScheduleText then
		addText(" The ride is on schedule so you better not miss it!");
	end
	sendNext();
end