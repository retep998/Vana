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

addText("Orbis Station has lots of platforms available to choose from. ");
addText("You need to choose the one that'll take you to the destination of your choice. ");
addText("Which platform will you take?\r\n");
addText(blue(choiceList({
	"The platform to the ship that heads to Ellinia",
	"The platform to the ship that heads to Ludibrium",
	"The platform to Hak that heads to Leafre",
	"The platform to Hak that heads to Mu Lung",
	"The platform to Geenie that heads to Ariant",
})));
choice = askChoice();

addText("Even if you took the wrong passage you can get back here using the portal, so no worries.");
if choice == 0 then
	map = 200000110;
	addText(" Will you move to the " .. blue("platform to the ship that heads to Ellinia") .. "?");
elseif choice == 1 then
	map = 200000120;
	addText(" Will you move to the " .. blue("platform to the ship that heads to Ludibrium") .. "?");
elseif choice == 2 then
	map = 200000130;
	addText("\r\n");
	addText("Will you move to the " .. blue("platform to the ship that heads to Leafre") .. "?");
elseif choice == 3 then
	map = 200000140;
	addText(" Will you move to the " .. blue("platform to Hak that heads to Mu Lung") .. "?");
elseif choice == 4 then
	map = 200000150;
	addText(" Will you move to the " .. blue("platform to Geenie that heads to Ariant"));
end
answer = askYesNo();

if answer == answer_yes then
	setMap(map, "west00");
else
	addText("Please make sure you know where you are going and then go to the platform through me.");
	if choice == 0 or choice == 1 or choice == 2 then
		addText(" The ride is on schedule so you better not miss it!");
	end
	sendNext();
end