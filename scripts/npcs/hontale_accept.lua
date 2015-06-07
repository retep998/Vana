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
-- Horntail Squad

dofile("scripts/utils/bossHelper.lua");
dofile("scripts/utils/npcHelper.lua");
dofile("scripts/utils/signupHelper.lua");
dofile("scripts/utils/tableHelper.lua");

signupInstance = "horntailSignup";

function enterBossMap()
	x = getMaxHorntailBattles();
	if enterBoss("Horntail", x) then
		setMap(240060000);
	else
		addText("You may only enter this place " .. x .. " " .. timeString(x) .. " per day.");
		sendOk();
	end
end

function isAtHead()
	if setInstance("horntail") then
		leftHead = getInstanceVariable("left_head", type_bool);
		revertInstance();
		if leftHead then
			return true;
		end
	end
	return false;
end

if not isHorntailChannel() then
	channels = getHorntailChannels();
	if #channels == 0 then
		addText("You may not summon Horntail at this time.");
	else
		addText("You can only summon Horntail at " .. getChannelString(channels) .. ".");
	end
	sendOk();
	return;
end

function displaySquadList()
	signedUp = getSignupList();
	signupCount = getSignedUpCount();
	addText("The total number of applicants are " .. blue(signupCount) .. ", and the list of the applicants is as follows.\r\n");
	for i = 1, #signedUp do
		player = signedUp[i];
		if i == 1 then
			addText(i .. " : " .. blue(player) .. " (The Leader of the Squad)");
		else
			addText(i .. " : " .. player);
		end
		addText("\r\n");
	end
	sendOk();
end

if getLevel() < 80 then
	addText("Only players with level 80 or above are qualified to join the Horntail Squad.");
	sendOk();
	return;
end

if verifyInstance(signupInstance) then
	if isBannedPlayer(getName()) or not compatibilityCheck(signupInstance) then
		addText("Your participation is formally rejected from the squad. ");
		addText("You may only participate with an approval from the leader of the squad.");
		sendOk();
		return;
	end
end

if not isInstance(signupInstance) then
	if not isInstance("horntail") then
		if getNumPlayers(240060000) > 0 then
			addText("Someone is already in the cave.");
			sendOk();
		else
			if isGm() or isPartyLeader() then
				addText("Would you like to become the leader of the Horntail Expedition Squad?");
				answer = askYesNo();

				if answer == answer_yes then
					if not isGm() and (not isPartyInLevelRange(80, 200) or getPartyMapCount() < 3) then
						addText("Only the leader of the party that consists of 3 or more members is eligible to become the leader of the Horntail Expedition Squad.");
						sendOk();
					else
						createInstance(signupInstance, 5 * 60, true);
						initSignup(30);
						showMapMessage(getName() .. " has been appointed the leader of the Horntail Expedition Squad. To those willing to participate in the Expedition Squad, APPLY NOW!", msg_blue);

						addText("You have been appointed the leader of the Horntail Expedition Squad. ");
						addText("You'll now have 5 minutes to form the squad and have every member enter the mission.");
						sendOk();
					end
				else
					addText("Talk to me if you want to become the leader of the squad.");
					sendOk();
				end
			else
				addText("Only the leader of the party with 3 or more members that are at least Level 80 is eligible to apply for the leader of the expedition squad.");
				sendOk();
			end
		end
	else
		addText("The battle against Horntail has already started.");
		sendOk();
	end
else
	if getName() == getInstanceVariable("master") then
		if isEntered() then
			if isAtHead() then
				addText("The battle has already begun.");
				sendOk();
			else
				enterBossMap();
			end
		else
			choices = {
				makeChoiceHandler(" Check out the list of the Squad", function()
					displaySquadList();
				end),
				makeChoiceHandler(" Expel a member from the Squad", function()
					if getSignedUpCount() == 1 then
						addText("No one has yet to sign up for the squad.");
						sendOk();
					else
						signedUp = getSignupList();
						choices = transform(slice(signedUp, 2), transform_type_array, function(idx, key, value)
							return makeChoiceData((key + 1) .. " : " .. value, {value});
						end);

						addText("Which of these members would you like to expel?\r\n");
						addText(choiceRef(choices));
						choice = askChoice();

						if not verifyMaster(signupInstance) then
							return;
						end

						data = selectChoice(choices, choice);
						player = data[1];
						addText("Are you sure you want to enter " .. blue(name) .. " in the Suspended List? ");
						addText("Once suspended, the user may not re-apply for a spot until the suspension is lifted by the leader of the squad.");
						answer = askYesNo();

						if not verifyMaster(signupInstance) then
							return;
						end

						if answer == answer_yes then
							banPlayer(player);
							if setPlayer(player) then
								showMessage("The leader of the squad has entered you in the squad's Suspended List.", msg_red);
								revertPlayer();
							end
						end
					end
				end),
				makeChoiceHandler(" Re-accept a member from the Suspended List", function()
					if getBannedInstancePlayerCount() > 0 then
						choices = getBannedList();
						addText("Whose application are you willing to accept?\r\n");
						addText(blue(choiceRef(choices)));
						choice = askChoice();

						if not verifyMaster(signupInstance) then
							return;
						end

						unbanPlayer(selectChoice(choices, choice));
					else
						addText("No user is currently in the Suspended List.");
						sendOk();
					end
				end),
				makeChoiceHandler(red(" Form the Squad and enter", npc_text_blue), function()
					if isGm() or getSignedUpCount() >= 6 then
						leaderEnterInstance();
						createInstance("horntail", 12 * 60 * 60, true);

						if isGm() and setInstance("horntail") then
							setInstanceVariable("gm", true);
							revertInstance();
						end

						enterBossMap();
					else
						addText("The squad needs to consist of 6 or more members in order to start the quest.");
						sendOk();
					end
				end),
			};

			addText("Greetings, leader of the Horntail Expedition Squad. ");
			addText("What would you like to do? \r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			if not verifyMaster(signupInstance) then
				return;
			end

			selectChoice(choices, choice);
		end
	else
		if isEntered() and not isAtHead() then
			if isSignedUpPlayer(getName()) then
				enterBossMap();
			else
				addText("You may not enter this premise if you are not a member of the Horntail Expedition Squad.");
				sendOk();
			end
		elseif isAtHead() then
			addText("The battle has already begun.");
			sendOk();
		else
			choices = {
				makeChoiceHandler(" Enter the Horntail Expedition Squad", function()
					if isListFull() then
						addText("Unable to apply for a spot due to number of applicants already reaching the maximum.");
						sendOk();
					elseif isSignedUpPlayer(getName()) then
						addText("You are already part of the expedition squad.");
						sendOk();
					elseif isEntered() then
						addText("The application process for the Horntail Expedition Squad had already been concluded.");
						sendOk();
					else
						playerName = getName();
						addPlayerSignup(playerName, getId());
						if setPlayer(getMasterId()) then
							showMessage(playerName .. " has joined the expedition squad.", msg_red);
							revertPlayer();
						end
						addText("You have been enrolled in the Horntail Expedition Squad.");
						sendOk();
					end
				end),
				makeChoiceHandler(" Leave the Horntail Expedition Squad", function()
					if isSignedUpPlayer(getName()) then
						playerName = getName();
						removePlayerSignup(playerName);
						if setPlayer(getMasterId()) then
							showMessage(playerName .. " has withdrawn from the squad.", msg_red);
							revertPlayer();
						end
						addText("You have formally withdrawn from the squad.");
						sendOk();
					else
						addText("Unable to leave the squad due to the fact that you're not participating in the Horntail Participation Squad.");
						sendOk();
					end
				end),
				makeChoiceHandler(" Check out the list of the Squad.", function()
					displaySquadList();
				end),
			};

			addText("What would you like to do?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			if not verifyInstance(signupInstance) then
				return;
			end

			selectChoice(choices, choice);
		end
	end
end