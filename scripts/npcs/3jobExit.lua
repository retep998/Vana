--[[
Copyright (C) 2008-2014 Vana Development Team

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
-- Sparkling Crystal

addText("You can use the Sparkling Crystal to go back to the real world. Are you sure you want to go back?");
yes = askYesNo();

if yes == 1 then
	toMap = 0;
	m = getMap();
	if m == 108010101 then toMap = 100000000;
	elseif m == 108010201 then toMap = 101000000;
	elseif m == 108010301 then toMap = 102000000;
	elseif m == 108010401 then toMap = 103000000;
	elseif m == 108010501 then toMap = 120000000;
	end
	setMap(toMap);
end