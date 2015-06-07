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
-- Cesar (every town one)

dofile("scripts/utils/npcHelper.lua");

if getLevel() >= 20 and getLevel() <= 30 then
	addText("I have prepared a huge festival here at Ariant for the great fighters of MapleStory. ");
	addText("It's called " .. blue("The Ariant Coliseum Challenge") .. ".");
	sendNext();

	addText("The Ariant Coliseum Challenge is a competition that matches the skills of monster combat against others. ");
	addText("In this competition, your object isn't to hunt the monster;  rather, you need to " .. blue("eliminate a set amount of HP from the monster, followed by absorbing it with a jewel") .. ". ");
	addText(blue("The fighter that ends up with the most jewels will win the competition") .. ".");
	sendBackNext();

	m = getMap();
	text = nil;

	if m == 100000000 then text = "If you are a superior bowman from " .. blue(mapRef(m)) .. ", training under the mighty Athena Pierce, then are";
	elseif m == 102000000 then text = "If you are a strong and brave warrior from " .. blue(mapRef(m)) .. ", training under Dances With Balrogs, then are";
	elseif m == 120000000 then text = "If you are a brave fighter from " .. blue(mapRef(m)) .. ", the town of fearless pirates, then are";
	elseif m == 220000000 then text = "If you are an adventurer from " .. blue(mapRef(m)) .. ", then are";
	elseif m == 260000000 then text = "Are";
	end

	addText(text .. " you interested in participating in The Ariant Coliseum Challenge?!\r\n");
	addText(blue(choiceRef("I'd love to participate in this great competition.")));
	choice = askChoice();

	addText("Okay, now I'll send you to the battle arena. ");
	addText("I'd like to see you emerge victorious!");
	sendNext();

	setPlayerVariable("aMatch_origin", m);
	setMap(980010000, "out00");
else
	addText("Your level is not proper to participate in the Ariant Coliseum Challenge. ");
	addText("Only players between Level " .. blue("20~30") .. " may participate.");
	sendOk();
end