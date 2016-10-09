--[[
Copyright (C) 2008-2016 Vana Development Team

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
-- Spiegelmann (every town one)

dofile("scripts/utils/npcHelper.lua");

function generateExchangeChoice(itemId, coinQty)
	return makeChoiceData(" " .. altItemRef(itemId) .. "(" .. coinQty .. " coins)", {itemId, coinQty});
end

function exchangeCoins(data)
	itemId, qty = data[1], data[2];
	if getItemAmount(4001129) < qty or getOpenSlots(1) < 1 then
		addText("Check and see if you are either lacking " .. blue(itemRef(4001129)) .. " or if your Equipment inventory is full.");
		sendNext();
	else
		giveItem(4001129, -qty);
		giveItem(itemId, 1);
	end
end

choices = {
	makeChoiceHandler(" Go to the Monster Carnival Field.", function()
		if getLevel() > 50 or getLevel() < 30 then
			addText("I'm sorry, but only the users within Level 30~50 may participate in Monster Carnival.");
			sendOk();
		else
			setPlayerVariable("mc_origin", getMap());
			setMap(980000000, "st00");
		end
	end),
	makeChoiceHandler(" Learn about the Monster Carnival.", function()
		iterationBreak = 1;

		choices = {
			makeChoiceHandler(" What's a Monster Carnival?", function()
				addText("Haha! ");
				addText("I'm Spiegelmann, the leader of this traveling carnival. ");
				addText("I started the 1st ever " .. blue("Monster Carnival") .. " here, waiting for travelers like you to participate in this extravaganza!");
				sendNext();

				addText("What's a " .. blue("Monster Carnival") .. "? ");
				addText("Hahaha! ");
				addText("let's just say that it's an experience you will never forget! ");
				addText("It's a " .. blue("battle against other travelers like yourself!"));
				sendBackNext();

				addText("I know that it is way too dangerous for you to fight one another using real weapons; nor would I suggest such a barbaric act. ");
				addText("No my friend, what I offer is competition. ");
				addText("The thrill of battle and the excitement of competing against people just as strong and motivated as yourself. ");
				addText("I offer the premise that your party and the opposing party both " .. blue("summon monsters, and defeat the monsters summoned by the opposing party. "));
				addText(blue("That's the essence of the Monster Carnival. "));
				addText(blue("Also, you can use Maple Coins earned during the Monster Carnival to obtain new items and weapons!"));
				sendBackNext();

				addText("Of course, it's not as simple as that. ");
				addText("There are different ways to prevent the other party from hunting monsters, and it's up to you to figure out how. ");
				addText("What do you think? ");
				addText("Interested in a little friendly (or not-so-friendly) competition?");
				sendBackNext();
			end),
			makeChoiceHandler(" General overview of the Monster Carnival", function()
				addText(blue("Monster Carnival") .. " consists of 2 parties entering the battleground, and hunting the monsters summoned by the other party. ");
				addText("It's a " .. blue("combat quest that determines the victor by the amount of Carnival Points (CP) earned") .. ".");
				sendNext();

				addText("Once you enter the Carnival Field, the task is to " .. blue("earn CP by hunting monsters from the opposing party, and use those CP's to distract the opposing party from hunting monsters") .. ".");
				sendBackNext();

				addText("There are three ways to distract the other party: " .. blue("Summon a Monster, Skill, and Protector") .. ". ");
				addText("I'll give you a more in-depth look if you want to know more about 'Detailed Instructions'.");
				sendBackNext();

				addText("Please remember this, though. ");
				addText("It's never a good idea to save up CP just for the sake of it. ");
				addText(blue("The CP's you've used will also help determine the winner and the loser of the carnival") .. ".");
				sendBackNext();
			end),
			makeChoiceHandler(" Detailed instructions about the Monster Carnival", function()
				addText("Once you enter the Carnival Field, you'll see a Monster Carnival window appear. ");
				addText("All you have to do is " .. blue("select the ones you want to use, and press OK") .. ". ");
				addText("Pretty easy, right?");
				sendNext();

				addText("Once you get used to the commands, try using " .. blue("the Hotkeys TAB and F1 ~  F12") .. ". ");
				addText(blue("TAB toggles between Summoning Monsters/Skill/Protector,") .. " and, " .. blue("F1~ F12 allows you to directly enter one of the windows") .. ".");
				sendBackNext();

				addText(blue("Summon a Monster") .. " calls a monster that attacks the opposing party, under your control. ");
				addText("Use CP to bring out a Summoned Monster, and it'll appear in the same area, attacking the opposing party.");
				sendBackNext();

				addText(blue("Skill") .. " is an option of using skills such as Darkness, Weakness, and others to prevent the opposing party from defeating the monsters. ");
				addText("It requires a lot of CP, but it's well worth it. ");
				addText("The only problem is that it doesn't last that long. ");
				addText("Use this tactic wisely!");
				sendBackNext();

				addText(blue("Protector") .. " is basically a summoned item that greatly boosts the abilities of the monster summoned by your party. ");
				addText("Protector works as long as it's not demolished by the opposing party, so I suggest you summon a lot of monsters first, and then bring out the Protector.");
				sendBackNext();

				addText("Lastly, while you're in the Monster Carnival, " .. blue("you cannot use the recovery items/potions that you carry around with you.") .. " ");
				addText("However, the monsters will drop those items every once in a while, and " .. blue("as soon as you pick it up, the item will activate immediately") .. ". ");
				addText("That's why it's just as important to know WHEN to pick up those items.");
				sendBackNext();
			end),
			makeChoiceHandler(" Nothing, really. I've changed my mind.", function()
				return iterationBreak;
			end),
		};

		while true do
			addText("What do you want to do?\r\n");
			addText(blue(choiceRef(choices)));
			choice = askChoice();

			if selectChoice(choices, choice) == iterationBreak then
				break;
			end
		end
	end),
	makeChoiceHandler(" Trade " .. itemRef(4001129) .. ".", function()
		choices = {
			makeChoiceHandler(" " .. itemRef(1122007) .. "(50 coins)", function()
				exchangeCoins({1122007, 50});
			end),
			makeChoiceHandler(" " .. itemRef(2041211) .. "(40 coins)", function()
				exchangeCoins({2041211, 40});
			end),
			makeChoiceHandler(" Weapon for Warriors", function()
				page = 1;
				while true do
					choices = nil;
					if page == 1 then
						addText("Please make sure you have enough Maple Coins for the weapon you desire. ");
						addText("Select the weapon you'd like to trade Maple Coins for. ");
						addText("The selection I have is pretty good, if I do say so myself! \r\n");
						choices = {
							generateExchangeChoice(1302004, 7),
							generateExchangeChoice(1402006, 7),
							generateExchangeChoice(1302009, 10),
							generateExchangeChoice(1402007, 10),
							generateExchangeChoice(1302010, 20),
							generateExchangeChoice(1402003, 20),
							generateExchangeChoice(1312006, 7),
							generateExchangeChoice(1412004, 7),
							generateExchangeChoice(1312007, 10),
							generateExchangeChoice(1412005, 10),
							generateExchangeChoice(1312008, 20),
							generateExchangeChoice(1412003, 20),
							makeChoiceHandler(" Go to the Next Page(1/2)", function()
								page = 2;
							end),
						};
					else
						choices = {
							generateExchangeChoice(1322015, 7),
							generateExchangeChoice(1422008, 7),
							generateExchangeChoice(1322016, 10),
							generateExchangeChoice(1422007, 10),
							generateExchangeChoice(1322017, 20),
							generateExchangeChoice(1422005, 20),
							generateExchangeChoice(1432003, 7),
							generateExchangeChoice(1442003, 7),
							generateExchangeChoice(1432005, 10),
							generateExchangeChoice(1442009, 10),
							generateExchangeChoice(1442005, 20),
							generateExchangeChoice(1432004, 20),
							makeChoiceHandler(" Back to First Page(2/2)", function()
								page = 1;
							end),
						};
					end

					addText(blue(choiceRef(choices)));
					choice = askChoice();

					data = selectChoice(choices, choice);
					if data ~= nil then
						exchangeCoins(data);
						break;
					end
				end
			end),
			makeChoiceHandler(" Weapon for Magicians", function()
				choices = {
					generateExchangeChoice(1372001, 7),
					generateExchangeChoice(1382018, 7),
					generateExchangeChoice(1372012, 10),
					generateExchangeChoice(1382019, 10),
					generateExchangeChoice(1382001, 20),
					generateExchangeChoice(1372007, 20),
				};

				addText("Select the weapon you'd like to trade for. ");
				addText("The weapons I have here are extremely appealing. ");
				addText("See for yourself! \r\n");
				addText(blue(choiceRef(choices)));
				choice = askChoice();

				exchangeCoins(selectChoice(choices, choice));
			end),
			makeChoiceHandler(" Weapon for Bowmen", function()
				choices = {
					generateExchangeChoice(1452006, 7),
					generateExchangeChoice(1452007, 10),
					generateExchangeChoice(1452008, 20),
					generateExchangeChoice(1462005, 7),
					generateExchangeChoice(1462006, 10),
					generateExchangeChoice(1462007, 20),
				};

				addText("Select the weapon you'd like to trade for. ");
				addText("The weapons I have here are highly appealing. ");
				addText("See for yourself! \r\n");
				addText(blue(choiceRef(choices)));
				choice = askChoice();

				exchangeCoins(selectChoice(choices, choice));
			end),
			makeChoiceHandler(" Weapon for Thieves", function()
				choices = {
					generateExchangeChoice(1472013, 7),
					generateExchangeChoice(1472017, 10),
					generateExchangeChoice(1472021, 20),
					generateExchangeChoice(1332014, 7),
					generateExchangeChoice(1332031, 10),
					generateExchangeChoice(1332011, 10),
					generateExchangeChoice(1332016, 20),
					generateExchangeChoice(1332003, 20),
				};

				addText("Select the weapon you'd like to trade for. ");
				addText("The weapons I have here are of the highest quality. ");
				addText("Select what appeals to you! \r\n");
				addText(blue(choiceRef(choices)));
				choice = askChoice();

				exchangeCoins(selectChoice(choices, choice));
			end),
			makeChoiceHandler(" Weapon for Pirates", function()
				choices = {
					generateExchangeChoice(1482005, 7),
					generateExchangeChoice(1482006, 10),
					generateExchangeChoice(1482007, 20),
					generateExchangeChoice(1492005, 7),
					generateExchangeChoice(1492006, 10),
					generateExchangeChoice(1492007, 20),
				};

				addText("Select the weapon you'd like to trade for. ");
				addText("The weapons I have here are of the highest quality. ");
				addText("Select what appeals to you! \r\n");
				addText(blue(choiceRef(choices)));
				choice = askChoice();

				exchangeCoins(selectChoice(choices, choice));
			end),
		};

		addText("Remember, if you have Maple Coins, you can trade them in for items. ");
		addText("Please make sure you have enough Maple Coins for the item you want. ");
		addText("Select the item you'd like to trade for! \r\n");
		addText(blue(choiceRef(choices)));
		choice = askChoice();

		selectChoice(choices, choice);
	end),
};

addText("What would you like to do? ");
addText("If you have never participated in the Monster Carnival, you'll need to know a thing or two about it before joining.\r\n");
addText(blue(choiceRef(choices)));
choice = askChoice();

selectChoice(choices, choice);