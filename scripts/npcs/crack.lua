--[[
Copyright (C) 2008-2013 Vana Development Team

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

function failText()
	addText("There seems to be a door that leads me to the other dimension, but I can't go in for some reason.");
	sendNext();
end

toMap = 0;
cloneId = 0;
instance = "";
questState = getPlayerVariable("third_job_advancement", true);

if getJobTrack() == 0 or getJobProgression() ~= 0 or questState == nil or questState ~= 2 then
	-- Beginners, non-second job, not in the process of advancing, too far in the advancement
	failText();
elseif questState == 2 and (getItemAmount(4031059) > 0 or getItemAmount(4031057) > 0) then
	-- Right stage but has an item, don't want them to be able to hoard
	failText();
else
	m = getMap();
	g = getJobLine();

	if m == 105070001 and g == 1 then
		instance = "warrior3rd";
		toMap = 108010300;
		cloneId = 1022000;
	elseif m == 100040106 and g == 2 then
		instance = "magician3rd";
		toMap = 108010200;
		cloneId = 1032001;
	elseif m == 105040305 and g == 3 then
		instance = "bowman3rd";
		toMap = 108010100;
		cloneId = 1012100;
	elseif m == 107000402 and g == 4 then
		instance = "thief3rd";
		toMap = 108010400;
		cloneId = 1052001;
	elseif m == 105070200 and g == 5 then
		instance = "pirate3rd";
		toMap = 108010500;
		cloneId = 1090000;
	else
		failText();
	end
end

if toMap ~= 0 then
	if not isInstance(instance) then
		createInstance(instance, 20 * 60, true);
		addInstancePlayer(getId());
		setMap(toMap);
	else
		addText("Someone is already fighting with #b#p" .. cloneId .. "##k's clone. Try again later.");
		sendNext();
	end
end