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
-- Dark Lord - Thief Instructor

dofile("scripts/lua_functions/jobFunctions.lua");

if isQuestActive(6141) then
	addText("Would you like to go to the training ground?");
	yes = askYesNo();
	if yes == 0 then
		addText("Let me know when you want to enter.");
		sendOK();
	else
		if not isInstance("ninjaAmbush") then
			createInstance("ninjaAmbush", 5 * 60, true);
			addInstancePlayer(getID());
			setMap(910300000);
		else
			addText("Someone is already inside. Try again later.");
			sendOK();
		end
	end
elseif getJob() == 0 then
	addText("Want to be a thief? There are some standards to meet, because we can't just accept ");
	addText("EVERYONE in ... #bYour level should be at least 10#k. ");
	addText("Let's see...");
	sendNext();

	if (getLevel() >= 10) then
		addText("Oh...! You look like someone that can definitely be a part of us...all you need is ");
		addText("a little sinister mind, and...yeah...so, what do you think? Wanna be the Rouge?");
		yes = askYesNo();

		if yes == 1 then
			addText("Alright, from here on out, you are part of us! You'll be living the life of a wanderer ");
			addText("at first, but just be patient and soon, you'll be living the high life. Alright, it ");
			addText("ain't much, but I'll give you some of my abilities...HAAAHHH!!");
			sendNext();

			if getLevel() >= 30 then 
				addText("I think you've made the job advancement way too late. Usually, for beginners under Level 29 ");
				addText("that were late in making job advancements, we compensate them with lost Skill Points, ");
				addText("that weren't rewarded, but...I think you're a little too late for that. I am so sorry, but there's nothing I can do.");
				sendBackNext();

				giveSP(1);
			else 
				giveSP((getLevel() - 10) * 3 + 1); -- Make up any SP for over-leveling like in GMS
			end

			setJob(400);
			giveItem(1472061, 1); -- Beginners Garnier
			giveItem(1332063, 1); -- Beginners Dagger
			giveItem(2070015, 3); -- Special subis
			hpinc = 100 + getRandomNumber(50); -- Generate a random number from 0-50 to add to the base hp increase
			mpinc = 25 + getRandomNumber(25); -- Generate a random number from 0-25 to add to the base mp increase
			setMaxHP(getRealMaxHP() + hpinc); -- Set HP to current HP plus full amount of hp increase
			setMaxMP(getRealMaxMP() + mpinc); -- Set MP to current MP plus full amount of mp increase
			setSTR(4); -- Stat reset
			setDEX(25);
			setINT(4);
			setLUK(4);
			setAP((getLevel() - 1) * 5 - 12);
			addSlots(1, 1); -- Add inventory slots
			addSlots(4, 1);
			addText("I've just created more slots for your equipment and etc. storage. Not only that, but you've also ");
			addText("gotten stronger as well. As you become part of us, and learn to enjoy life in different angles, ");
			addText("you may one day be on top of this of darkness. I'll be watching your every move, so don't let me down.");
			sendNext();

			addText("I just gave you a little bit of #bSP#k. When you open up the #bSkill menu#k on the lower left ");
			addText("corner of the screen, there are skills you can learn by using SP's. One warning, though: You ");
			addText("can't raise it all together all at once. There are also skills you can acquire only after having ");
			addText("learned a couple of skills first.");
			sendNext();

			addText("One more warning. Once you have chosen your job, try to stay alive as much as you can. Once you ");
			addText("reach that level, when you die, you will lose your experience level. You wouldn't want to lose ");
			addText("your hard-earned experience points, do you?");
			sendBackNext();

			addText("OK! This is all I can teach you. Go to places, train and better yourself. Find me when you feel ");
			addText("like you've done all you can, and need something interesting. Then, and only then, I'll hook you ");
			addText("up with more interesting experiences...");
			sendBackNext();

			addText("Oh, and... if you have any questions about being the Thief, feel free to ask. I don't know ");
			addText("EVERYTHING, but I'll help you out with all that I know of. Til then...");
			sendBackNext();
		else
			addText("I see...well, it's a very important step to take, choosing your job. But don't you ");
			addText("want to live the fun life? let me know when you have made up your mind, ok?");
			sendNext();
		end
	else
		addText("Hmm...you're still a beginner...doesn't look like you can hang out with us, yet... ");
		addText("make yourself much stronger, THEN find me...");
		sendBackNext();
	end
elseif (getJobLine() == 4 and getJobTrack() > 0) then
	addText("Do you have anything you want to know about thieves?\r\n");
	addText("#L0##bWhat are the basic characters of a Thief?#k#l\r\n");
	addText("#L1##bWhat are the weapons that the Thieves use?#k#l\r\n");
	addText("#L2##bWhat are the armors that the Thieves use?#k#l\r\n");
	addText("#L3##bWhat are the skills available for Thieves?#k#l");
	explain = askChoice();

	if explain == 0 then
		addText("Let me explain to you what being a thief means. Thieves have just the right amount of stamina and ");
		addText("strength to survive. We don't recommend training for strength just like those warriors. What we do ");
		addText("need are LUK and DEX...");
		sendNext();

		addText("If you raise the level of luck and dexterity, that will increase the damage you'll dish out to the ");
		addText("enemies. Thieves also differentiate themselves from the rest by using such throwing weapons as ");
		addText("throwing stars and throwing knives. They can also avoid many attacks with high dexterity.");
		sendBackNext();
	elseif explain == 1 then
		addText("Thieves use these weapons. They have just the right amount of intelligence and power...what we do have ");
		addText("are quick movements and even quick brain...");
		sendNext();

		addText("Because of that, we usually use daggers or throwing weapons. Daggers are basically small swords but ");
		addText("they are very quick, enabling you to squeeze in many attacks. If you fight in a close combat, use ");
		addText("the dagger to quickly eliminate the enemy before it reacts to your attack.");
		sendBackNext();

		addText("For throwing weapons there are throwing-stars and throwing-knives available. You can't just use them ");
		addText("by themselves, though. Go to the weapon store at Kerning City, and they'll sell an claw call Garnier. ");
		addText("Equip yourself with it, then you'll be able to throw the throwing stars that's in the use inventory.");
		sendBackNext();

		addText("Not only is it important to choose the right Claw to use, but it's just as important to choose the ");
		addText("right kind of throwing stars to use. Do you want to know where to get those stars? Go check out the ");
		addText("armor store around this town...there's probably someone that handles those specifically...");
		sendBackNext();
	elseif explain == 2 then
		addText("Let me explain to you the armors that the thieves can wear. Thieves value quick so you need clothes that ");
		addText("fit you perfectly. But then again, they don't need chained armors like the bowman, because it won't help ");
		addText("you one bit.");
		sendNext();

		addText("Instead of flashy clothes or tough, hard gold-plated armor, try putting on simple, comfortable clothes ");
		addText("that fit you perfectly and still do its job in the process. It'll help you a lot in hunting the monsters.");
		sendBackNext();
	elseif explain == 3 then
		addText("For thieves there are skills that supports high accuracy and dexterity we have in general. A good mix of ");
		addText("skills are available, for both the throwing stars and daggers. Choose your weapon carefully, for skills ");
		addText("are determined by it.");
		sendNext();

		addText("If you are using throwing-stars, skills like #bKeen Eyes#k or #bLucky Seven#k are perfect. Lucky Seven ");
		addText("allows you to throw multiple throwing-stars at once, so it'll help you greatly in hunting down enemies.");
		sendBackNext();

		addText("If you are using daggers choose #bDisorder#k or #bDouble Stab#k as skills. Double Stab, in fact, will be ");
		addText("a great skills to use in that it enables you to attack an enemy in a blinding flurry of stabs, a definate ");
		addText("stunner indeed.");
		sendBackNext();
	end
elseif (getJob() == 400 and getLevel() >= 30) then
	if (getItemAmount(4031011) == 0 and getItemAmount(4031012) == 0) then
		addText("Hmmm...you seem to have gotten a whole lot stronger. You got rid of the old, weak self and and look much ");
		addText("more like a thief now. Well, what do you think? Don't you want to get even more powerful than that? ");
		addText("Pass a simple test and I'll do just that for you. Do you want to do it?");
		yes = askYesNo();

		if yes == 0 then 
			addText("Really? It will help you out a great deal on your journey if you get stronger fast...if you choose to ");
			addText("change your mind in the future, please feel free to come back. Know that I'll make you much more ");
			addText("powerful than you are right now.");
			sendNext();
		else
			addText("Good thinking. But, I need to make sure you are as strong as you look. It's not a hard test, one that ");
			addText("should be easy for you to pass. First, take this letter...make sure you don't lose it.");
			sendNext();

			giveItem(4031011, 1);
			addText("Please take this letter to #b#p1072003##k around #b#m102040000##k ");
			addText("near Kerning City. He's doing the job of an instructor in place of me. Give him the letter and he'll give you ");
			addText("the test for me. If you want more details, hear it straight from him. I'll be wishing you good luck.");
			sendBackNext();
		end
	elseif (getItemAmount(4031012) == 0 and getItemAmount(4031011) > 0) then
		addText("Still haven't met the person yet? Find #b#p1072003##k who's around #b#m102040000##k ");
		addText("near Kerning City. Give the letter to him and he may let you know what to do.");
		sendNext();
	elseif (getItemAmount(4031012) > 0 and getItemAmount(4031011) == 0) then 
		addText("Hmmm...so you got back here safely. I knew that test would be too easy for you. I admit, you are a great ");
		addText("great thief. Now...I'll make you even more powerful than you already are. But, before all that...you need to ");
		addText("choose one of two ways. It'll be a difficult decision for you to make, but...if you have any questions, please ask.");
		sendNext();

		addText("Alright, when you have made your decision, click on [I'll choose my occupation!] at the very bottom...\r\n");
		addText("#L0##bPlease explain the characteristics of the Assassin.#k#l\r\n");
		addText("#L1##bPlease explain the characteristics of the Bandit.#k#l\r\n");
		addText("#L2##bI'll choose my occupation!#k#l\r\n");
		choice = askChoice();
		if choice == 0 then 
			addText("Let me explain the role of the Assassin. Assassin is the Thief that uses throwing stars or daggers. Skills like ");
			addText("#bClaw Mastery#k and #bCritical Throw#k will help you use your throwing stars better. Boost Claw Mastery up ");
			addText("more and your maximum number of throwing stars increases, so it'll be best to learn it. Please remember that.");
			sendNext();

			addText("I'll explain to you one of the skills of the Assassin, #bHaste#k. It temporarily boost up you and your party ");
			addText("members' abilities and moving speed, perfect when facing enemies that are really fast. It's also useful when ");
			addText("walking to a place far far away. Wouldn't it be much nicer to get to your destination on time as opposed to ");
			addText("taking a whole day just to get there?");
			sendBackNext();

			addText("And this is the over skill available for the Assassin, #bDrain#k. It allows you to take back a portion of the ");
			addText("damage you dished out on an enemy and absorb it as HP! The more the damage, the more you'll regain health...how ");
			addText("awesome is that? Remember the most you can absorb at once is half of your maximum HP. The higher the enemy's HP, ");
			addText("the more you can take away.");
			sendBackNext();
		elseif choice == 1 then
			addText("This is what being the Bandit is all about. Bandits are thieves who specialize in using daggers. Skills like ");
			addText("#bDagger Mastery#k and #bDagger Booster#k will help you use your dagger better. Daggers have quick attacking speed ");
			addText("to begin with, and if you add that with a booster, then...oh my! Fast enough to scare the crap out of the monsters!");
			sendNext();

			addText("I'll explain to you what #bSteal#k does for Bandits. It gives you a certain probability to let you steal an item ");
			addText("from an enemy. You may only steal once from one enemy, but you can keep trying until you succeed from it. The stolen ");
			addText("item will be dropped onto the ground; make sure you pick it up first because it's anyone's to grab once it's dropped.");
			sendBackNext();

			addText("I'll explain to you what #bSavage Blow#k does for Bandits. It uses up HP and MP to attack the enemy 6 TIMES ");
			addText("with the dagger. The higher the skill level, the more the attacks may occur. You'll cut up the enemy to pieces ");
			addText("with the dagger...ooooh, isn't it sweet? What do you think? Want to become a Bandit and feel the adrenaline rush that comes with it?");
			sendBackNext();
		elseif choice == 2 then
			addText("Hmmm, have you made up your mind? Then choose the 2nd job advancement of your liking.\r\n");
			addText("#L0##bAssassin#k#l\r\n");
			addText("#L1##bBandit#k#l\r\n");
			job = askChoice();

			if job == 0 then
				addText("So you want to make the 2nd job advancement as the #bAssassin#k? Once you have made the decision, ");
				addText("you can't go back and change your mind. You ARE sure about this, right?");
				yes = askYesNo();

				if yes == 0 then 
					addText("Really? Have to give more thought to it, huh? Take your time, take your time. ");
					addText("This is not something you should take lightly ... come talk to me once you have made your decision.");				
					sendNext();
				else
					if getSP() > ((getLevel() - 30) * 3) then
						addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many ");
						addText("SP in store. Use more SP on the skills on the 1st level and then come back.");
						sendNext();
					else
						setJob(410);
						giveSP(1);
						giveItem(4031012, -1); -- Take The Proof of a Hero
						addSlots(2, 1); -- Add inventory slots
						newhp = 300 + getRandomNumber(50); -- Extra HP calculated 
						setMaxHP(getRealMaxHP() + newhp); -- Add to current HP
						newmp = 150 + getRandomNumber(50); -- Extra MP calculated
						setMaxMP(getRealMaxMP() + newmp); -- Add to current MP
						addText("Alright, from here on out you are the #bAssassin#k. Assassins revel in shadows and darkness, waiting until ");
						addText("the right time comes for them to stick a dagger through the enemy's heart, suddenly and swiftly...please ");
						addText("keep training. I'll make you even more powerful than you are right now!");
						sendNext();

						addText("I have just given you a book that gives you the the list of skills you can acquire as an assassin. I have ");
						addText("also added a whole row to your use inventory, along with boosting up your max HP and MP...go see for it yourself.");
						sendBackNext();

						addText("I have also given you a little bit of #bSP#k. Open the #bSkill Menu#k located at the bottomleft ");
						addText("corner. You'll be able to boost up the newly-acquired 2nd level skills. A word of warning ");
						addText("though: You can't boost them up all at once. Some of the skills are only available after ");
						addText("you have learned other skills. Make sure to remember that.");
						sendBackNext();

						addText("Assassins have to be strong. But remember that you can't abuse that power and use it on a ");
						addText("weakling. Please use your enormous power the right way, because...for you to use that the right way, ");
						addText("that is much harder than just getting stronger. Find me after you have advanced much further.");
						sendBackNext();
					end
				end
			elseif job == 1 then 
				addText("So you want to make the 2nd job advancement as the #bBandit#k? Once you have made the decision, ");
				addText("you can't go back and change your mind. You ARE sure about this, right?");
				yes = askYesNo();

				if yes == 0 then 
					addText("Really? Have to give more thought to it, huh? Take your time, take your time. ");
					addText("This is not something you should take lightly ... come talk to me once you have made your decision.");				
					sendNext();
				else
					if getSP() > ((getLevel() - 30) * 3) then
						addText("Hmmm...you have too much SP...you can't make the 2nd job advancement with that many ");
						addText("SP in store. Use more SP on the skills on the 1st level and then come back.");
						sendNext();
					else
						setJob(420);
						giveSP(1); -- Give necessary SP
						giveItem(4031012, -1); -- Take The Proof of a Hero
						addSlots(2, 1); -- Add inventory slots
						newhp = 300 + getRandomNumber(50); -- Extra HP calculated
						setMaxHP(getRealMaxHP() + newhp); -- Add to current HP 
						newmp = 150 + getRandomNumber(50); -- Extra MP calculared
						setMaxMP(getRealMaxMP() + newmp); -- Add to current MP
						addText("Alright from here on out, you're the #bBandit#k. Bandits have quick hands and quicker feet to ");
						addText("dominate the enemies. Please keep training. I'll make you even more powerful than you are right now.");
						sendNext();

						addText("I have just given you a book that gives you the the list of skills you can acquire as an assassin. I have ");
						addText("also added a whole row to your use inventory, along with boosting up your max HP and MP...go see for it yourself.");
						sendBackNext();

						addText("I have also given you a little bit of #bSP#k. Open the #bSkill Menu#k located at the bottomleft ");
						addText("corner. You'll be able to boost up the newly-acquired 2nd level skills. A word of warning ");
						addText("though: You can't boost them up all at once. Some of the skills are only available after ");
						addText("you have learned other skills. Make sure to remember that.");
						sendBackNext();

						addText("Assassins have to be strong. But remember that you can't abuse that power and use it on a ");
						addText("weakling. Please use your enormous power the right way, because...for you to use that the right way, ");
						addText("that is much harder than just getting stronger. Find me after you have advanced much further.");
						sendBackNext();
					end
				end
			end
		end
	end
else
	addText("Exploring is good, and getting stronger is good and all... but don't you want to enjoy ");
	addText("living the life as you know it? How about becoming a Rouge like us and really LIVE the ");
	addText("life? Sounds fun, isn't it?");
	sendNext();
end
