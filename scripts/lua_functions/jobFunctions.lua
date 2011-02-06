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
-- A subset of Lua functions that helps with common job-related tasks

function getJobLine()
	local g = getJob();
	g = (g - (g % 100)) / 100;
	return g;
end

function getJobTrack()
	local g = getJob();
	g = ((g - (g % 10)) / 10) % 10;
	return g;
end

function getJobProgression()
	local g = getJob() % 10;
	return g;
end

function isCygnus()
	return getJob() >= 1000;
end
