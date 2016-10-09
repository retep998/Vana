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
-- Exiting the Free Market

map = getPlayerVariable("fm_origin", type_int);
portal = getPlayerVariable("fm_origin_portal");
if map == nil then
	map = 100000100;
	portal = "market00";
end

playPortalSe();
setMap(map, portal);
deletePlayerVariable("fm_origin");
deletePlayerVariable("fm_origin_portal");