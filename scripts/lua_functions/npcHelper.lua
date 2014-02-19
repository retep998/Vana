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
-- A subset of Lua functions that helps with common NPC text patterns

function red(text)
	return "#r" .. text .. "#k";
end

function blue(text)
	return "#b" .. text .. "#k";
end

function fileRef(text)
	return "#f" .. text .. "#";
end

function iconRef(itemId)
	return "#v" .. itemId .. "#";
end

function mapRef(mapId)
	return "#m" .. mapId .. "#";
end

function itemRef(itemId)
	return "#t" .. itemId .. "#";
end

function npcRef(npcId)
	return "#p" .. npcId .. "#";
end

function questCompleteIcon()
	return fileRef("UI/UIWindow.img/QuestIcon/4/0");
end

function questExpIcon()
	return fileRef("UI/UIWindow.img/QuestIcon/8/0");
end

function questItemIcon(itemId, qty)
	return iconRef(itemId) .. " " .. qty .. " " .. itemRef(itemId);
end

function choiceRef(choice, choiceId)
	if choiceId == nil then
		choiceId = 0;
	end
	if type(choice) == "table" then
		choice = choice[1];
	end
	return "#L" .. choiceId .. "#" .. choice .. "#l";
end

function choiceList(choices)
	text = "";
	for i = 1, #choices do
		if i > 1 then
			text = text .. "\r\n";
		end
		text = text .. choiceRef(choices[i], i - 1);
	end
	return text;
end

function makeChoiceHandler(choice, handler)
	return {choice, handler};
end

function selectChoice(choices, choiceId)
	choiceId = choiceId + 1;
	choice = choices[choiceId];
	if type(choice) == "table" then
		return choice[2]();
	end
end