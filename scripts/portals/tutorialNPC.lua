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
-- Portals in every job instructor room

if getLevel() <= 10 and getJob() == 0 then
	m = getMap();
	npcid = 0;

	if m == 120000101 then -- Navigation Room, The Nautilus
		npcid = 1072008; -- Maybe 1090000?
	elseif m == 102000003 then -- Warrior's Sanctuary
		npcid = 1022000;
	elseif m == 103000003 then -- Thieves' Hideout
		npcid = 1052001;
	elseif m == 100000201 then -- Bowman Instructional School
		npcid = 1012100;
	elseif m == 101000003 then -- Magic Library
		npcid = 1032001;
	end

	if npcid ~= 0 then
		runNPC(npcid);
	end
end