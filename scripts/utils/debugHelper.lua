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
-- A subset of Lua functions that helps with debugging

function printElement(elem)
	_printers = {
		["table"] = function(val, indent)
			local text = "{";
			indent = indent .. "\t";
			local anyElement = false;
			for key, value in pairs(val) do
				local keyPrinter = _printers[type(key)];
				local valuePrinter = _printers[type(value)];
				if anyElement then
					text = text .. ",";
				end
				text = text .. "\n";
				text = text .. keyPrinter(key, indent);
				text = text .. " = ";
				if type(value) == "table" then
					text = text .. valuePrinter(value, indent);
				else
					text = text .. valuePrinter(value, "");
				end
				anyElement = true;
			end

			indent = indent:sub(1, #indent - 1);
			text = text .. "\n" .. indent .. "}";
			return text;
		end,
		["number"] = function(val, indent)
			return indent .. tostring(val);
		end,
		["boolean"] = function(val, indent)
			if val then
				return indent .. "true";
			end
			return indent .. "false";
		end,
		["string"] = function(val, indent)
			return indent .. val;
		end,
		["nil"] = function(val, indent)
			return indent .. "nil";
		end,
		["function"] = function(val, indent)
			return indent .. "function";
		end,
	};

	local printer = _printers[type(elem)];
	if printer ~= nil then
		print(printer(elem, ""));
	else
		print("NO TYPE AVAILABLE: " .. type(elem));
	end
end