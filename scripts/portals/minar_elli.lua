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
-- Ellinia <-> Leafre

if getItemAmount(4031346) > 0 then
	giveItem(4031346, -1);

	showMessage("The Magical Seed is spent and you are transferred to somewhere.", m_red);
	playPortalSE();

	if getMap() == 240010100 then
		setMap(101010000, "minar00");
	else
		setMap(240010100, "elli00");
	end
else
	showMessage("Magic Seed is needed to go through the portal.", m_red);
end
