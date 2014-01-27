--[[
Copyright (C) 2008-2009 Vana Development Team

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
-- Portal inside Horntail battles

map = 0;

if getMap() == 240060000 then
	if isGm() or (getInstanceVariable("lefthead") and countMobs(getMap(), 8810000) == 0) then
		map = 240060100;
	end
else
	if isGm() or (getInstanceVariable("righthead") and countMobs(getMap(), 8810001) == 0) then
		map = 240060200;
	end
end

if map == 0 then
	showMessage("The portal doesn't work now.", env_redMessage);
else
	playPortalSe();
	setMap(map);
end