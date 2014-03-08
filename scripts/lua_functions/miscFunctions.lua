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
-- A subset of Lua functions that helps with miscellaneous tasks

function getOrdinal(n)
	n = tonumber(n);
	local ending = "th";
	local digit = n % 10;
	if digit < 10 or digit > 14 then
		if digit == 1 then ending = "st";
		elseif digit == 2 then ending = "nd";
		elseif digit == 3 then ending = "rd";
		end
	end
	return ending;
end

function append(tbl, elem)
	tbl[#tbl + 1] = elem;
end

function prepend(tbl, elem)
	table.insert(tbl, 1, elem);
end

function selectElement(tbl)
	return tbl[getRandomNumber(#tbl)];
end