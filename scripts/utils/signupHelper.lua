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
-- Various functions to make coding sign up scripts much easier

dofile("scripts/utils/jsonHelper.lua");
dofile("scripts/utils/tableHelper.lua");

function verifyMaster(instanceName)
	return verifyInstance(instanceName) and isSignupMaster();
end

function verifyInstance(instanceName)
	-- We make sure that the person didn't keep open the window for just long enough
	-- to screw things up using this
	return isInstance(instanceName);
end

function compatibilityCheck(instanceName)
	if setInstance(instanceName) then
		gm = isGm();
		gmInstance = getInstanceVariable("gm_instance", type_bool);
		revertInstance();
		return gm == gmInstance;
	end
	return false;
end

function initSignup(maxSignups)
	local signups = {};
	append(signups, {getName(), getId()});
	for i = 2, maxSignups do
		append(signups, {"", 0});
	end

	local serializeState, serializedSignups = serialize(signups);
	local serializeState, serializedBanned = serialize({});

	setInstanceVariable("max_signup", maxSignups);
	setInstanceVariable("signup_count", 1);
	setInstanceVariable("signup_list", serializedSignups);
	setInstanceVariable("ban_count", 0);
	setInstanceVariable("banned_list", serializedBanned);
	setInstanceVariable("master_id", getId());
	setInstanceVariable("master_name", getName());
	setInstanceVariable("gm_instance", isGm());
	setInstanceVariable("entered", false);
end

function leaderEnterInstance()
	setInstanceVariable("entered", true);

	local signups = getFullSignupList();
	performAction(signups, function(i, playerPair)
		local name, id = playerPair[1], playerPair[2];
		if #name > 0 and setPlayer(id) then
			showMessage("The leader of the squad has entered the map. Please enter the map before time runs out on the squad.", msg_red);
			revertPlayer();
		end
	end);
end

function isSignupMaster()
	return getId() == getInstanceVariable("master_id", type_int);
end

function getMasterId()
	return getInstanceVariable("master_id", type_int);
end

function getBannedCount()
	return getInstanceVariable("ban_count", type_int);
end

function getSignedUpCount()
	return getInstanceVariable("signup_count", type_int);
end

function getMaxSignups()
	return getInstanceVariable("max_signup", type_int);
end

function isEntered()
	return getInstanceVariable("entered", type_bool);
end

function getSignupList()
	return transform(getFullSignupList(), transform_type_array, function(idx, key, value)
		return value[1];
	end);
end

function getBannedList()
	return transform(getFullBannedList(), transform_type_array, function(idx, key, value)
		return value[1];
	end);
end

function getFullSignupList()
	local serializeStatus, list = deserialize(getInstanceVariable("signup_list", type_string));
	return list;
end

function getFullBannedList()
	local serializeStatus, list = deserialize(getInstanceVariable("banned_list", type_string));
	return list;
end

function isListFull()
	return getSignedUpCount() >= getMaxSignups();
end

function isSignedUpPlayer(playerName)
	return findPlayerByName(getFullSignupList(), playerName)[1];
end

function isBannedPlayer(playerName)
	return findPlayerByName(getFullBannedList(), playerName)[1];
end

function findPlayerByName(list, playerName)
	return findValue(list, function(value)
		return value[1] == playerName;
	end);
end

function banPlayer(playerName)
	local signupList = getFullSignupList();
	local bannedList = getFullBannedList();

	local foundPair = findPlayerByName(signupList, playerName);
	local found, foundKey = foundPair[1], foundPair[2];

	if not found then
		-- Not sure how this would be, but bail out
		return;
	end

	local value = signupList[foundKey];
	table.remove(signupList, foundKey);
	append(signupList, {"", 0});

	append(bannedList, value);

	local serializeStatus, signupList = serialize(signupList);
	local serializeStatus, bannedList = serialize(bannedList);

	setInstanceVariable("signup_count", getSignedUpCount() - 1);
	setInstanceVariable("signup_list", signupList);
	setInstanceVariable("ban_count", getBannedCount() + 1);
	setInstanceVariable("banned_list", bannedList);
end

function unbanPlayer(playerName)
	local bannedList = getFullBannedList();

	local foundPair = findPlayerByName(bannedList, playerName);
	local found, foundKey = foundPair[1], foundPair[2];

	if not found then
		-- Not sure how this would be, but bail out
		return;
	end

	table.remove(bannedList, foundKey);
	local serializeStatus, bannedList = serialize(bannedList);

	setInstanceVariable("ban_count", getBannedCount() - 1);
	setInstanceVariable("banned_list", bannedList);
end

function addPlayerSignup(playerName, playerId)
	local signupList = getFullSignupList();

	local foundPair = findPlayerByName(signupList, playerName);
	local found, foundKey = foundPair[1], foundPair[2];

	if found then
		-- Not sure how this would be, but bail out
		return;
	end

	local index = getSignedUpCount() + 1;
	signupList[index] = {playerName, playerId};

	local serializeStatus, signupList = serialize(signupList);

	setInstanceVariable("signup_count", index);
	setInstanceVariable("signup_list", signupList);
end

function removePlayerSignup(playerName)
	local signupList = getFullSignupList();

	local foundPair = findPlayerByName(signupList, playerName);
	local found, foundKey = foundPair[1], foundPair[2];

	if not found then
		-- Not sure how this would be, but bail out
		return;
	end

	table.remove(signupList, foundKey);
	append(signupList, {"", 0});

	local serializeStatus, signupList = serialize(signupList);

	setInstanceVariable("signup_count", getSignedUpCount() - 1);
	setInstanceVariable("signup_list", signupList);
end