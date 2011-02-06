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
-- Cesar (every town one)

if getLevel() >= 20 and getLevel() <= 30 then
	addText("I have prepared a huge festival here at Ariant for the great fighters of MapleStory. It's called #bThe Ariant Coliseum Challenge#k.");
	sendNext();

	addText("The Ariant Coliseum Challenge is a competition that matches the skills of monster combat against others. In this competition, your object isn't to hunt the monster;  rather, you need to #beliminate a set amount of HP from the monster, followed by absorbing it with a jewel#k. #bThe fighter that ends up with the most jewels will win the competition.#k.");
	sendBackNext();

	if getMap() == 100000000 then
		addText("If you are a superior bowman from #b#m" .. 100000000 .. "##k, training under the mighty Athena Pierce, then are you interested in participating in The Ariant Coliseum Challenge?!\r\n");
	elseif getMap() == 102000000 then
		addText("If you are a strong and brave warrior from #b#m" .. 102000000 .. "##k, training under Dances With Balrogs, then are you interested in participating in The Ariant Coliseum Challenge?!\r\n");
	elseif getMap() == 120000000 then
		addText("If you are a brave fighter from #b#m" .. 120000000 .. "##k, the town of fearless pirates, then are you interested in participating in The Ariant Coliseum Challenge?!\r\n");
	elseif getMap() == 220000000 then
		addText("If you are an adventurer from #b#m" .. 220000000 .. "##k, then are you interested in participating in The Ariant Coliseum Challenge?!\r\n");
	elseif getMap() == 260000000 then
		addText("Are you interested in participating in The Ariant Coliseum Challenge?!\r\n");
	end
	addText("#b#L0# I'd love to participate in this great competition.#l");
	what = askChoice();

	if what == 0 then 
		addText("Okay, now I'll send you to the battle arena. I'd like to see you emerge victorious!");
		sendNext();

		setPlayerVariable("aMatch_origin", getMap());
		setMap(980010000, "out00");
	end
else
	addText("Your level is not proper to participate in the Ariant Coliseum Challenge. Only players between Level #b20~30#k may participate.");
	sendOK();
end
