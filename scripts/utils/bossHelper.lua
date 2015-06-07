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
-- Common code to determine how many times a player has been into a boss each day

local seconds_per_day = 60 * 60 * 24;

function enterBoss(boss, maxEntries)
	if maxEntries == -1 then
		return true;
	end
	if maxEntries == 0 then
		return false;
	end

	local currentTime = getTime();
	local entered = false;
	for i = 1, maxEntries do
		local var = boss .. i;
		local entry = getPlayerVariable(var, type_int);
		if entry == nil or currentTime > (entry + seconds_per_day) then
			setPlayerVariable(var, currentTime);
			entered = true;
			break;
		end
	end
	return entered;
end

function getEntryCount(boss, maxEntries)
	if maxEntries == -1 then
		return 32767;
	end
	if maxEntries == 0 then
		return 0;
	end

	local currentTime = getTime();
	local count = 0;
	for i = 1, maxEntries do
		local var = boss .. i;
		local entry = getPlayerVariable(var, type_int);
		if entry ~= nil and currentTime < (entry + seconds_per_day) then
			count = count + 1;
		end
	end
	return count;
end

function getChannelString(channels)
	local s = "";
	if #channels == 1 then
		s = "channel " .. channels[1];
	elseif #channels == 2 then
		s = "channels " .. channels[1] .. " and " .. channels[2];
	elseif #channels > 0 then
		s = "channels ";
		local maxChannels = #channels - 1;
		for i = 1, maxChannels do
			s = s .. channels[i] .. ", ";
		end
		s = s .. "and " .. channels[#channels];
	else
		s = "no channels";
	end
	return s;
end

function timeString(times)
	if times == 1 then
		return "time";
	end
	return "times";
end