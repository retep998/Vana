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
-- A subset of Lua functions that helps common party quest tasks

function getRandomFootholds(desiredamount, tablefootholds)
	i = 0;
	correctfootholds = {};
	while i < desiredamount do
		n = getRandomNumber(#tablefootholds);
		local add = true;
		if correctfootholds ~= nil then
			for v = 1, #correctfootholds do
				if correctfootholds[v] == n then
					add = false;
					break;
				end
			end
		end
		if add then
			i = i + 1;
			correctfootholds[i] = n;
		end
	end
	returnfootholds = {};
	for m = 1, desiredamount do
		returnfootholds[m] = tablefootholds[correctfootholds[m]];
	end
	return returnfootholds;
end