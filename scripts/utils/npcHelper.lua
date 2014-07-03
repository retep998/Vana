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

-- Other commands that may or may not do anything:
-- #x
-- #i<itemId>#

npc_text_black = 0;
npc_text_blue = 1;
npc_text_red = 2;
npc_text_green = 3;
npc_text_purple = 4;

npc_text_normal = 0;
npc_text_bold = 1;

function processTextColor(text, previous, directive)
	returnText = directive;
	if text ~= nil then
		returnText = returnText .. text;
	end
	if previous ~= nil then
		if previous == npc_text_blue then returnText = returnText .. "#b";
		elseif previous == npc_text_red then returnText = returnText .. "#r";
		elseif previous == npc_text_green then returnText = returnText .. "#g";
		elseif previous == npc_text_purple then returnText = returnText .. "#d";
		else returnText = returnText .. "#k";
		end
	end
	return returnText;
end

function processTextQuality(text, previous, directive)
	returnText = directive;
	if text ~= nil then
		returnText = returnText .. text;
	end
	if previous ~= nil then
		if previous == npc_text_bold then returnText = returnText .. "#e";
		else returnText = returnText .. "#n";
		end
	end
	return returnText;
end

function black(text, previousColor)
	return processTextColor(text, previousColor, "#k");
end

function red(text, previousColor)
	if previousColor == nil then
		previousColor = npc_text_black;
	end
	return processTextColor(text, previousColor, "#r");
end

function blue(text, previousColor)
	if previousColor == nil then
		previousColor = npc_text_black;
	end
	return processTextColor(text, previousColor, "#b");
end

function green(text, previousColor)
	if previousColor == nil then
		previousColor = npc_text_black;
	end
	return processTextColor(text, previousColor, "#g");
end

function purple(text, previousColor)
	if previousColor == nil then
		previousColor = npc_text_black;
	end
	return processTextColor(text, previousColor, "#d");
end

function bold(text, previousQuality)
	if previousQuality == nil then
		previousQuality = npc_text_normal;
	end
	return processTextQuality(text, previousQuality, "#e");
end

function normal(text, previousQuality)
	return processTextQuality(text, previousQuality, "#n");
end

function fileRef(text)
	return "#f" .. text .. "#";
end

function itemIcon(itemId)
	return "#v" .. itemId .. "#";
end

function mobRef(mobId)
	return "#o" .. mobId .. "#";
end

function mapRef(mapId)
	return "#m" .. mapId .. "#";
end

function inventoryRef(itemId)
	return "#c" .. itemId .. "#";
end

function skillRef(skillId)
	return "#q" .. skillId .. "#";
end

-- Not sure what this is for, but it appears to be used by some scripts
function altItemRef(itemId)
	return "#z" .. itemId .. "#";
end

function itemRef(itemId)
	return "#t" .. itemId .. "#";
end

function playerRef()
	return "#h #";
end

function npcRef(npcId)
	return "#p" .. npcId .. "#";
end

function questCompleteIcon()
	return fileRef("UI/UIWindow.img/QuestIcon/4/0");
end

function questExpIcon(experience)
	return fileRef("UI/UIWindow.img/QuestIcon/8/0") .. " " .. experience .. " EXP";
end

function questMesosIcon(mesos)
	return itemIcon(4031138) .. " " .. mesos .. " mesos";
end

localeSupportsGenderedText = false;
function genderedText(female, male)
	if localeSupportsGenderedText then
		return "#G" .. male .. ":" .. female .. "#";
	end
	if getGender() == gender_male then
		return male;
	end
	return female;
end

function questItemIcon(itemId, qty)
	if qty == nil or qty == 1 then
		return itemIcon(itemId) .. " " .. itemRef(itemId);
	end
	return itemIcon(itemId) .. " " .. qty .. " " .. itemRef(itemId) .. "s";
end

function questSkillIcon(skillId)
	return "#s" .. skillId .. "#";
end

function progressBar(completionPercentage)
	return "#B" .. completionPercentage .. "#";
end

function choiceRef(choice, choiceId)
	if type(choice) ~= "table" then
		if choiceId == nil then
			choiceId = 0;
		end
		return "#L" .. choiceId .. "#" .. choice .. "#l";
	end

	local text = "";
	for i = 1, #choice do
		if i > 1 then
			text = text .. "\r\n";
		end
		local element = choice[i];
		-- This is really important for functions and data being able to be associated to an option
		-- Essentially, we pull out the text for anything other than a pure text option
		if type(element) == "table" then
			element = element[1];
		end
		text = text .. "#L" .. (i - 1) .. "#" .. element .. "#l";
	end
	return text;
end

function makeChoiceData(choice, data)
	return {choice, data};
end

function makeChoiceHandler(choice, handler)
	return {choice, handler};
end

function selectChoice(choices, choiceId)
	choiceId = choiceId + 1;
	choice = choices[choiceId];
	if type(choice) == "table" then
		if type(choice[2]) == "function" then
			return choice[2](choiceId);
		end
		return choice[2], choiceId;
	end
	return choice;
end