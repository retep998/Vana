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
-- Portals in every job instructor room

if getLevel() <= 10 and getJob() == 0 then
	m = getMap();
	npcId = nil;

	if m == 120000101 then npcId = 1090000; -- Navigation Room, The Nautilus
	elseif m == 102000003 then npcId = 1022000; -- Warrior's Sanctuary
	elseif m == 103000003 then npcId = 1052001; -- Thieves' Hideout
	elseif m == 100000201 then npcId = 1012100; -- Bowman Instructional School
	elseif m == 101000003 then npcId = 1032001; -- Magic Library
	end

	if npcId ~= nil then
		runNpc(npcId);
	end
end