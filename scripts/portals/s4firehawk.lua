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
-- Portal to pick up egg for Phoenix

if getItemAmount(4001113) > 0 then
	showMessage("You already have Phoenix's egg. You can't enter.", m_red);
else
	if (isQuestActive(6240) or isQuestActive(6241)) or (isQuestCompleted(6240) and isQuestInactive(6241)) then
		playPortalSE();
		setMap(921100200);
	end
end