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
-- Door of Dimension

dofile("scripts/lua_functions/jobFunctions.lua");

function failtext()
	addText("There seems to be a door that leads me to the other dimension, but I can't go in for some reason.");
	sendNext();
end

tomap = 0;
cloneid = 0;
instance = "";

if getJobTrack() == 0 or getJobProgression() ~= 0 then
	-- They are either Beginners or non-second job
	failtext();
else
	m = getMap();
	g = getJobLine();
	if m == 105070001 and g == 1 then
		instance = "warrior3rd";
		tomap = 108010300;
		cloneid = 1022000;
	elseif m == 100040106 and g == 2 then
		instance = "magician3rd";
		tomap = 108010200;
		cloneid = 1032001;
	elseif m == 105040305 and g == 3 then
		instance = "bowman3rd";
		tomap = 108010100;
		cloneid = 1012100;
	elseif m == 107000402 and g == 4 then
		instance = "thief3rd";
		tomap = 108010400;
		cloneid = 1052001;
	elseif m == 105070200 and g == 5 then
		instance = "pirate3rd";
		tomap = 108010500;
		cloneid = 1090000;
	else
		failtext();
	end
end

if tomap ~= 0 then
	if not isInstance(instance) then
		createInstance(instance, 20 * 60, true);
		addInstancePlayer(getID());
		setMap(tomap);
	else
		addText("Someone is already fighting with #b#p" .. cloneid .. "##k's clone. Try again later.");
		sendNext();
	end
end