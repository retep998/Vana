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
-- Cloud Balcony, Night Lord quest

if getJob() == 412 then
	map = false;
	if getItemAmount(4001110) > 0 and isQuestInactive(6230) then
		map = true;
	elseif isQuestActive(6230) then
		map = true;
	elseif isQuestCompleted(6230) and isQuestInactive(6231) then
		map = true;
	end
	if map then
		playPortalSE();
		setMap(922020200);
	end
end