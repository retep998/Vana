--[[
Copyright (C) 2008-2015 Vana Development Team

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
-- A subset of Lua functions that help with table manipulation

function append(tbl, elem)
	tbl[#tbl + 1] = elem;
end

function prepend(tbl, elem)
	table.insert(tbl, 1, elem);
end

function selectElement(tbl)
	return tbl[getRandomNumber(#tbl)];
end

function merge(...)
	local args = {...};
	local result = {};
	for i = 1, #args do
		local tbl = args[i];
		if type(tbl) == "table" then
			for j = 1, #tbl do
				append(result, tbl[j]);
			end
		else
			append(result, tbl);
		end
	end
	return result;
end

function slice(tbl, startIndex, endIndex)
	local result = {};
	if startIndex == nil then
		startIndex = 1;
	end
	if endIndex == nil then
		endIndex = #tbl;
	end
	for i = startIndex, endIndex do
		append(result, tbl[i]);
	end
	return result;
end

transform_type_kvp = 1;
transform_type_array = 2;
function transform(tbl, transformType, func)
	local result = {};
	local index = 1;
	for key, value in ipairs(tbl) do
		local value = func(index, key, value);
		if transformType == transform_type_kvp then
			if value["key"] ~= nil then
				result[value["key"]] = value["value"];
			else
				result[value[1]] = value[2];
			end
		elseif transformType == transform_type_array then
			append(result, value);
		end
		index = index + 1;
	end
	return result;
end

function findValue(tbl, needle)
	local found = false;
	local foundKey = nil;
	if type(needle) == "function" then
		for key, value in ipairs(tbl) do
			if needle(value) then
				found = true;
				foundKey = key;
				break;
			end
		end
	else
		for key, value in ipairs(tbl) do
			if value == needle then
				found = true;
				foundKey = key;
				break;
			end
		end
	end
	return {found, foundKey};
end

function performAction(tbl, action)
	for key, value in ipairs(tbl) do
		action(key, value);
	end
end