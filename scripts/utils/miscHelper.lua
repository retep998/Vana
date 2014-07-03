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
	local digit = n % 10;
	local tensDigit = n % 100;
	if tensDigit >= 10 and tensDigit <= 13 then return "th"; end
	if digit == 1 then return "st"; end
	if digit == 2 then return "nd"; end
	if digit == 3 then return "rd"; end
	return "th";
end

function separateDigits(num)
	local left, num, right = string.match(num, "^([^%d]*%d)(%d*)(.-)$");
	return left .. (num:reverse():gsub("(%d%d%d)", "%1,"):reverse()) .. right;
end