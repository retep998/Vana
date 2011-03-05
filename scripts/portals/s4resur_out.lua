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
-- Portal to Bishop Resurrection quest - exiting the awful dark part

if isQuestActive(6134) then
	if not giveItem(4031448, 1) then
		showMessage("You can't proceed as you don't have an empty slot in your inventory. Please clear your inventory and try again.", m_red);
	else
		playPortalSE();
		setMap(220070400, "pt00");
	end
end