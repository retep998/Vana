--[[
Copyright (C) 2008-2011 Vana Development Team

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

dofile("scripts/lua_functions/beautyFunctions.lua");

addText("Hello, what can I do for you today?\r\n");
addText("#L0##bSkin#k#l\r\n");
addText("#L1##bHair#k#l\r\n");
addText("#L2##bHair Color#k#l\r\n");
addText("#L3##bEyes#k#l\r\n");
addText("#L4##bEyes Color#k#l\r\n");
addText("#L5##bRandom New Look#k#l\r\n");
what = askChoice();

if what == 5 then
	setStyle(getRandomFace());
	setStyle(getRandomHair());
else
	styles = {};
	if what == 0 then
		getSkins(styles);
	elseif what == 1 then
		getHairs(styles);
	elseif what == 2 then
		getHairColours(styles);
	elseif what == 3 then
		getEyeStyles(styles);
	elseif what == 4 then
		getEyeColour(styles);
	elseif what == 5 then
		setStyle(getRandomFace());
		setStyle(getRandomHair());
	end
	style = askStyle(styles) + 1;
	
	if styles[style] >= 0 then
		setStyle(styles[style]);
	end
end