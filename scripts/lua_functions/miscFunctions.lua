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

function append(tbl, elem)
	tbl[#tbl + 1] = elem;
end

function prepend(tbl, elem)
	table.insert(tbl, 1, elem);
end

function selectElement(tbl)
	return tbl[getRandomNumber(#tbl)];
end

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

serialize_ok = 1;
serialize_err = 2;
function serialize(val)
	_serializers = {
		["table"] = function(val)
			-- Determine if it's a plain old array-style table or one that requires a JSON dictionary
			local isArray = true;
			local maxKey = nil;
			local stringKeys = {};
			local numberKeys = {};

			for key, value in pairs(val) do
				if type(key) ~= "number" then
					isArray = false;
					append(stringKeys, key);
				elseif key < 0 or key >= math.huge then
					isArray = false;
					append(numberKeys, key);
				elseif maxKey == nil or maxKey < key then
					maxKey = key;
					append(numberKeys, key);
				end
			end

			if isArray then
				if maxKey == nil then
					-- Empty table
					return serialize_ok, "{}";
				end

				local returnText = "[";
				local anyKeys = false;
				for i = 1, maxKey do
					local key, value = i, val[i];
					local valueSerializer = _serializers[type(value)];
					if valueSerializer == nil then
						return serialize_err, "value " .. value .. " has no valid serializer";
					end
					local ret, valueText = valueSerializer(value);
					if ret == serialize_err then
						return serialize_err, valueText;
					end

					if anyKeys then
						returnText = returnText .. ", ";
					end
					returnText = returnText .. valueText;
					anyKeys = true;
				end

				returnText = returnText .. "]";
				return serialize_ok, returnText;			
			else
				local finalizedPairs = val;
				if #numberKeys > 0 then
					finalizedPairs = {};
					for _, key in ipairs(stringKeys) do
						finalizedPairs[key] = val[key];
					end
					for _, key in ipairs(numberKeys) do
						local converted = tostring(key);
						if stringKeys[converted] ~= nil then
							return serialize_err, "Integer key and string key collide";
						end
						append(stringKeys, converted);
						finalizedPairs[converted] = val[key];
					end
				end

				table.sort(stringKeys);
				local returnText = "{";
				local anyKeys = false;
				for _, key in ipairs(stringKeys) do
					local value = finalizedPairs[key];
					local keySerializer = _serializers[type(key)];
					local valueSerializer = _serializers[type(value)];
					if keySerializer == nil then
						return serialize_err, "key " .. key .. " has no valid serializer";
					end
					if valueSerializer == nil then
						return serialize_err, "value " .. value .. " has no valid serializer";
					end
					local ret, keyText = keySerializer(key);
					if ret == serialize_err then
						return serialize_err, keyText;
					end
					local ret, valueText = valueSerializer(value);
					if ret == serialize_err then
						return serialize_err, valueText;
					end

					if anyKeys then
						returnText = returnText .. ", ";
					end
					returnText = returnText .. keyText .. ": ";
					returnText = returnText .. valueText;
					anyKeys = true;
				end

				returnText = returnText .. "}";
				return serialize_ok, returnText;
			end

			return serialize_err, "Control flow should not ever hit this point";
		end,
		["number"] = function(val)
			return serialize_ok, tostring(val);
		end,
		["boolean"] = function(val)
			if val then
				return serialize_ok, "true";
			end
			return serialize_ok, "false";
		end,
		["string"] = function(val)
			-- Need to escape JSON-backslashes so they get reproduced accurately on the deserialize side
			local function escape(c)
				if c == "\n" then return "\\n"; end
				if c == "\r" then return "\\r"; end
				if c == "\t" then return "\\t"; end
				if c == "\b" then return "\\b"; end
				if c == "\f" then return "\\f"; end
				if c == "\v" then return "\\v"; end
				if c == "\"" then return "\\\""; end
				if c == "\\" then return "\\\\"; end
				return string.format("\\u%04x", c:byte());
			end

			local escapeChars = "[\"%\\%z\001-\031]";
			local modified = val:gsub(escapeChars, escape);
			return serialize_ok, "\"" .. modified .. "\"";
		end,
		["nil"] = function(val)
			return serialize_ok, "null";
		end,
	};

	local serializer = _serializers[type(val)];
	if serializer == nil then
		return serialize_err, "val has no valid serializer";
	end

	return serializer(val);
end

function deserialize(valString)
	if type(valString) ~= "string" then
		return valString;
	end
	if #valString == 0 then
		return nil;
	end

	-- Need this for closure capability
	local getNext = nil;
	local function eatWhitespace(text, start)
		local beginPos, endPos = text:find("^[ \n\r\t]+", start);
		if endPos ~= nil then
			return endPos + 1;
		end
		return start;
	end

	local function arrayDeserializer(text, start)
		-- Skip one character for the [
		start = eatWhitespace(text, start + 1);
		if text:sub(start, start) == "]" then
			return serialize_ok, {}, start + 1;
		end

		local i = start;
		local returnVal = {};
		local elementCount = 1;
		local ended = false;

		while i <= #text do
			local code, value, modifier = getNext(text, i);
			if code == serialize_err then
				return code, value;
			end

			if value ~= nil then
				returnVal[elementCount] = value;
			end
			elementCount = elementCount + 1;

			i = eatWhitespace(text, modifier);
			if text:sub(i, i) == "]" then
				ended = true;
				break;
			end
			if text:sub(i, i) ~= "," then
				return serialize_err, "Invalid JSON array";
			end
			i = i + 1;
		end

		if not ended then
			return serialize_err, "Unterminated array literal";
		end

		return serialize_ok, returnVal, i + 1;
	end

	local function objectDeserializer(text, start)
		-- Skip one character for the {
		start = eatWhitespace(text, start + 1);
		if text:sub(start, start) == "}" then
			return serialize_ok, {}, start + 1;
		end

		local i = start;
		local returnVal = {};
		local ended = false;

		while i <= #text do
			local code, key, modifier = getNext(text, i);
			if code == serialize_err then
				return code, key;
			end
			i = eatWhitespace(text, modifier);

			if text:sub(i, i) ~= ":" then
				return serialize_err, "Invalid JSON object";
			end
			i = eatWhitespace(text, i + 1);

			local code, value, modifier = getNext(text, i);
			if code == serialize_err then
				return code, value;
			end
			i = eatWhitespace(text, modifier);

			if value ~= nil then
				returnVal[key] = value;
			end

			if text:sub(i, i) == "}" then
				ended = true;
				break;
			end
			if text:sub(i, i) ~= "," then
				return serialize_err, "Invalid JSON object";
			end
			i = i + 1;
		end

		if not ended then
			return serialize_err, "Unterminated object literal";
		end

		return serialize_ok, returnVal, i + 1;
	end

	local function numberDeserializer(text, start)
		-- Unfortunately, numbers can be quite complex in JSON
		local mainNumber = text:match("^-?[1-9]%d*", start) or text:match("^-?0", start);
		if not mainNumber then
			return serialize_err, "Invalid JSON number";
		end
		local numLength = #mainNumber;

		local decimal = text:match("^%.%d+", start + numLength) or "";
		numLength = numLength + #decimal;
		local exponent = text:match("^[eE][-+]?%d+", start + numLength) or "";
		numLength = numLength + #exponent;

		local expandedNumber = mainNumber .. decimal .. exponent;
		local returnVal = tonumber(expandedNumber);

		if not returnVal then
			return serialize_err, "Invalid Lua number: " .. expandedNumber;
		end

		return serialize_ok, returnVal, start + numLength;
	end

	local function stringDeserializer(text, start)
		-- Account for " character
		start = start + 1;
		local i = start;
		local ended = false;
		local returnValue = "";

		while i <= #text do
			local c = text:sub(i, i);
			if c == "\"" then
				ended = true;
				break;
			end
			if c ~= "\\" then
				returnValue = returnValue .. c;
			else
				if i == #text then
					break;
				end
				i = i + 1;
				local escaped = text:sub(i, i);
				if escaped == "f" then returnValue = returnValue .. "\f";
				elseif escaped == "n" then returnValue = returnValue .. "\n";
				elseif escaped == "b" then returnValue = returnValue .. "\b";
				elseif escaped == "r" then returnValue = returnValue .. "\r";
				elseif escaped == "t" then returnValue = returnValue .. "\t";
				elseif escaped == "v" then returnValue = returnValue .. "\v";
				elseif escaped == "\\" then returnValue = returnValue .. "\\";
				elseif escaped == "\"" then returnValue = returnValue .. "\"";
				elseif escaped == "\'" then returnValue = returnValue .. "'";
				elseif escaped == "u" then
					if i == #text then
						break;
					end
					local hex = text:match("^([0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])", i + 1);
					if not hex then
						return serialize_err, "Invalid JSON Unicode escape";
					end
					i = i + 4

					local codepoint = tonumber(hex, 16);
					if codepoint >= 0xD800 and codepoint <= 0xDBFF then
						-- High surrogate
						-- Here we need to get the corresponding low surrogate
						local lowSurrogate = text:match("^\\u([dD][c-fC-F][0-9a-fA-F][0-9a-fA-F])", i + 1);
						if not lowSurrogate then
							return serialize_err, "Invalid Unicode low surrogate following a high surrogate";
						end
						i = i + 6;
						codepoint = 0x2400 + ((codepoint - 0xD800) * tonumber(lowSurrogate, 16));
					end

					if codepoint <= 127 then
						returnValue = returnValue .. string.char(codepoint);
					elseif codepoint <= 2047 then
						local firstByte = math.floor(codepoint / 0x40);
						local lastByte = codepoint - (0x40 * firstByte);
						returnValue = returnValue .. string.char(0xC0 + firstByte, 0x80 + lastByte);
					elseif codepoint <= 65535 then
						local firstByte = math.floor(codepoint / 0x1000);
						local remaining = codepoint - (0x1000 * firstByte);
						local secondByte = math.floor(remaining / 0x40);
						local lastByte = remaining - (0x40 * secondByte);
						returnValue = returnValue .. string.char(0xE0 + firstByte, 0x80 + secondByte, 0x80 + lastByte);
					else
						local firstByte = math.floor(codepoint / 0x40000);
						local remaining = codepoint - (0x40000 * firstByte);
						local secondByte = math.floor(remaining / 0x1000);
						remaining = remaining - (0x1000 * secondByte);
						local thirdByte = math.floor(remaining / 0x40);
						local lastByte = remaining - (0x40 * thirdByte);
						returnValue = returnValue .. string.char(0xF0 + firstByte, 0x80 + secondByte, 0x80 + thirdByte, 0x80 + lastByte);
					end
				elseif escaped == "x" then
					if i == #text then
						break;
					end
					local hex = text:match("^([0-9a-fA-F][0-9a-fA-F])", i + 1);
					if not hex then
						return serialize_err, "Invalid JSON ASCII escape";
					end
					returnValue = returnValue .. string.char(tonumber(hex, 16));
					i = i + 2
				elseif escaped:match("^[0-8]") then
					local oct = text:match("^([0-8][0-8]?[0-8]?)", i);
					if not oct then
						return serialize_err, "Invalid JSON octal escape";
					end
					returnValue = returnValue .. string.char(tonumber(oct, 8));
					i = i + #oct - 1
				else
					return serialize_err, "Invalid JSON string";
				end
			end
			i = i + 1;
		end

		if not ended then
			return serialize_err, "Unterminated JSON string";
		end

		return serialize_ok, returnValue, i + 1;
	end

	local function nullDeserializer(text, start)
		return serialize_ok, nil, start + 4;
	end

	local function trueDeserializer(text, start)
		return serialize_ok, true, start + 4;
	end

	local function falseDeserializer(text, start)
		return serialize_ok, false, start + 5;
	end

	getNext = function(text, start)
		start = eatWhitespace(text, start);
		if start > #text then
			return serialize_err, "String ended prematurely";
		end

		if text:find("^\"", start) then return stringDeserializer(text, start); end
		if text:find("^[-0123456789 ]", start) then return numberDeserializer(text, start); end
		if text:find("^%{", start) then return objectDeserializer(text, start); end
		if text:find("^%[", start) then return arrayDeserializer(text, start); end
		if text:find("^true", start) then return trueDeserializer(text, start); end
		if text:find("^false", start) then return falseDeserializer(text, start); end
		if text:find("^null", start) then return nullDeserializer(text, start); end
		return serialize_err, "Invalid JSON";
	end

	local code, val, pos = getNext(valString, 1);
	return code, val;
end