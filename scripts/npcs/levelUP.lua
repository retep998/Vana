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
-- KIN

if not isGm() then
	addText("You shouldn't be here, " .. playerRef() .. "~");
	sendNext();
	return;
end

dofile("scripts/lua_functions/beautyFunctions.lua");
dofile("scripts/lua_functions/npcHelper.lua");

addText("Hello, what can I do for you today?\r\n");
addText(blue(choiceList({
	"Skin",
	"Hair",
	"Hair Color",
	"Eyes",
	"Eyes Color",
	"Random New Look",
})));
choice = askChoice();

if choice == 5 then
	setStyle(getRandomFace());
	setStyle(getRandomHair());
else
	styles = {};
	if choice == 0 then getSkins(styles);
	elseif choice == 1 then getHairs(styles);
	elseif choice == 2 then getHairColours(styles);
	elseif choice == 3 then getEyeStyles(styles);
	elseif choice == 4 then getEyeColour(styles);
	end
	style = askStyle(styles) + 1;

	if styles[style] >= 0 then
		setStyle(styles[style]);
	end
end