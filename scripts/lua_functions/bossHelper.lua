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
-- Common code to determine how many times a player has been into a boss each day

secondsinday = 60 * 60 * 24;

function enterBoss(boss, maxentries)
	if maxentries == -1 then
		return true;
	end
	if maxentries == 0 then
		return false;
	end

	time = getTime();
	entered = false;
	for i = 1, maxentries do
		var = boss .. i;
		entry = getPlayerVariable(var, true);
		if entry == nil or time > (entry + secondsinday) then
			setPlayerVariable(var, time);
			entered = true;
			break;
		end
	end
	return entered;
end

function getEntryCount(boss, maxentries)
	if maxentries == -1 then
		return 32767;
	end
	if maxentries == 0 then
		return 0;
	end

	time = getTime();
	count = 0;
	for i = 1, maxentries do
		var = boss .. i;
		entry = getPlayerVariable(var, true);
		if entry ~= nil and time < (entry + secondsinday) then
			count = count + 1;
		end
	end
	return count;
end

function getChannelString(channels)
	s = "no channels";
	if #channels == 1 then
		s = "channel " .. channels[1];
	elseif #channels == 2 then
		s = "channels " .. channels[1] .. " and " .. channels[2];
	elseif #channels > 0 then
		s = "channels ";
		max = #channels - 1;
		for i = 1, max do
			s = s .. channels[i] .. ", ";
		end
		s = s .. "and " .. channels[#channels];
	end
	return s;
end

function timeString(times)
	if times == 1 then
		return "time";
	end
	return "times";
end