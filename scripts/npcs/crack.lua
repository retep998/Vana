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
-- Door of Dimension

dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/jobHelper.lua");

function failText()
	addText("There seems to be a door that leads me to the other dimension, but I can't go in for some reason.");
	sendNext();
end

destinationMap = nil;
cloneId = nil;
instance = nil;
questState = getPlayerVariable("third_job_advancement", type_int);

if getJobTrack() == 0 or getJobProgression() ~= progression_second or questState == nil or questState ~= 2 then
	-- Beginners, non-second job, not in the process of advancing, too far in the advancement
	failText();
elseif questState == 2 and (getItemAmount(4031059) > 0 or getItemAmount(4031057) > 0) then
	-- Right stage but has an item, don't want them to be able to hoard
	failText();
else
	m = getMap();
	jobLine = getJobLine();

	if m == 105070001 and jobLine == line_warrior then
		instance = "warrior3rd";
		destinationMap = 108010300;
		cloneId = 1022000;
	elseif m == 100040106 and jobLine == line_magician then
		instance = "magician3rd";
		destinationMap = 108010200;
		cloneId = 1032001;
	elseif m == 105040305 and jobLine == line_bowman then
		instance = "bowman3rd";
		destinationMap = 108010100;
		cloneId = 1012100;
	elseif m == 107000402 and jobLine == line_thief then
		instance = "thief3rd";
		destinationMap = 108010400;
		cloneId = 1052001;
	elseif m == 105070200 and jobLine == line_pirate then
		instance = "pirate3rd";
		destinationMap = 108010500;
		cloneId = 1090000;
	else
		failText();
	end
end

if destinationMap ~= nil then
	if not isInstance(instance) then
		createInstance(instance, 20 * 60, true);
		addInstancePlayer(getId());
		setMap(destinationMap);
	else
		addText("Someone is already fighting with " .. blue(npcRef(cloneId)) .. "'s clone. ");
		addText("Try again later.");
		sendNext();
	end
end