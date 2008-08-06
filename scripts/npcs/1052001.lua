-- Dark Lord - Thief Instructor

if state == 0 then
	if getJob() == 0 then
		what = 1;
		addText("Want to be a thief? There are some standards to meet, because we can't just accept ");
		addText("EVERYONE in ... #bYour level should be at least 10, with your DEX over 25#k. ");
		addText("Let's see...");
		sendNext();
	elseif ((getJob() == 400 and getLevel() < 30) or getJob() == 410 or getJob() == 420 or getJob() == 411 or getJob() == 421 or getJob() == 412 or getJob() == 422) then
		what = 2;
		addText("Do you have anything you want to know about thieves?\r\n");
		addText("#L0##bWhat are the basic characters of a Thief?#k#l\r\n");
		addText("#L1##bWhat are the weapons that the Thieves use?#k#l\r\n");
		addText("#L2##bWhat are the armors that the Thieves use?#k#l\r\n");
		addText("#L3##bWhat are the skills available for Thieves?#k#l");
		sendSimple();
	else
		what = 3;
		addText("Exploring is good, and getting stronger is good and all... but don't you want to enjoy ");
		addText("living the life as you know it? How about becoming a Rouge like us and really LIVE the ");
		addText("life? Sounds fun, isn't it?");
		sendNext();
		endNPC();
	end
	
elseif state == 1 then
	if what == 1 then
		if (getLevel() >= 10 and getDEX() >= 25) then
			passed = true;
			addText("Oh...! You look like someone that can definitely be a part of us...all you need is ");
			addText("a little sinister mind, and...yeah...so, what do you think? Wanna be the Rouge?");
			sendYesNo();
		else
			addText("Hmm...you're still a beginner...doesn't look like you can hang out with us, yet... ");
			addText("make yourself much stronger, THEN find me...");
			sendBackNext();
		end
	elseif what == 2 then
		explain = getSelected();
		if explain == 0 then
			addText("Let me explain to you what being a thief means. Thieves have just the right amount of stamina and ");
			addText("strength to survive. We don't recommend training for strength just like those warriors. What we do ");
			addText("need are LUK and DEX...");
			sendNext();
		elseif explain == 1 then
			addText("Thieves use these weapons. They have just the right amount of intelligence and power...what we do have ");
			addText("are quick movements and even quick brain...");
			sendNext();
		elseif explain == 2 then
			addText("Let me explain to you the armors that the thieves can wear. Thieves value quick so you need clothes that ");
			addText("fit you perfectly. But then again, they don't need chained armors like the bowman, because it won't help ");
			addText("you one bit.");
			sendNext();
		elseif explain == 3 then
			addText("For thieves there are skills that supports high accuracy and dexterity we have in general. A good mix of ");
			addText("skills are available, for both the throwing stars and daggers. Choose your weapon carefully, for skills ");
			addText("are determined by it.");
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
				addText("Alright, from here on out, you are part of us! You'll be living the life of a wanderer ");
				addText("at first, but just be patient and soon, you'll be living the high life. Alright, it ");
				addText("ain't much, but I'll give you some of my abilities...HAAAHHH!!");
				sendNext();
			else
				addText("I see...well, it's a very important step to take, choosing your job. But don't you ");
				addText("want to live the fun life? let me know when you have made up your mind, ok?");
				sendNext();
			end
		else
			endNPC();
		end
	elseif what == 2 then
		if explain == 0 then
			addText("If you raise the level of luck and dexterity, that will increase the damage you'll dish out to the ");
			addText("enemies. Thieves also differentiate themselves from the rest by using such throwing weapons as ");
			addText("throwing stars and throwing knives. They can also avoid many attacks with high dexterity.");
			sendBackNext();
		elseif explain == 1 then
			addText("Because of that, we usually use daggers or throwing weapons. Daggers are basically small swords but ");
			addText("they are very quick, enabling you to squeeze in many attacks. If you fight in a close combat, use ");
			addText("the dagger to quickly eliminate the enemy before it reacts to your attack.");
			sendBackNext();
		elseif explain == 2 then
			addText("Instead of flashy clothes or tough, hard gold-plated armor, try putting on simple, comfortable clothes ");
			addText("that fit you perfectly and still do its job in the process. It'll help you a lot in hunting the monsters.");
			sendBackNext();
		elseif explain == 3 then
			addText("If you are using throwing-stars, skills like #bKeen Eyes#k or #bLucky Seven#k are perfect. Lucky Seven ");
			addText("allows you to throw multiple throwing-stars at once, so it'll help you greatly in hunting down enemies."):
			sendBackNext();
		end
	end
elseif state == 3 then
	if what == 1 then
		if advance then
			addText("I've just created more slots for your equipment and etc. storage. Not only that, but you've also ");
			addText("gotten stronger as well. As you become part of us, and learn to enjoy life in different angles, ");
			addText("you may one day be on top of this of darkness. I'll be watching your every move, so don't let me down.");
			setJob(400);
			giveSP((getLevel() - 10) * 3 + 1); -- Make up any SP for over-leveling like in GMS
			giveItem(1472061, 1);
			giveItem(1332063, 1);
			giveItem(2070015, 1); -- Special subis
			giveItem(2070015, 1);
			giveItem(2070015, 1);
			hpinc = 100 + getRandomNumber(50); -- Generate a random number from 0-50 to add to the base hp increase
			mpinc = 25 + getRandomNumber(25); -- Generate a random number from 0-25 to add to the base mp increase
			setMHP(getMHP() + hpinc); -- Set HP to current HP plus full amount of hp increase
			setRMHP(getRMHP() + hpinc);
			setMMP(getMMP() + mpinc); -- Set MP to current MP plus full amount of hp increase
			setRMMP(getRMMP() + mpinc);
			addSlots(1, 1);
			addSlots(4, 1);
			sendNext();
		else
			endNPC();
		end
	elseif what == 2 then
		if explain == 1 then
			addText("For throwing weapons there are throwing-stars and throwing-knives available. You can't just use them ");
			addText("by themselves, though. Go to the weapon store at Kerning City, and they'll sell an claw call Garnier. ");
			addText("Equip yourself with it, then you'll be able to throw the throwing stars that's in the use inventory.");
			sendBackNext();
		elseif explain == 3 then
			addText("If you are using daggers choose #bDisorder#k or #bDouble Stab#k as skills. Double Stab, in fact, will be ");
			addText("a great skills to use in that it enables you to attack an enemy in a blinding flurry of stabs, a definate ");
			addText("stunner indeed.");
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
	elseif what == 2 then
		if explain == 1 then
			addText("Not only is it important to choose the right Claw to use, but it's just as important to choose the ");
			addText("right kind of throwing stars to use. Do you want to know where to get those stars? Go check out the ");
			addText("armor store around this town...there's probably someone that handles those specifically...");
			sendBackNext();
		else
			endNPC();
		end
	end
elseif state == 5 then
	if what == 1 then
		addText("One more warning. Once you have chosen your job, try to stay alive as much as you can. Once you ");
		addText("reach that level, when you die, you will lose your experience level. You wouldn't want to lose ");
		addText("your hard-earned experience points, do you?");
		sendBackNext();
	else
		endNPC();
	end
elseif state == 6 then
	if what == 1 then
		addText("OK! This is all I can teach you. Go to places, train and better yourself. Find me when you feel ");
		addText("like you've done all you can, and need something interesting. Then, and only then, I'll hook you ");
		addText("up with more interesting experiences...");
		sendBackNext();
	end
elseif state == 7 then
	if what == 1 then
		addText("Oh, and... if you have any questions about being the Thief, feel free to ask. I don't know ");
		addText("EVERYTHING, but I'll help you out with all that I know of. Til then...");
		sendBackNext();
	end
else
	endNPC();
end
