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
-- Dances With Balrog - Warrior Instructor

dofile("scripts/lua_functions/jobFunctions.lua");

if getJob() == 0 then
	addText("Do you wish to be a Warrior? You need to meet some ");
	addText("criteria in order to do so. #bYou need to be at least in Level 10");
	addText("#k. Let's see...");
	sendNext();

	if (getLevel() >= 10) then
		addText("You definitely have the look of a Warrior. You may not be ");
		addText("there just yet, but I can see the Warrior in you.");
		addText("What do you think? Do you want to become a Warrior?");
		yes = askYesNo();

		if yes == 1 then
			addText("From here on out, you are going to be the Warrior! Please continue working hard...I'll enhance your abilities a bit with the hope of you training yourself to be even stronger than you're now. Haaaaaap!!");
			sendNext();

			if getLevel() >= 30 then -- For rare "too high level" instance.
				addText("I think you've made the job advancement way too late. Usually, for beginners under Level 29 ");
				addText("that were late in making job advancements, we compensate them with lost Skill Points, ");
				addText("that weren't rewarded, but...I think you're a little too late for that. I am so sorry, but there's nothing I can do.");
				sendBackNext();

				giveSP(1);
			else
				giveSP((getLevel() - 10) * 3 + 1); -- Make up SP for any over-leveling like in GMS
			end

			setJob(100);
			giveItem(1302077, 1); -- Give beginner warrior's sword
			hpinc = 200 + getRandomNumber(50); -- Extra HP given
			setMaxHP(getRealMaxHP() + hpinc); -- Set HP to current HP plus amount to be added for this job
			setSTR(35); -- Stat reset
			setDEX(4);
			setINT(4);
			setLUK(4);
			setAP((getLevel() - 1) * 5 - (22));
			addSlots(1, 1); -- Add extra inventory rows
			addSlots(2, 1);
			addSlots(3, 1);
			addSlots(4, 1);

			addText("You've gotten much stronger now. Plus every single one of your inventories have added slots. A whole row, to be exact. Go see for it yourself. I just gave you a little bit of #bSP#k. When you open up the #bSkill menu#k on the lower left corner of the screen, there are skills you can learn by using SP's. One warning, though: You can't raise it all together all at once. There are also skills you can accquire only after having learned a couple of skills first.");
			sendNext();

			addText("One more warning. Once you have chosen your job, try to stay alive as much as you can. Once you ");
			addText("reach that level, when you die, you will lose your experience level. You wouldn't want to lose ");
			addText("your hard-earned experience points, do you? This is all i can teach you...from here on out, it's all about pushing yourself harder and become better. See me after you feel that you have gotten much more powerful than you are right now.");
			sendBackNext();

			addText("Oh, and... if you have questions about being a Warrior, feel free to ask. I don't know ");
			addText("EVERYTHING, but I'll help you out with all that I know of. Til then...");
			sendBackNext();
		else
			addText("Really? Do you need more time to give more thought to it?");
			addText("By all means... this is not something you should take ");
			addText("lightly. Come talk to me once your have made your decision.");
			sendNext();
		end
	else
		addText("You need more training to be a Warrior. In order to be one, you need to train ");
		addText("yourself to be more powerful than you are right now. Please come back much stronger.");
		sendBackNext();
	end
elseif (getJobLine() == 1 and getJobTrack() > 0) then
	addText("Oh, you have a question?\r\n");
	addText("#L0##bWhat are the general characteristics of being a Warrior?#k#l\r\n");
	addText("#L1##bWhat are the weapons that the Warriors use?#k#l\r\n");
	addText("#L2##bWhat are the armors that the Warriors can wear?#k#l\r\n");
	addText("#L3##bWhat are the skills available for the Warriors?#k#l");
	explain = askChoice();

	if explain == 0 then
		addText("Let me explain the role of a Warrior. Warriors possess awesome physical strength and power. They can ");
		addText("also defende monsters' attacks, so they are the best when fighting up close with the monsters. With a ");
		addText("high level of stamina, you won't be dying easily either.");
		sendNext();

		addText("To accurately attack the monster, however, you need a healthy dose of DEX, so don't just concentrate ");
		addText("on boosting up the STR. If you want to improve rapidly, I recommend that you face stronger monsters.");
		sendBackNext();
	elseif explain == 1 then
		addText("Let me explain the weapons Warriors use. They can use weapons that allow them to slash, stab or strike. ");
		addText("You won't be able to use weapons like bows and projectile weapons. Same with the small canes. ");
		sendNext();

		addText("The most common weapons are sword, blunt weapon, polearm, speak, axe, and etc...Every weapon has its ");
		addText("advantages and disadvantages, so please take a close look at them before choosin gone. For now, try ");
		addText("using the ones with high attack rating.");
		sendBackNext();
	elseif explain == 2 then
		addText("Let me explain the armors Warriors wear. Warriors are strong with high stamine, so they are able to wear ");
		addText("tough, strong armor. It's not the greatest looking ones...but it serves its purpose well, the best of the armors.");
		sendNext();

		addText("Especially the shields, they are perfect for the Warriors. Remember, though, that you won't be able to ");
		addText("use the shield if you are using the weapon that requires both hands. I know it's going to be a hard decision for you...");
		sendBackNext();
	elseif explain == 3 then
		addText("For the Warriors, the skills available are geared towards their awesome physical strength and power. The ");
		addText("skill that helps you in close combats will help you the most. There's also a skill that allows you to ");
		addText("recover your HP. Make sure to master that.");
		sendNext();

		addText("The two attacking skills available are #bPower Strike#k and #bSlash Blast#k. Power Strike is the one that ");
		addText("applies a lot of damage to a single enemy. You can boost this skills up from the beginning.");
		sendBackNext();

		addText("On the other hand, Slash Blast does not apply much damage, but instead attacks multiple enemies around the ");
		addText("area at once. You can only use this once you have 1 Power Strike boosted up. Its up to you.");
		sendBackNext();
	end
elseif getJob() == 100 and getLevel() >= 30 then
	if (getItemAmount(4031012) == 0 and getItemAmount(4031008) == 0) then
		addText("Whoa! You have definitely grown up! You don't look small and weak anymore...rather, now I can feel your presence ");
		addText("as the Warrior! Impressive...so, what do you think? Do you want to get even stronger than you are right now? ");
		addText("Pass a simple test and I'll do just that! Wanna do it?");
		yes = askYesNo();

		if yes == 0 then
			addText("Really? It will help you out a great deal on your journey if you get stronger fast...if you choose to ");
			addText("change your mind in the future, please feel free to come back. Know that I'll make you much more ");
			addText("powerful than you are right now.");
			sendNext();
		else
			addText("Good thinking. You look strong, don't get me wrong, but there's still a need to test your strength ");
			addText("and see if you are for real. The test isn't too difficult, so you'll do just fine...Here, take this ");
			addText("letter first. Make sure you don't lose it.");
			sendNext();

			giveItem(4031008, 1);
			addText("Please get this letter to #b#p1072000##k who may be around #b#m102020300##k ");
			addText("that's near Perion. He's the one being the instructor now in place of me, as I am busy here. ");
			addText("Get him the letter and he'll give the test in place of me. For more details, hear it straight from him. ");
			addText("Best of luck to you.");
			sendBackNext();
		end
	elseif (getItemAmount(4031012) == 0 and getItemAmount(4031008) > 0) then
		addText("Still haven't met the person yet? Find #b#p1072000##k who's around #b#m102020300##k ");
		addText("near Perion. Give the letter to him and he may let you know what to do.");
		sendNext();
	elseif (getItemAmount(4031012) > 0 and getItemAmount(4031008) == 0) then
		addText("OHH...you came back safe! I knew you'd breeze through...I'll admit you are a strong, formidable warrior...");
		addText("alright, I'll make you an even strong Warrior than you already are right now...Before THAT! you need to ");
		addText("choose one of the three paths that you'll be given...it isn't going to be easy, so if you have any questions, ");
		addText("feel free to ask.");
		sendNext();

		addText("Alright, when you have made your decision, click on [I'll choose my occupation!] at the very bottom.\r\n");
		addText("#L0##bPlease explain the role of the Fighter.#k#l\r\n");
		addText("#L1##bPlease explain the role of the Page.#k#l\r\n");
		addText("#L2##bPlease explain the role of the Spearman.#k#l\r\n");
		addText("#L3##bI'll choose my occupation!#k#l\r\n");
		explain = askChoice();

		if explain == 0 then
			addText("Let me explain the role of the fighter. It's the most common kind of Warriors. The weapons they ");
			addText("use are #bsword#k and #baxe#k , because there will be advanced skills available to acquired later on.");
			addText("I strongly recommend you avoid using both weapons, but rather stick to the one of your liking...");
			sendNext();

			addText("Other than that, there are also skills such as #bRage#k and #bPower Guard#k available for fighters. ");
			addText("#bRage#k is the kind of an ability that allows you and your party to temporarily enhance your weapon power. ");
			addText("With that you can take out the enemies with a sudden surge of power, so it'll come very handy for you. ");
			addText("The downside to this is that your guarding ability (defense) goes down a bit.");
			sendBackNext();

			addText("#bPower Guard#k is an ability that allows you to return a portion of the damage that you take from a ");
			addText("weapon hit by an enemy. The harder the hit, the harder the damage they'll get in return. It'll help ");
			addText("those that prefer close combat. What do you think? Isn't being the Fighter pretty cool?");
			sendBackNext();
		elseif explain == 1 then
			addText("Let me explain the role of the Page. Page is a knight-in-training, taking its first steps to becoming ");
			addText("an actual knight. They usually use #bswords#k and/or #bblunt weapons#k. It's not wise to use both weapons ");
			addText("so it'll be best for you to stick to one or the other.");
			sendNext();

			addText("Other than that, there are also skills such as #bThreaten#k and #Power Guard#k to learn. #bThreaten#k makes ");
			addText("every opponent around you lose some attacking and defending abilities for a time being. It's very useful ");
			addText("against powerful monsters with good attacking abilities. It also works well in party play.");
			sendBackNext();

			addText("#bPower Guard#k is an ability that allows you to return a portion of the damage that you take from a ");
			addText("weapon hit by an enemy. The harder the hit, the harder the damage they'll get in return. It'll help ");
			addText("those that prefer close combat. What do you think? Isn't being the Page pretty cool?");
			sendBackNext();
		elseif explain == 2 then
			addText("Let me explain the role of the Spearman. It's a job that specializes in using long weapons such as ");
			addText("#bspears#k and #bpolearms#k. There are lots of useful skills to acquire with both of the weapons, ");
			addText("but I strongly recommend you stick to one and focus on it.");
			sendNext();

			addText("Other than that, there are also skills such as #bIron Will#k and #bHyper Body#k to learn. #bIron Will#k ");
			addText("allows you and the members of your party to increase attack and magic defense for a period of time. ");
			addText("It's the skill that's very useful for Spearmen with weapons that require both hands and can't guard ");
			addText("themselves as well.");
			sendBackNext();

			addText("#bHyper Body#k is a skill that allows you and your party to temporarily improve the max HP and MP. ");
			addText("You can improve almost 160% so it'll help you and your party especially when going up against really ");
			addText("tough opponents. What do you think? Don't you think being the Spearman can be pretty cool?");
			sendBackNext();
		elseif explain == 3 then
			addText("Hmmm, have you made up your mind? Then choose the 2nd job advancement of your liking.\r\n");
			addText("#L0##bFighter#k#l\r\n");
			addText("#L1##bPage#k#l\r\n");
			addText("#L2##bSpearman#k#l\r\n");
			choice = askChoice();

			if choice == 0 then
				addText("So you want to make the 2nd job advancement as the #bFighter#k? Once you make that decision ");
				addText("you can't go back and choose another job...do you still wanna do it?");
				yes = askYesNo();

				if yes == 0 then
					addText("Really? So you need to think about it a little more? Take your time...this is not something ");
					addText("that you should take lightly...let me know when you have made your decision, okay?");				
					sendNext();

				else
					if getSP() > ((getLevel() - 30) * 3) then
						addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many ");
						addText("SP in store. Use more SP on the skills on the 1st level and then come back.");
						sendNext();
					else
						setJob(110);
						giveSP(1);
						giveItem(4031012, -1); -- Take away The Proof of a Hero
						addSlots(2, 1); -- Add inventory slots
						addSlots(4, 1);
						newhp = 300 + getRandomNumber(50); -- Extra HP given
						setMaxHP(getRealMaxHP() + newhp); -- Add to current HP
						addText("Alright! You have now become the #bFighter#k! A fighter strives to become the strongest ");
						addText("of the strong, and never stops fighting. Don't ever lose that will to fight, and push forward 24/7. ");
						addText("It'll help you become even stronger than you already are.");
						sendNext();

						addText("I have just given you a book that gives you the list of skills you can acquire as the Fighter. ");
						addText("In that book, you'll find a bunch of skills the Fighter can learn. Your use and etc. inventories ");
						addText("have also been expanded with an additional row of slots also available. Your max MP has also ");
						addText("been increased...go check and see for it yourself.");
						sendBackNext();

						addText("I have also given you a little bit of #bSP#k. Open the #bSkill Menu#k located at the bottomleft ");
						addText("corner. You'll be able to boost up the newly-acquired 2nd level skills. A word of warning ");
						addText("though: You can't boost them up all at once. Some of the skills are only available after ");
						addText("you have learned other skills. Make sure to remember that.");
						sendBackNext();

						addText("Fighters have to be strong. But remember that you can't abuse that power and use it on a ");
						addText("weakling. Please use your enormous power the right way, because...for you to use that the right way, ");
						addText("that is much harder than just getting stronger. Find me after you have advanced much further.");
						sendBackNext();
					end
				end
			elseif choice == 1 then
				addText("So you want to make the 2nd job advancement as the #bPage#k? Once you make that decision ");
				addText("you can't go back and choose another job...do you still wanna do it?");
				yes = askYesNo();

				if yes == 0 then
					addText("Really? So you need to think about it a little more? Take your time...this is not something ");
					addText("that you should take lightly...let me know when you have made your decision, okay?");				
					sendNext();

				else
					if getSP() > ((getLevel() - 30) * 3) then
						addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many ");
						addText("SP in store. Use more SP on the skills on the 1st level and then come back.");
						sendNext();
					else
						setJob(120);
						giveSP(1);
						giveItem(4031012, -1); -- Take away The Proof of a Hero
						addSlots(2, 1); -- Add inventory slots
						addSlots(4, 1);
						newmp = 100 + getRandomNumber(50); -- Extra MP given
						setMaxMP(getRealMaxMP() + newmp); -- Add to current MP
						addText("Alright! You have now become the #bPage#k! The Pages have high intelligence and bravery for ");
						addText("a Warrior...here's hoping that you'll take the right path with the right mindset...I'll help you ");
						addText("become much stronger than you are right now.");
						sendNext();

						addText("I have just given you a book that gives you the list of skills you can acquire as the Page. ");
						addText("In that book, you'll find a bunch of skills the Fighter can learn. Your use and etc. inventories ");
						addText("have also been expanded with an additional row of slots also available. Your max MP has also ");
						addText("been increased...go check and see for it yourself.");
						sendBackNext();

						addText("I have also given you a little bit of #bSP#k. Open the #bSkill Menu#k located at the bottomleft ");
						addText("corner. You'll be able to boost up the newly-acquired 2nd level skills. A word of warning ");
						addText("you have learned other skills. Make sure to remember that.");
						sendBackNext();

						addText("Pages have to be strong. But remember that you can't abuse that power and use it on a ");
						addText("weakling. Please use your enormous power the right way, because...for you to use that the right way, ");
						addText("that is much harder than just getting stronger. Find me after you have advanced much further.");
						sendBackNext();
					end
				end
			elseif choice == 2 then 
				addText("So you want to make the 2nd job advancement as the #bSpearman#k? Once you make that decision ");
				addText("you can't go back and choose another job...do you still wanna do it?");
				yes = askYesNo();

				if yes == 0 then
					addText("Really? So you need to think about it a little more? Take your time...this is not something ");
					addText("that you should take lightly...let me know when you have made your decision, okay?");				
					sendNext();

				else
					if getSP() > ((getLevel() - 30) * 3) then
						addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many ");
						addText("SP in store. Use more SP on the skills on the 1st level and then come back.");
						sendNext();
					else
						setJob(130);
						giveSP(1);
						giveItem(4031012, -1); -- Take away The Proof of a Hero
						addSlots(2, 1); -- Add inventory slots
						addSlots(4, 1);
						newmp = 100 + getRandomNumber(50); -- Extra MP given
						setMaxMP(getRealMaxMP() + newmp); -- Add to current MP
						addText("Alright! You have now become the #bSpearman#k! The spearman use the power of darkness to take out ");
						addText("the enemies, always in shadows...please believe in yourself and your awesome power as you go on ");
						addText("in your journey...I'll help you become much stronger than you are right now.");
						sendNext();

						addText("I have just given you a book that gives you the list of skills you can acquire as the Spearman. ");
						addText("In that book, you'll find a bunch of skills the Fighter can learn. Your use and etc. inventories ");
						addText("have also been expanded with an additional row of slots also available. Your max MP has also ");
						addText("been increased...go check and see for it yourself.");
						sendBackNext();

						addText("I have also given you a little bit of #bSP#k. Open the #bSkill Menu#k located at the bottomleft ");
						addText("corner. You'll be able to boost up the newly-acquired 2nd level skills. A word of warning ");
						addText("though: You can't boost them up all at once. Some of the skills are only available after ");
						addText("you have learned other skills. Make sure to remember that.");
						sendBackNext();

						addText("Spearmen have to be strong. But remember that you can't abuse that power and use it on a ");
						addText("weakling. Please use your enormous power the right way, because...for you to use that the right way, ");
						addText("that is much harder than just getting stronger. Find me after you have advanced much further.");
						sendBackNext();
					end
				end
			end
		end
	end
else
	addText("Awesome body! Awesome power! Warriors are they way to go!!!! What do you think? ");
	addText("Want to make the job advancement as a Warrior??");
	sendNext();
end
