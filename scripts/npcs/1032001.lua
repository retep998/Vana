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
-- Grendel the Really Old - Magician Instructor

if state == 0 then
	if getJob() == 0 then
		what = 1;
		addText("Do you want to be a Magician? You need to meet some requirements in ");
		addText("order to do so. You need to be at least at #bLevel 8, and INT 20#k. ");
		addText("Let's see if you have what it takes to become a Magician...");
		sendNext();
	elseif ((getJob() == 200 and getLevel() < 30) or getJob() == 210 or getJob() == 220 or getJob() == 230 or getJob() == 211 or getJob() == 221 or getJob() == 231 or getJob() == 212 or getJob() == 222 or getJob() == 232) then
		what = 2;
		addText("Any questions about being a Magician?\r\n");
		addText("#L0##bWhat are the basic characteristics of being a Magician?#k#l\r\n");
		addText("#L1##bWhat are the weapons that the Magicians use?#k#l\r\n");
		addText("#L2##bWhat are the armors the Magicians can wear?#k#l\r\n");
		addText("#L3##bWhat are the skills available for Magicians?#k#l");
		sendSimple();
	else
		what = 3;
		addText("Would you like to have the power of nature in itself in your hands? It may be a long, ");
		addText("hard road to be on, but you'll surely be rewarded in the end, reaching the very top ");
		addText("of wizardry...");
		sendNext();
	end
elseif state == 1 then
	if what == 1 then
		if (getLevel() >= 8 and getINT() >= 20) then
			passed = true;
			addText("You definitely have the look of a Magician. You may not be there yet, but I can ");
			addText("see the Magician in you...what do you think? Do you want to become the Magician?");
			sendYesNo();
		else
			addText("You need more training to be a Magician. In order to be one, you need to train ");
			addText("yourself to be more powerful than you are right now. Please come back much stronger.");
			sendBackNext();
		end
	elseif what == 2 then
		explain = getSelected();
		if explain == 0 then
			addText("I'll tell you more about being a Magician. Magicians put high levels of magic and intelligence to ");
			addText("good use. They can use the power of nature all around us to kill the enemies, but they are very ");
			addText("weak in close combats. The stamina isn't high, either, so be careful and avoid death at all cost.");
			sendNext();
		elseif explain == 1 then
			addText("I'll tell you more about the weapons that Magicians use. Actually, it doesn't mean much for Magicians ");
			addText("to attack the opponents with weapons. Magicians lack power and dexterity, so you will have a hard time ");
			addText("even defeating a snail.");
			sendNext();
		elseif explain == 2 then
			addText("I'll tell you more about the armors that Magicians can wear. Honestly, the Magicians don't have much ");
			addText("armor to wear since they are weak in physiical strength and low in stamina. Its defensive abilities ");
			addText("isn't great either, so I don't know if it helps a lot or not...");
			sendNext();
		elseif explain == 3 then
			addText("The skills available for Magicians use the high levels of intelligence and magic that Magicians have. ");
			addText("Also available are Magic Guard and Magic Armor, which help Magicians with weak stamina prevent from dying.");
			sendNext();
		end
	else
		endNPC();
	end
elseif state == 2 then
	if what == 1 then
		if passed then
			if getSelected() == 1 then
				advance = true;
				addText("Alright, you're a Magician from here on out, since I, Grendel the Really old, the ");
				addText("head Magician, allow you so. It isn't much, but I'll give you a little bit of what I have...");
				sendNext();
			else
				addText("Really? Have to give more thought to it, huh? Take your time, take your time. This is not ");
				addText("something you should take lightly...come talk to me once your have made your decision...");
				sendNext();
			end
		else
			endNPC();
		end
	elseif what == 2 then
		if explain == 0 then
			addText("Since you can attack the monsters from afar, that'll help you quite a bit. Try boosting up the level ");
			addText("of INT if you want to attack the enemies accurately with your magic. The higher your intelligence, the ");
			addText("better you'll be able to handle your magic...");
			sendBackNext();
		elseif explain == 1 then
			addText("If we're talking about the magicial powers, then THAT's a whole different story. The weapons that ");
			addText("Magicians use are blunt weapons, staff, and wands. Blunt weapons are good for, well, blunt attacks, ");
			addText("but...I would not recommend that on Magicians, period...");
			sendBackNext();
		elseif explain == 2 then
			addText("Some armors, however, have the ability to eliminate the magicial power, so it can guard you from magic ");
			addText("attacks. It won't help much, but still better than not warning them at all...so buy them if you have time...");
			sendBackNext();
		elseif explain == 3 then
			addText("The offensive skills are #bEnergy Bolt#k and #bMagic Claw#k. First, Energy Bolt is a skill that applies a ");
			addText("lot of damage to the opponent with minimal use of MP.");
			sendBackNext();
		end
	end
elseif state == 3 then
	if what == 1 then
		if advance then
			addText("You have just equipped yourself with much more magicial power. Please keep training and ");
			addText("make yourself much better...I'll be watching you from here and there...");
			setJob(200) -- Set job to Magician
			giveSP((getLevel() - 8) * 3 + 1); -- Make up SP for any over-leveling like in GMS
			giveItem(1372043, 1); -- Give Beginner Wand
			mpinc = 100 + getRandomNumber(50); -- Extra MP given
			setRMMP(getRMMP() + mpinc); -- Increase MP by adding the randomly generated number to the current MP
			setMMP(getMMP() + mpinc); -- Increase MP by adding the randomly generated number to the current MP
			sendNext();
		else
			endNPC();
		end
	elseif what == 2 then
		if explain == 1 then
			addText("Rather, staffs and wands are the main weaponry of choice. These weapons have special magicial powers ");
			addText("in them, so it enhances the Magicians' effectiveness. It'll be wise for you to carry a weapon with ");
			addText("a lot of magicial powers in it...");
			sendBackNext();
		elseif explain == 3 then
			addText("Magic Claw, on the other hand, uses up a lot of MP to attack one opponent TWICE. But, you can only use ");
			addText("Energy Bolt once it's more than 1, so keep that in mind. Whatever you choose to do, it's all upto you...");
			sendBackNext();
		else
			endNPC();
		end
	end
elseif state == 4 then
	if what == 1 then
		addText("I just gave you a little bit of #bSP#k. When you open up the #bSkill menu#k on the lower left ");
		addText("corner of the screen, there are skills you can learn by using SP's. One warning, though: You ");
		addText("can't raise it all together all at once. There are also skills you can acquire only after having ");
		addText("learned a couple of skills first.");
		sendNext();
	else
		endNPC();
	end
elseif state == 5 then
	if what == 1 then
		addText("One more warning. Once you have chosen your job, try to stay alive as much as you can. Once you ");
		addText("reach that level, when you die, you will lose your experience level. You wouldn't want to lose ");
		addText("your hard-earned experience points, do you?");
		sendBackNext();
	end
elseif state == 6 then
	if what == 1 then
		addText("OK! This is all I can teach you. Go to places, train and better yourself. Find me when you feel ");
		addText("like you've done all you can, and need something interesting. I'll be waiting for you here...");
		sendBackNext();
	end
elseif state == 7 then
	if what == 1 then
		addText("Oh, and... if you have any questions about being the Magician, feel free to ask. I don't know ");
		addText("EVERYTHING, per se, but I'll help you out with all that I know of. Til then...");
		sendBackNext();
	end
else
	endNPC();
end
