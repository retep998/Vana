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
-- NimaKIN

dofile("scripts/utils/npcHelper.lua");

if isGm() then
	choices = {
		makeChoiceHandler("I would like my inventory slots maxed", function()
			addSlots(1, 25);
			addSlots(2, 25);
			addSlots(3, 25);
			addSlots(4, 25);
			addSlots(5, 25);
		end),
		makeChoiceHandler("I would like my skills maxed", function()
			skills = {
				-- Beginner
				{8, 1}, -- Follow the Lead
				{12, 20}, -- Blessing of the Fairy
				{1003, 1}, -- Legendary Spirit
				{1007, 1}, -- Maker
				{1004, 1}, -- Monster Rider
				{1005, 1}, -- Echo of Hero
				{1002, 3}, -- Nimble Feet
				{1001, 3}, -- Recovery
				{1000, 3}, -- Three Snails
				-- Warrior (1st job)
				{1000002, 8}, -- Endure
				{1000000, 16}, -- Improving HP Recovery
				{1000001, 10}, -- Improving MaxHP Increase
				{1001003, 20}, -- Iron Body
				{1001004, 20}, -- Power Strike
				{1001005, 20}, -- Slash Blast
				-- Magician (1st Job)
				{2001004, 20}, -- Energy Bolt
				{2000001, 10}, -- Improving Max MP Increase
				{2000000, 16}, -- Improving MP recovery
				{2001003, 20}, -- Magic Armor
				{2001005, 20}, -- Magic Claw
				{2001002, 20}, -- Magic Guard
				-- Bowman (1st Job)
				{3001004, 20}, -- Arrow Blow
				{3000001, 20}, -- Critical Shot
				{3001005, 20}, -- Double Shot
				{3001003, 20}, -- Focus
				{3000000, 16}, -- The Blessing of Amazon
				{3000002, 8}, -- The Eye of Amazon
				-- Thief (1st Job)
				{4001003, 20}, -- Dark Sight
				{4001002, 20}, -- Disorder
				{4001334, 20}, -- Double Stab
				{4000001, 8}, -- Keen Eyes
				{4001344, 20}, -- Lucky Seven
				{4000000, 20}, -- Nimble Body
				-- Pirate (1st Job)
				{5000000, 20}, -- Bullet Time
				{5001001, 20}, -- Flash Fist
				{5001002, 20}, -- Sommersault Kick
				{5001003, 20}, -- Double Shot
				{5001005, 10}, -- Dash
				-- Fighter (2nd Job)
				{1101005, 20}, -- Axe Booster
				{1100001, 20}, -- Axe Mastery
				{1100003, 30}, -- Final Attack : Axe
				{1100002, 30}, -- Final Attack : Sword
				{1101007, 30}, -- Power Guard
				{1101006, 20}, -- Rage
				{1101004, 20}, -- Sword Booster
				{1100000, 20}, -- Sword Mastery
				-- Page (2nd Job)
				{1201005, 20}, -- BW Booster
				{1200001, 20}, -- BW Mastery
				{1200003, 30}, -- Final Attack : BW
				{1200002, 30}, -- Final Attack : Sword
				{1201007, 30}, -- Power Guard
				{1201004, 20}, -- Sword Booster
				{1200000, 20}, -- Sword Mastery
				{1201006, 20}, -- Threaten
				-- Spearman (2nd Job)
				{1300003, 30}, -- Final Attack : Pole Arm
				{1300002, 30}, -- Final Attack : Spear
				{1301007, 30}, -- Hyper Body
				{1301006, 20}, -- Iron Will
				{1301005, 20}, -- Pole Arm Booster
				{1300001, 20}, -- Pole Arm Mastery
				{1301004, 20}, -- Spear Booster
				{1300000, 20}, -- Spear Mastery
				-- Fire/Poison Wizard (2nd Job)
				{2101004, 30}, -- Fire Arrow
				{2101001, 20}, -- Meditation
				{2100000, 20}, -- MP Eater
				{2101005, 30}, -- Poison Brace
				{2101003, 20}, -- Slow
				{2101002, 20}, -- Teleport
				-- Ice/Lightning Wizard (2nd Job)
				{2201004, 30}, -- Cold Beam
				{2201001, 20}, -- Meditation
				{2200000, 20}, -- MP Eater
				{2201003, 20}, -- Slow
				{2201002, 20}, -- Teleport
				{2201005, 30}, -- Thunderbolt
				-- Cleric (2nd Job)
				{2301004, 20}, -- Bless
				{2301002, 30}, -- Heal
				{2301005, 30}, -- Holy Arrow
				{2301003, 20}, -- Invincible
				{2300000, 20}, -- MP Eater
				{2301001, 20}, -- Teleport
				-- Hunter (2nd Job)
				{3101005, 30}, -- Arrow Bomb : Bow
				{3101002, 20}, -- Bow Booster
				{3100000, 20}, -- Bow Mastery
				{3100001, 30}, -- Final Attack : Bow
				{3101003, 20}, -- Power Knock-Back
				{3101004, 20}, -- Soul Arrow : Bow
				-- Crossbowman (2nd Job)
				{3201002, 20}, -- Crossbow Booster
				{3200000, 20}, -- Crossbow Mastery
				{3200001, 30}, -- Final Attack : Crossbow
				{3201005, 30}, -- Iron Arrow : Crossbow
				{3201003, 20}, -- Power Knock-Back
				{3201004, 20}, -- Soul Arrow : Crossbow
				-- Assassin (2nd Job)
				{4101003, 20}, -- Claw Booster
				{4100000, 20}, -- Claw Mastery
				{4100001, 30}, -- Critical Throw
				{4101005, 30}, -- Drain
				{4100002, 20}, -- Endure
				{4101004, 20}, -- Haste
				-- Bandit (2nd Job)
				{4201002, 20}, -- Dagger Booster
				{4200000, 20}, -- Dagger Mastery
				{4200001, 20}, -- Endure
				{4201003, 20}, -- Haste
				{4201005, 30}, -- Savage Blow
				{4201004, 30}, -- Steal
				-- Buccaneer (2nd Job)
				{5100000, 10}, -- Improve MaxHP
				{5100001, 20}, -- Knuckler Mastery
				{5101006, 20}, -- Knuckler Booster
				{5101002, 20}, -- Backspin Blow
				{5101003, 20}, -- Double Uppercut
				{5101004, 20}, -- Corkscrew Blow
				{5101005, 10}, -- MP Recovery
				{5101007, 10}, -- Oak Barrel
				-- Gunslinger (2nd Job)
				{5200000, 20}, -- Gun Mastery
				{5201001, 20}, -- Invisible Shot
				{5201002, 20}, -- Grenade
				{5201003, 20}, -- Gun Booster
				{5201004, 20}, -- Blank Shot
				{5201005, 10}, -- Wings
				{5201006, 20}, -- Recoil Shot
				-- Crusader (3rd Job)
				{1111007, 20}, -- Armor Crash
				{1111006, 30}, -- Coma: Axe
				{1111005, 30}, -- Coma: Sword
				{1111002, 30}, -- Combo Attack
				{1110000, 20}, -- Improving MP recovery
				{1111004, 30}, -- Panic: Axe
				{1111003, 30}, -- Panic: Sword
				{1110001, 20}, -- Shield Mastery
				{1111008, 30}, -- Shout
				--White Knight (3rd Job)
				{1211006, 30}, -- Blizzard Charge: BW
				{1211002, 30}, -- Charged Blow
				{1211004, 30}, -- Fire Charge: BW
				{1211003, 30}, -- Fire Charge: Sword
				{1211005, 30}, -- Ice Charge: Sword
				{1210000, 20}, -- Improving MP recovery
				{1211008, 30}, -- Lightning Charge: BW
				{1211009, 20}, -- Magic Crash
				{1210001, 20}, -- Shield Mastery
				{1211007, 30}, -- Thunder Charge: Sword
				-- Dragon Knight (3rd Job)
				{1311008, 20}, -- Dragon Blood
				{1311004, 30}, -- Dragon Fury: Pole Arm
				{1311003, 30}, -- Dragon Fury: Spear
				{1311006, 30}, -- Dragon Roar
				{1310000, 20}, -- Elemental Resistance
				{1311002, 30}, -- Pole Arm Crusher
				{1311007, 20}, -- Power Crash
				{1311005, 30}, -- Sacrifice
				{1311001, 30}, -- Spear Crusher
				-- Fire/Poison Mage (3rd Job)
				{2110001, 30}, -- Element Amplification
				{2111006, 30}, -- Element Composition
				{2111002, 30}, -- Explosion
				{2110000, 20}, -- Partial Resistance
				{2111003, 30}, -- Poison Mist
				{2111004, 20}, -- Seal
				{2111005, 20}, -- Spell Booster
				-- Ice/Lightning Mage (3rd Job)
				{2210001, 30}, -- Element Amplification
				{2211006, 30}, -- Element Composition
				{2211002, 30}, -- Ice Strike
				{2210000, 20}, -- Partial Resistance
				{2211004, 20}, -- Seal
				{2211005, 20}, -- Spell Booster
				{2211003, 30}, -- Thunder Spear
				-- Priest (3rd Job)
				{2311001, 20}, -- Dispel
				{2311005, 30}, -- Doom
				{2310000, 20}, -- Elemental Resistance
				{2311003, 30}, -- Holy Symbol
				{2311002, 20}, -- Mystic Door
				{2311004, 30}, -- Shining Ray
				{2311006, 30}, -- Summon Dragon
				-- Ranger (3rd Job)
				{3111004, 30}, -- Arrow Rain
				{3111003, 30}, -- Inferno
				{3110001, 20}, -- Mortal Blow
				{3111002, 20}, -- Puppet
				{3111005, 30}, -- Silver Hawk
				{3111006, 30}, -- Strafe
				{3110000, 20}, -- Thrust
				-- Sniper (3rd Job)
				{3211004, 30}, -- Arrow Eruption
				{3211003, 30}, -- Blizzard
				{3211005, 30}, -- Golden Eagle
				{3210001, 20}, -- Mortal Blow
				{3211002, 20}, -- Puppet
				{3211006, 30}, -- Strafe
				{3210000, 20}, -- Thrust
				-- Hermit (3rd Job)
				{4110000, 20}, -- Alchemist
				{4111005, 30}, -- Avenger
				{4111006, 20}, -- Flash Jump
				{4111001, 20}, -- Meso Up
				{4111004, 30}, -- Shadow Meso
				{4111002, 30}, -- Shadow Partner
				{4111003, 20}, -- Shadow Web
				-- Chief Bandit (3rd Job)
				{4211002, 30}, -- Assaulter
				{4211004, 30}, -- Band of Thieves
				{4211001, 30}, -- Chakra
				{4211006, 30}, -- Meso Explosion
				{4211005, 20}, -- Meso Guard
				{4211003, 20}, -- Pickpocket
				{4210000, 20}, -- Shield Mastery
				-- Marauder (3rd Job)
				{5110000, 20}, -- Stun Mastery
				{5110001, 40}, -- Energy Charge
				{5111002, 30}, -- Energy Blast
				{5111004, 20}, -- Energy Drain
				{5111005, 20}, -- Transformation
				{5111006, 30}, -- Shockwave
				-- Outlaw (3rd Job)
				{5210000, 20}, -- Burst Fire
				{5211001, 30}, -- Octopus
				{5211002, 30}, -- Gaviota
				{5211004, 30}, -- Flamethrower
				{5211005, 30}, -- Ice Splitter
				{5211006, 30}, -- Homing Beacon
				-- Hero (4th Job)
				{1120004, 30, 30}, -- Achilles
				{1120003, 30, 30}, -- Advanced Combo
				{1121011, 5, 5}, -- Hero's Will
				{1120005, 30, 30}, -- Blocking
				{1121008, 30, 30}, -- Brandish
				{1121010, 30, 30}, -- Enrage
				{1121000, 30, 30}, -- Maple Warrior
				{1121001, 30, 30}, -- Monster Magnet
				{1121006, 30, 30}, -- Rush
				{1121002, 30, 30}, -- Stance
				-- Paladin (4th Job)
				{1220005, 30, 30}, -- Achilles
				{1220010, 10, 10}, -- Advanced Charge
				{1221012, 5, 5}, -- Hero's Will
				{1221009, 30, 30}, -- Blast
				{1220006, 30, 30}, -- Blocking
				{1221004, 20, 20}, -- Divine Charge: Mace
				{1221003, 20, 20}, -- Holy Charge: Sword
				{1221000, 30, 30}, -- Maple Warrior
				{1221001, 30, 30}, -- Monster Magnet
				{1221007, 30, 30}, -- Rush
				{1221011, 30, 30}, -- Sanctuary
				{1221002, 30, 30}, -- Stance
				-- Dark Knight (4th Job)
				{1320005, 30, 30}, -- Achilles
				{1321010, 5, 5}, -- Hero's Will
				{1321007, 10, 10}, -- Beholder
				{1320009, 25, 25}, -- Beholder's Buff
				{1320008, 25, 25}, -- Beholder's Healing
				{1320006, 30, 30}, -- Berserk
				{1321000, 30, 30}, -- Maple Warrior
				{1321001, 30, 30}, -- Monster Magnet
				{1321003, 30, 30}, -- Rush
				{1321002, 30, 30}, -- Stance
				-- Fire/Poison Arch Mage (4th Job)
				{2121008, 5, 5}, -- Hero's Will
				{2121001, 30, 30}, -- Big Bang
				{2121005, 30, 30}, -- Elquines
				{2121003, 30, 30}, -- Fire Demon
				{2121004, 30, 30}, -- Infinity
				{2121002, 30, 30}, -- Mana Reflection
				{2121000, 30, 30}, -- Maple Warrior
				{2121007, 30, 30}, -- Meteo
				{2121006, 30, 30}, -- Paralyze
				-- Ice/Lightning Arch Mage (4th Job)
				{2221008, 5, 5}, -- Hero's Will
				{2221001, 30, 30}, -- Big Bang
				{2221007, 30, 30}, -- Blizzard
				{2221006, 30, 30}, -- Chain Lightning
				{2221003, 30, 30}, -- Ice Demon
				{2221005, 30, 30}, -- Ifrit
				{2221004, 30, 30}, -- Infinity
				{2221002, 30, 30}, -- Mana Reflection
				{2221000, 30, 30}, -- Maple Warrior
				-- Bishop (4th Job)
				{2321007, 30, 30}, -- Angel's Ray
				{2321009, 5, 5}, -- Hero's Will
				{2321003, 30, 30}, -- Bahamut
				{2321001, 30, 30}, -- Big Bang
				{2321008, 30, 30}, -- Genesis
				{2321005, 30, 30}, -- Holy Shield
				{2321004, 30, 30}, -- Infinity
				{2321002, 30, 30}, -- Mana Reflection
				{2321000, 30, 30}, -- Maple Warrior
				{2321006, 10, 10}, -- Resurrection
				-- Bow Master (4th Job)
				{3121009, 5, 5}, -- Hero's Will
				{3120005, 30, 30}, -- Bow Expert
				{3121008, 30, 30}, -- Concentration
				{3121003, 30, 30}, -- Dragon Pulse
				{3121007, 30, 30}, -- Hamstring
				{3121000, 30, 30}, -- Maple Warrior
				{3121006, 30, 30}, -- Phoenix
				{3121002, 30, 30}, -- Sharp Eyes
				{3121004, 30, 30}, -- Storm Arrow
				-- Crossbow Master (4th Job)
				{3221008, 5, 5}, -- Hero's Will
				{3221006, 30, 30}, -- Blind
				{3220004, 30, 30}, -- Crossbow Expert
				{3221003, 30, 30}, -- Dragon Pulse
				{3221005, 30, 30}, -- Freezer
				{3221000, 30, 30}, -- Maple Warrior
				{3221001, 30, 30}, -- Piercing
				{3221002, 30, 30}, -- Sharp Eyes
				{3221007, 30, 30}, -- Sniping
				-- Night Lord (4th Job)
				{4121009, 5, 5}, -- Hero's Will
				{4120002, 30, 30}, -- Shadow Shifter
				{4121000, 30, 30}, -- Maple Warrior
				{4121004, 30, 30}, -- Ninja Ambush
				{4121008, 30, 30}, -- Ninja Storm
				{4121003, 30, 30}, -- Taunt
				{4121006, 30, 30}, -- Spirit Claw
				{4121007, 30, 30}, -- Triple Throw
				{4120005, 30, 30}, -- Venomous Star
				-- Shadower (4th Job)
				{4221001, 30, 30}, -- Assassinate
				{4221008, 5, 5}, -- Hero's Will
				{4221007, 30, 30}, -- Boomerang Step
				{4220002, 30, 30}, -- Shadow Shifter
				{4221000, 30, 30}, -- Maple Warrior
				{4221004, 30, 30}, -- Ninja Ambush
				{4221003, 30, 30}, -- Taunt
				{4221006, 30, 30}, -- Smokescreen
				{4220005, 30, 30}, -- Venomous Dagger
				-- Buccaneer (4th Job)
				{5121000, 30, 30}, -- Maple Warrior
				{5121001, 30, 30}, -- Dragon Strike
				{5121002, 30, 30}, -- Energy Orb
				{5121003, 20, 20}, -- Super Transformation
				{5121004, 30, 30}, -- Demolition
				{5121005, 30, 30}, -- Snatch
				{5121007, 30, 30}, -- Barrage
				{5121008, 5, 5}, -- Pirate's Rage
				{5121009, 20, 20}, -- Speed Infusion
				{5121010, 30, 30}, -- Time Leap
				-- Corsair (4th Job)
				{5220001, 30, 30}, -- Elemental Boost
				{5220002, 20, 20}, -- Wrath of the Octopi
				{5220011, 20, 20}, -- Bullseye
				{5221000, 30, 30}, -- Maple Warrior
				{5221003, 30, 30}, -- Aerial Strike
				{5221004, 30, 30}, -- Rapid Fire
				{5221006, 10, 10}, -- Battleship
				{5221007, 30, 30}, -- Battleship Cannon
				{5221008, 30, 30}, -- Battleship Torpedo
				{5221009, 20, 20}, -- Hypnotize
				{5221010, 5, 5}, -- Speed Infusion
				-- Manager Skills
				{8001000, 1}, -- Macro test
				{8001001, 1}, -- Teleport
				-- GM Skills
				{9001000, 1}, -- Haste (Normal)
				{9001001, 1}, -- Super Dragon Roar
				{9001002, 1}, -- Teleport
				-- SuperGM Skills
				{9101000, 1}, -- Heal + Dispel
				{9101001, 1}, -- Haste (Super)
				{9101002, 1}, -- Holy Symbol
				{9101003, 1}, -- Bless
				{9101004, 1}, -- Hide
				{9101005, 1}, -- Resurrection
				{9101006, 1}, -- Super Dragon Roar, does not work (not implemented)!
				{9101007, 1}, -- Teleport, does not work (not set as teleport skill)!
				{9101008, 1}, -- Hyper Body
				-- Noblesse
				{10000012, 20}, -- Blessing of the Fairy
				{10001000, 3}, -- Three Snails
				{10001001, 3}, -- Recovery
				{10001002, 3}, -- Nimble Feet
				{10001003, 1}, -- Legendary Spirit
				{10001007, 1}, -- Maker
				{10001004, 1}, -- Monster Rider
				{10001005, 1}, -- Echo of Hero
				-- Soul Warrior 1st Job
				{11000000, 10}, -- Improving MaxHP Increase
				{11001001, 10}, -- Iron Body
				{11001002, 20}, -- Power Strike
				{11001003, 20}, -- Slash Blast
				{11001004, 20}, -- Soul
				-- Flame Wizard 1st Job
				{12000000, 10}, -- Improving Max MP Increase
				{12001001, 10}, -- Magic Guard
				{12001002, 10}, -- Magic Armor
				{12001003, 20}, -- Magic Claw
				{12001004, 20}, -- Flame
				-- Wind Breaker 1st Job
				{13000000, 20}, -- Critical Shot
				{13000001, 8}, -- The Eye of Amazon
				{13001002, 10}, -- Focus
				{13001003, 20}, -- Double Shot
				{13001004, 20}, -- Storm
				-- Night Walker 1st Job
				{14000000, 10}, -- Nimble Body
				{14000001, 8}, -- Keen Eyes
				{14001002, 10}, -- Disorder
				{14001003, 10}, -- Dark Sight
				{14001004, 20}, -- Lucky Seven
				{14001005, 20}, -- Darkness
				-- Striker 1st Job
				{15000000, 10}, -- Bullet Time
				{15001001, 20}, -- Flash Fist
				{15001002, 20}, -- Sommersault Kick
				{15001003, 10}, -- Dash
				{15001004, 20}, -- Lightning
				-- Soul Warrior 2nd Job
				{11100000, 20}, -- Sword Mastery
				{11101001, 20}, -- Sword Booster
				{11101002, 30}, -- Final Attack : Sword
				{11101003, 20}, -- Rage
				{11101004, 30}, -- Soul Blade
				{11101005, 10}, -- Soul Rush
				-- Flame Wizard 2nd Job
				{12101000, 20}, -- Meditation
				{12101001, 20}, -- Slow
				{12101002, 20}, -- Flame Arrow
				{12101003, 20}, -- Teleport
				{12101004, 20}, -- Spell Booster
				{12101005, 20}, -- Elemental Reset
				{12101006, 20}, -- Fire Filar
				-- Wind Breaker 2nd Job
				{13100000, 20}, -- Bow Mastery
				{13100004, 20}, -- Thrust
				{13101001, 20}, -- Bow Booster
				{13101002, 30}, -- Final Attack : Bow
				{13101003, 20}, -- Soul Arrow : Bow
				{13101005, 20}, -- Storm Brakes
				{13101006, 10}, -- Wind Walk
				-- Night Walker 2nd Job
				{14100000, 20}, -- Claw Mastery
				{14100001, 30}, -- Critical Throw
				{14100005, 10}, -- Vanish
				{14101002, 20}, -- Claw Booster
				{14101003, 20}, -- Haste
				{14101004, 20}, -- Flash Jump
				{14101006, 20}, -- Vampire
				-- Striker 2nd Job
				{15100000, 10}, -- Improve MaxHP
				{15100001, 20}, -- Knuckle Mastery
				{15100004, 20}, -- Energy Charge
				{15101002, 20}, -- Knuckle Booster
				{15101003, 20}, -- Corkscrew Blow
				{15101005, 20}, -- Energy Blast
				{15101006, 20}, -- Lightning Charge
				-- Soul Warrior 3rd Job
				{11110000, 20}, -- Improving MP Recovery
				{11110005, 20}, -- Advance Combo
				{11111001, 20}, -- Combo Attack
				{11111002, 20}, -- Sword: Panic
				{11111003, 20}, -- Sword: Coma
				{11111004, 30}, -- Brandish
				{11111006, 30}, -- Soul Blow
				{11111007, 20}, -- Soul Charge
				-- Flame Wizard 3rd Job
				{12110000, 20}, -- Elemental Resistance
				{12110001, 20}, -- Element Amplification
				{12111002, 20}, -- Seal
				{12111003, 20}, -- Meteo
				{12111004, 20}, -- Ifrit
				{12111005, 30}, -- Flame Gear
				{12111006, 30}, -- Fire Strike
				-- Wind Breaker 3rd Job
				{13110003, 20}, -- Bow Expert
				{13111000, 20}, -- Arrow Rain
				{13111001, 30}, -- Strafe
				{13111002, 20}, -- Hurricane
				{13111004, 20}, -- Puppet
				{13111005, 10}, -- Albatross
				{13111006, 20}, -- Wind Piercing
				{13111007, 20}, -- Wind Shot
				-- Night Walker 3rd Job
				{14110003, 20}, -- Alchemist
				{14110004, 20}, -- Venom
				{14111000, 30}, -- Shadow Partner
				{14111001, 20}, -- Shadow Web
				{14111002, 30}, -- Avenger
				{14111005, 20}, -- Triple Throw
				{14111006, 30}, -- Poison Bomb
				-- Striker 3rd Job
				{15110000, 20}, -- Critical Punch
				{15111001, 20}, -- Energy Drain
				{15111002, 10}, -- Transformation
				{15111003, 20}, -- Shockwave
				{15111004, 20}, -- Barrage
				{15111005, 20}, -- Speed Infusion
				{15111006, 20}, -- Spark
				{15111007, 30}, -- Shark Wave
			};

			for i = 1, #skills do
				skill = skills[i];
				skillId, skillLevel, skillMasterLevel = skill[1], skill[2], skill[3]
				if skillMasterLevel == nil then
					addSkillLevel(skillId, skillLevel);
				else
					addSkillLevel(skillId, skillLevel, skillMasterLevel);
				end
			end
		end),
	};

	addText("Hi " .. playerRef() .. "! ");
	addText("What do you wish?\r\n");
	addText(blue(choiceRef(choices)));
	choice = askChoice();

	selectChoice(choices, choice);

	addText("There we go! ");
	addText("Have fun!");
	sendOk();
else
	addText("You need to be a GM in order for me to max your skills, " .. playerRef() .. "!");
	sendOk();
end
