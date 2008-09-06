--[[
Copyright (C) 2008 Vana Development Team

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
-- Dances With Balrog - Warrior Instructor

if state == 0 then
	if getJob() == 0 then
		what = 1;
		addText("Do you wish to be a Warrior? You need to meet some ");
		addText("criteria in order to do so. #bYou need to be at least in Level 10,");
		addText("with at least 35 in STR#k. Let's see...");
		sendNext();
	elseif ((getJob() == 100 and getLevel() < 30) or getJob() == 110 or getJob() == 120 or getJob() == 130 or getJob() == 111 or getJob() == 121 or getJob() == 131 or getJob() == 112 or getJob() == 122 or getJob() == 132) then
		what = 2;
		addText("Oh, you have a question?\r\n");
		addText("#L0##bWhat are the general characteristics of being a Warrior?#k#l\r\n");
		addText("#L1##bWhat are the weapons that the Warriors use?#k#l\r\n");
		addText("#L2##bWhat are the armors that the Warriors can wear?#k#l\r\n");
		addText("#L3##bWhat are the skills available for the Warriors?#k#l");
		sendSimple();
	else
		what = 3;
		addText("Awesome body! Awesome power! Warriors are they way to go!!!! What do you think? ");
		addText("Want to make the job advancement as a Warrior??");
		sendNext();
		endNPC();
	end
elseif state == 1 then
	if what == 1 then
		if (getLevel() >= 10 and getSTR() >= 35) then
			passed = true;
			addText("You definitely have the look of a Warrior. You may not be ");
			addText("there just yet, but I can see the Warrior in you.");
			addText("What do you think? Do you want to become a Warrior?");
			sendYesNo();
		else
			addText("You need more training to be a Warrior. In order to be one, you need to train ");
			addText("yourself to be more powerful than you are right now. Please come back much stronger.");
			sendBackNext();
		end
	elseif what == 2 then
		explain = getSelected();
		if explain == 0 then
			addText("Let me explain the role of a Warrior. Warriors possess awesome physical strength and power. They can ");
			addText("also defende monsters' attacks, so they are the best when fighting up close with the monsters. With a ");
			addText("high level of stamina, you won't be dying easily either.");
			sendNext();
		elseif explain == 1 then
			addText("Let me explain the weapons Warriors use. They can use weapons that allow them to slash, stab or strike. ");
			addText("You won't be able to use weapons like bows and projectile weapons. Same with the small canes. ");
			sendNext();
		elseif explain == 2 then
			addText("Let me explain the armors Warriors wear. Warriors are strong with high stamine, so they are able to wear ");
			addText("tough, strong armor. It's not the greatest looking ones...but it serves its purpose well, the best of the armors.");
			sendNext();
		elseif explain == 3 then
			addText("For the Warriors, the skills available are geared towards their awesome physical strength and power. The ");
			addText("skill that helps you in close combats will help you the most. There's also a skill that allows you to ");
			addText("recover your HP. Make sure to master that.");
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
				addText("From here on out, you are going to be the Warrior! Please continue working hard...I'll enhance your abilities a bit with the hope of you training yourself to be even stronger than you're now. Haaaaaap!!");
				sendNext();
			else
				addText("Really? Do you need more time to give more thought to it?");
				addText("By all means... this is not something you should take ");
				addText("lightly. Come talk to me once your have made your decision.");
				sendNext();
			end
		else
			endNPC();
		end
	elseif what == 2 then
		if explain == 0 then
			addText("To accurately attack the monster, however, you need a healthy dose of DEX, so don't just concentrate ");
			addText("on boosting up the STR. If you want to improve rapidly, I recommend that you face stronger monsters.");
			sendBackNext();
		elseif explain == 1 then
			addText("The most common weapons are sword, blunt weapon, polearm, speak, axe, and etc...Every weapon has its ");
			addText("advantages and disadvantages, so please take a close look at them before choosin gone. For now, try ");
			addText("using the ones with high attack rating.");
			sendBackNext();
		elseif explain == 2 then
			addText("Especially the shields, they are perfect for the Warriors. Remember, though, that you won't be able to ");
			addText("use the shield if you are using the weapon that requires both hands. I know it's going to be a hard decision for you...");
			sendBackNext();
		elseif explain == 3 then
			addText("The two attacking skills available are #bPower Strike#k and #bSlash Blast#k. Power Strike is the one that ");
			addText("applies a lot of damage to a single enemy. You can boost this skills up from the beginning.");
			sendBackNext();
		end
	end
elseif state == 3 then
	if what == 1 then
		if advance then
			addText("You've gotten much stronger now. Plus every single one of your inventories have added slots. A whole row, to be exact. Go see for it yourself. I just gave you a little bit of #bSP#k. When you open up the #bSkill menu#k on the lower left corner of the screen, there are skills you can learn by using SP's. One warning, though: You can't raise it all together all at once. There are also skills you can accquire only after having learned a couple of skills first.");
			setJob(100)
			giveSP((getLevel() - 10) * 3 + 1); -- Make up SP for any over-leveling like in GMS
			giveItem(1302077, 1);
			hpinc = 200 + getRandomNumber(50); -- Extra HP given
			setMHP(getMHP() + hpinc); -- Set HP to current HP plus amount to be added for this job
			setRMHP(getRMHP() + hpinc);
			addSlots(1, 1); -- Add extra inventory rows
			addSlots(2, 1);
			addSlots(3, 1);
			addSlots(4, 1);
			sendNext();
		else
			endNPC();
		end
	elseif what == 2 then
		if explain == 3 then
			addText("On the other hand, Slash Blast does not apply much damage, but instead attacks multiple enemies around the ");
			addText("area at once. You can only use this once you have 1 Power Strike boosted up. Its up to you.");
			sendBackNext();
		else
			endNPC();
		end
	end
elseif state == 4 then
	if what == 1 then
		addText("I just gave you a little bit of #bSP#k. When you open op the #bSkill menu#k on the lower left ");
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
		addText("your hard-earned experience points, do you? This is all i can teach you...from here on out, it's all about pushing yourself harder and become better. See me after you feel that you have gotten much more powerful than you are right now.");
		sendBackNext();
	end
elseif state == 6 then
	if what == 1 then
		addText("Oh, and... if you have questions about being a Warrior, feel free to ask. I don't know ");
		addText("EVERYTHING, but I'll help you out with all that I know of. Til then...");
		sendBackNext();
	end
else
	endNPC();
end
