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
-- Horntail Squad

dofile("scripts/lua_functions/bossHelper.lua");
dofile("scripts/lua_functions/npcHelper.lua");
dofile("scripts/lua_functions/signupHelper.lua");

function enterBossMap()
	x = getMaxHorntailBattles();
	if enterBoss("Horntail", x) then
		setMap(240060000);
	else
		addText("You may only enter this place " .. x .. " " .. timeString(x) .. " per day.");
		sendOk();
	end
end

function verifyMaster()
	if not verifyInstance() or getName() ~= getInstanceVariable("master") then
		return false;
	end
	return true;
end

function verifyInstance()
	-- We make sure that the person didn't keep open the window for just long enough
	-- to screw things up using this
	return isInstance("horntailSignup");
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

function compatibilityCheck()
	if setInstance("horntailSignup") then
		gm = isGm();
		gmInstance = getInstanceVariable("gm", type_bool);
		revertInstance();
		return gm == gmInstance;
	end
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

if getLevel() < 80 then
	addText("Only players with level 80 or above are qualified to join the Horntail Squad.");
	sendOk();
	return;
end

if verifyInstance() then
	if isBannedInstancePlayer(getName()) or not compatibilityCheck() then
		addText("Your participation is formally rejected from the squad. ");
		addText("You may only participate with an approval from the leader of the squad.");
		sendOk();
		return;
	end
end

if not isInstance("horntailSignup") then
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
					else
						createInstance("horntailSignup", 5 * 60, true);
						addPlayerSignUp(getName());
						setInstanceVariable("master", getName());
						setInstanceVariable("gm", isGm());
						showMapMessage(getName() .. " has been appointed the leader of the Zakum Expedition Squad. To those willing to participate in the Expedition Squad, APPLY NOW!", msg_blue);

						addText("You have been appointed the leader of the Horntail Expedition Squad. ");
						addText("You'll now have 5 minutes to form the squad and have every member enter the mission.");
					end
				else
					addText("Talk to me if you want to become the leader of the squad.");
				end
			else
				addText("Only the leader of the party with 3 or more members that are at least Level 80 is eligible to apply for the leader of the expedition squad.");
			end
		end
		sendOk();
	else
		addText("The battle against Horntail has already started.");
		sendOk();
	end
else
	if getName() == getInstanceVariable("master") then
		if getInstanceVariable("enter", type_bool) then
			if isAtHead() then
				addText("The battle has already begun.");
				sendOk();
			else
				enterBossMap();
			end
		else
			choices = {
				makeChoiceHandler(" Check out the list of the Squad", function()
					getList();
					sendOk();
				end),
				makeChoiceHandler(" Expel a member from the Squad", function()
					if getInstanceSignupCount() == 1 then
						addText("No one has yet to sign up for the squad.");
						sendOk();
					else
						getLinkedList();
						choice = askChoice();

						if not verifyMaster() then
							return;
						end

						name = getInstancePlayerByIndex(choice + 1);
						addText("Are you sure you want to enter " .. blue(name) .. " in the Suspended List? ");
						addText("Once suspended, the user may not re-apply for a spot until the suspension is lifted by the leader of the squad.");
						answer = askYesNo();

						if not verifyMaster() then
							return;
						end

						if answer == answer_yes then
							banInstancePlayer(name);
							if setPlayer(name) then
								showMessage("The leader of the squad has entered you in the squad's Suspended List.", msg_red);
								revertPlayer();
							end
						end
					end
				end),
				makeChoiceHandler(" Re-accept a member from the Suspended List", function()
					if getBannedInstancePlayerCount() > 0 then
						getBannedList();
						player = askChoice();

						if not verifyMaster() then
							return;
						end

						unbanInstancePlayer(getBannedInstancePlayerByIndex(player + 1));
					else
						addText("No user is currently in the Suspended List.");
						sendOk();
					end
				end),
				makeChoiceHandler(red(" Form the Squad and enter", previousBlue), function()
					if isGm() or getInstanceSignupCount() >= 6 then
						setInstanceVariable("enter", true);
						messageAll("The leader of the squad has entered the map. Please enter the map before time runs out on the squad.");
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

			if not verifyMaster() then
				return;
			end

			selectChoice(choices, choice);
		end
	else
		if getInstanceVariable("enter", type_bool) and not isAtHead() then
			if isPlayerSignedUp(getName()) then
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
					elseif isPlayerSignedUp(getName()) then
						addText("You are already part of the expedition squad.");
					elseif getInstanceVariable("enter", type_bool) then
						addText("The application process for the Horntail Expedition Squad had already been concluded.");
					else
						addPlayerSignUp(getName());
						if setPlayer(getInstanceVariable("master")) then
							showMessage(getName() .. " has joined the expedition squad.", msg_red);
							revertPlayer();
						end
						addText("You have been enrolled in the Horntail Expedition Squad.");
					end
					sendOk();
				end),
				makeChoiceHandler(" Leave the Horntail Expedition Squad", function()
					if isPlayerSignedUp(getName()) then
						removePlayerSignUp(getName());
						if setPlayer(getInstanceVariable("master")) then
							showMessage(getName() .. " has withdrawn from the squad.", msg_red);
							revertPlayer();
						end
						addText("You have formally withdrawn from the squad.");
					else
						addText("Unable to leave the squad due to the fact that you're not participating in the Horntail Participation Squad.");
					end
					sendOk();
				end),
				makeChoiceHandler(" Check out the list of the Squad.", function()
					getList();
					sendOk();
				end),
			};

			addText("What would you like to do?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			if not verifyInstance() then
				return;
			end

			selectChoice(choices, choice);
		end
	end
end