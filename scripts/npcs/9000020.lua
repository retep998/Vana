--[[
Copyright (C) 2008-2009 Vana Development Team

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
if getMap() == 800000000 then
	if state == 0 then
		wt_origin = getPlayerVariable("wt_origin");
		addText("How's the traveling? Are you enjoying it?\r\n");
		addText("#b#L0#Yes, I'm done with traveling. Can I go back to #m" .. wt_origin .."#? #l\r\n");
		addText("#b#L1#No, I'd like to continue exploring this place.#l");
		sendSimple();
	elseif state == 1 then
		if getSelected() == 0 then
			addText("Alright. I'll now take you back to where you were before the visit to Japan. If you ever feel like traveling again down the road, please let me know!");
			sendNext();
		else
			addText("OK. If you ever change your mind, please let me know.");
			sendOK();
			endNPC();
		end
	elseif state == 2 then
		endNPC();
		wt_origin = getPlayerVariable("wt_origin");
		setMap(wt_origin);
		deletePlayerVariable("wt_origin");
	end
else
	if state == 0 then
		addText("If you're tired of the monotonous daily life, how about getting out for a change? There's nothing quite like soaking up a new culture, learning something new by the minute! It's time for you to get out and travel. We, at the Maple Travel Agency recommend you going on a #bWorld Tour#k! Are you worried about the travel expense? ");
		if getJob() == 0 then
			addText("No need to worry! The #bMaple Travel Agency#k offers first class travel accommodation for the low price of #b300 mesos#k");
		else
			addText("You shouldn't be! We, the #bMaple Travel Agency#k, have carefully come up with a plan to let you travel for ONLY #b3,000 mesos!#k");
		end
		sendNext();
	elseif state == 1 then
		addText("We currently offer this place for your traveling pleasure: #bMushroom Shrine of Japan#k. I'll be there serving you as the travel guide. Rest assured, the number of destinations will increase over time. Now, would you like to head over to the Mushroom Shrine?\r\n");
		addText("#b#L0# Yes, take me to Mushroom Shrine (Japan)#k#l");
		sendSimple();
	elseif state == 2 then
		if getSelected() == 0 then
			addText("Would you like to travel to #bMushroom Shrine of Japan#k? If you desire to feel the essence of Japan, there's nothing like visiting the Shrine, a Japanese cultural melting pot. Mushroom Shrine is a mythical place that serves the incomparable Mushroom God from ancient times.");
			sendNext();
		end
	elseif state == 3 then
		if getSelected() == 0 then
			addText("Check out the female shaman serving the Mushroom God, and I strongly recommend trying Takoyaki, Yakisoba, and other delicious food sold in the streets of Japan. Now, let's head over to #bMushroom Shrine#k, a mythical place if there ever was one.");
			sendBackNext();
		end
	elseif state == 4 then
		if getJob() == 0 then
			price = 300;
		else
			price = 3000;
		end
		if getMesos() >= price then
			endNPC();
			setPlayerVariable("wt_origin", getMap());
			setMap(800000000);
			giveMesos(-price);
		else
			addText("Please check and see if you have enough mesos to go.");
			sendBackOK();
		end
	else
		endNPC();
	end
end
