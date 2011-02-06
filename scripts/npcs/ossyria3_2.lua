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
-- El Nath Magic Spot (Orbis Tower: 1st Floor)

if getItemAmount(4001019) >= 1 then
	addText("You can use #b#t4001019##k to activate #b#p2012015##k. Will you teleport to where #b#p2012014##k is?");
	yes = askYesNo();

	if yes == 1 then
		giveItem(4001019, -1);
		setMap(200080200);
	end
else
	addText("There's a #b#p2012015##k that'll enable you to teleport to where #b#p2012014##k is, but you can't activate it without the scroll.");
	sendOK();
end
