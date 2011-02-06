--[[
Copyright (C) 2008-2011 Vana Development Team

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
-- Adobis that allows you to enter Zakum

dofile("scripts/lua_functions/signupHelper.lua");
dofile("scripts/lua_functions/bossHelper.lua");

function enterBossMap()
	x = getMaxZakumBattles();
	if enterBoss("Zakum", x) then
		setMap(280030000);
	else
		addText("You may only enter this place " .. x .. " " .. timeString(x) .. " per day.");
		sendOK();
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
	return isInstance("zakumSignup");
end

if getLevel() < 50 then
	-- Should never get this far, but whatever, double checking
	addText("Only players with level 50 or above are qualified to join the Zakum Squad.");
	sendOK();
	return;
end

if verifyInstance() and isBannedInstancePlayer(getName()) then
	addText("Your participation is formally rejected from the squad. ");
	addText("You may only participate with an approval from the leader of the squad.");
	sendOK();
	return;
end

if not verifyInstance() then
	if not isInstance("zakum") then
		if isPartyLeader() then
			addText("Would you like to become the leader of the Zakum Expedition Squad?");
			ans = askYesNo();

			if verifyInstance() then
				-- Check again, make sure that no glitches occur... in theory
				addText("The expedition squad is already active.");
			else
				if ans == 1 then
					if not isPartyInLevelRange(50, 200) or getPartyMapCount() < 3 then
						addText("Only the leader of the party that consists of 3 or more members is eligible to become the leader of the Zakum Expedition Squad.");
					else
						createInstance("zakumSignup", 5 * 60, true);
						addPlayerSignUp(getName());
						setInstanceVariable("master", getName());
						showMapMessage(getName() .. " has been appointed the leader of the Zakum Expedition Squad. To those willing to participate in the Expedition Squad, APPLY NOW!", 6);

						addText("You have been appointed the leader of the Zakum Expedition Squad. You'll now have 5 minutes to form the squad and have every member enter the mission.");
					end
				else
					addText("Talk to me if you want to become the leader of the squad.");
				end
			end
		else
			addText("Only the leader of the party with 3 or more members that are at least Level 50 is eligible to apply for the leader of the expedition squad.");
		end
	else
		addText("The battle against Zakum has already started.");
	end
	sendOK();
else
	if getName() == getInstanceVariable("master") then
		if getInstanceVariable("enter", true) then
			if getReactorState(211042300, 2118002) == 1 then
				addText("The battle has already begun.");
				sendOK();
			else
				enterBossMap();
			end
		else
			addText("Greetings, leader of the Zakum Expedition Squad. What would you like to do? \r\n");
			addText("#b#L0# Check out the list of the Squad#l\r\n");
			addText("#L1# Expel a member from the Squad#l\r\n");
			addText("#L2# Re-accept a member from the Suspended List#l\r\n");
			addText("#r#L3# Form the Squad and enter#l#k");
			choice = askChoice();

			if not verifyMaster() then
				return;
			end

			if choice == 0 then
				getList();
				sendOK();
			elseif choice == 1 then
				if getInstanceSignupCount() == 1 then
					addText("No one has yet to sign up for the squad.");
					sendOK();
				else
					getLinkedList();
					banmember = askChoice();

					if not verifyMaster() then
						return;
					end

					name = getInstancePlayerByIndex(banmember + 1);
					addText("Are you sure you want to enter #b" + name + "#k in the Suspended List? ");
					addText("Once suspended, the user may not re-apply for a spot until the suspension is lifted by the leader of the squad.");
					ban = askYesNo();

					if not verifyMaster() then
						return;
					end

					banInstancePlayer(name);
					if setPlayer(name) then
						showMessage("The leader of the squad has entered you in the squad's Suspended List.", m_red);
						revertPlayer();
					end
				end
			elseif choice == 2 then
				if getBannedInstancePlayerCount() > 0 then
					getBannedList();
					player = askChoice();

					if not verifyMaster() then
						return;
					end

					unbanInstancePlayer(getBannedInstancePlayerByIndex(player + 1));
				else
					addText("No user is currently in the Suspended List.");
					sendOK();
				end
			elseif choice == 3 then
				if getInstanceSignupCount() >= 6 then
					setInstanceVariable("enter", "1");
					messageAll("The leader of the squad has entered the map. Please enter the map before time runs out on the squad.");
					createInstance("zakum", 0, false);

					enterBossMap();
				else
					addText("The squad needs to consist of 6 or more members in order to start the quest.");
					sendOK();
				end
			end
		end
	else
		if getInstanceVariable("enter", true) and getReactorState(211042300, 2118002) == 0 then
			if isPlayerSignedUp(getName()) then
				enterBossMap();
			else
				addText("You may not enter this premise if you are not a member of the Zakum Expedition Squad.");
				sendOK();
			end
		elseif getReactorState(211042300, 2118002) == 1 then
			addText("The battle has already begun.");
			sendOK();				
		else
			addText("What would you like to do?\r\n#b");
			addText("#L0# Enter the Zakum Expedition Squad#l\r\n");
			addText("#L1# Leave the Zakum Expedition Squad#l\r\n");
			addText("#L2# Check out the list of the Squad.#k");
			choice = askChoice();

			if not verifyInstance() then
				return;
			end

			if choice == 0 then
				if isListFull() then
					addText("Unable to apply for a spot due to number of applicants already reaching the maximum.");
				elseif isPlayerSignedUp(getName()) then
					addText("You are already part of the expedition squad.");
				elseif getInstanceVariable("enter", true) then
					addText("The application process for the Zakum Expedition Squad had already been concluded.");
				else
					addPlayerSignUp(getName());
					if setPlayer(getInstanceVariable("master")) then
						showMessage(getName() .. " has joined the expedition squad.", m_red);
						revertPlayer();
					end
					addText("You have been enrolled in the Zakum Expedition Squad.");
				end
			elseif choice == 1 then
				if isPlayerSignedUp(getName()) then
					removePlayerSignUp(getName());
					if setPlayer(getInstanceVariable("master")) then
						showMessage(getName() .. " has withdrawn from the squad.", m_red);
						revertPlayer();
					end
					addText("You have formally withdrawn from the squad.");
				else
					addText("Unable to leave the squad due to the fact that you're not participating in the Zakum Participation Squad.");
				end
			elseif choice == 2 then
				getList();
			end
			sendOK();
		end
	end
end