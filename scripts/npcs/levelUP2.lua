--[[
Copyright (C) 2008-2012 Vana Development Team

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
-- NimaKIN, credits TerraEnvy of Ragezone/Terra of dev.chisoft & Bri/Zak for adding new skills

if isGm() == 1 then
	addText("Hi #h #! What do you wish?\r\n");
	addText("#b#L1#I would like my inventory slots maxed#l\r\n");
	addText("#L2#I would like my skills maxed#l");
	choice = askChoice();

	if choice == 1 then
		addSlots(1, 25);
		addSlots(2, 25);
		addSlots(3, 25);
		addSlots(4, 25);
		addSlots(5, 25);
		addText("There we go! Have fun!");
		sendOk();
	elseif choice == 2 then
		addSkillLevel(8, 1);--	Follow the Lead
		addSkillLevel(12, 20);--	Blessing of the Fairy
		addSkillLevel(73, 30);--	Empress's Blessing
		addSkillLevel(74, 1);--	Empress's Might
		addSkillLevel(80, 1);--	Empress's Might
		addSkillLevel(86, 1, 1);--	Archangelic Blessing
		addSkillLevel(88, 1, 1);--	Dark Angelic Blessing
		addSkillLevel(91, 1, 1);--	Archangelic Blessing
		addSkillLevel(93, 1);--	Hidden Potential (Explorer)
		addSkillLevel(97, 1);--	Freezing Axe
		addSkillLevel(99, 1);--	Ice Smash
		addSkillLevel(100, 1);--	Ice Tempest
		addSkillLevel(103, 1);--	Ice Chop
		addSkillLevel(104, 1);--	Ice Curse
		addSkillLevel(109, 1);--	Master of Swimming
		addSkillLevel(110, 1);--	Pirate Blessing
		addSkillLevel(111, 1);--	Master of Organization
		addSkillLevel(112, 2);--	Master of Organization
		addSkillLevel(180, 1, 1);--	White Angelic Blessing
		addSkillLevel(190, 1);--	Will of the Alliance
		addSkillLevel(1000, 3);--	Three Snails
		addSkillLevel(1001, 3);--	Recovery
		addSkillLevel(1002, 3);--	Nimble Feet
		addSkillLevel(1003, 1);--	Legendary Spirit
		addSkillLevel(1005, 1);--	Echo of Hero
		addSkillLevel(1006, 1);--	Test
		addSkillLevel(1007, 3);--	Maker
		addSkillLevel(1009, 1);--	Bamboo Rain
		addSkillLevel(1010, 1);--	Invincibility
		addSkillLevel(1011, 1);--	Power Explosion
		addSkillLevel(1013, 2);--	Spaceship
		addSkillLevel(1014, 1);--	Space Dash
		addSkillLevel(1015, 1);--	Space Beam
		addSkillLevel(1020, 1);--	Rage of Pharaoh
		addSkillLevel(1021, 1);
		addSkillLevel(1026, 1);--	Soaring
		addSkillLevel(1032, 1);
		addSkillLevel(1036, 1);--	Lion
		addSkillLevel(1037, 1);--	Unicorn
		addSkillLevel(1038, 1);--	Low Rider
		addSkillLevel(1039, 1);--	Red Truck
		addSkillLevel(1040, 1);--	Gargoyle
		addSkillLevel(1046, 2);--	Spaceship
		addSkillLevel(1047, 1);--	Space Dash
		addSkillLevel(1048, 1);--	Space Beam
		addSkillLevel(1054, 1);--	Chicken
		addSkillLevel(1065, 1);--	OS4 Shuttle
		addSkillLevel(1066, 4);--	Visitor Melee Attack
		addSkillLevel(1067, 4);--	Visitor Range Attack
		addSkillLevel(1069, 1);--	Owl 
		addSkillLevel(1070, 1);--	Mothership
		addSkillLevel(1071, 1);--	OS3A Machine
		addSkillLevel(1075, 5);--	Mihile's Soul Driver
		addSkillLevel(1076, 5);--	Oz's Flame Gear
		addSkillLevel(1077, 5);--	Irena's Wind Piercing
		addSkillLevel(1078, 5);--	Eckhart's Vampire
		addSkillLevel(1079, 5);--	Hawkeye's Shark Wave
		addSkillLevel(1085, 1, 1);--	Archangel
		addSkillLevel(1087, 1, 1);--	Dark Angel
		addSkillLevel(1090, 1, 1);--	Archangel
		addSkillLevel(1092, 1, 1);
		addSkillLevel(1094, 1, 1);
		addSkillLevel(1095, 1, 1);
		addSkillLevel(1096, 1);--	Giant Rabbit
		addSkillLevel(1098, 1);--	Ice Double Jump
		addSkillLevel(1101, 1);--	Small Rabbit
		addSkillLevel(1102, 1);--	Rabbit Rickshaw
		addSkillLevel(1105, 1);--	Ice Knight
		addSkillLevel(1113, 1);--	6th Party Tonight
		addSkillLevel(1114, 1);--	6th Party Tonight
		addSkillLevel(1115, 1);--	Law Officer
		addSkillLevel(1118, 1);--	Nina's Pentacle
		addSkillLevel(1121, 1);--	Frog
		addSkillLevel(1122, 1);--	Turtle
		addSkillLevel(1129, 1);--	Spirit Viking
		addSkillLevel(1130, 1);--	Pachinko Robo
		addSkillLevel(1139, 1);--	Napolean Mount
		addSkillLevel(1142, 10);--	Soaring (Mount)
		addSkillLevel(1143, 10);--	Red Draco
		addSkillLevel(1144, 10);--	Shinjo
		addSkillLevel(1145, 10);--	Hot-Air Balloon
		addSkillLevel(1146, 10);--	Nadeshiko Fly High
		addSkillLevel(1147, 10);--	Pegasus
		addSkillLevel(1148, 10);--	Dragon
		addSkillLevel(1149, 10);--	Magic Broom
		addSkillLevel(1150, 10);--	Cloud
		addSkillLevel(1151, 10);--	Knight's Chariot
		addSkillLevel(1152, 10);--	Nightmare
		addSkillLevel(1153, 10);--	Balrog
		addSkillLevel(1154, 10);--	Invisible Balrog
		addSkillLevel(1155, 10);--	Dragon (Level 3)
		addSkillLevel(1156, 10);--	Owl
		addSkillLevel(1157, 10);--	Helicopter
		addSkillLevel(1158, 1);--	Highway Patrol Car
		addSkillLevel(1179, 1, 1);--	White Angel
		addSkillLevel(1196, 10);--	Mihile's Soul
		addSkillLevel(1197, 10);--	Oz's Flame
		addSkillLevel(1198, 10);--	Irena's Wind
		addSkillLevel(1199, 10);--	Eckhart's Darkness
		addSkillLevel(1200, 10);--	Hawkeye's Lightning
		addSkillLevel(8000, 1);--	Decent Haste
		addSkillLevel(8001, 1);--	Decent Mystic Door
		addSkillLevel(8002, 1);--	Decent Sharp Eyes
		addSkillLevel(8003, 1);--	Decent Hyper Body
		addSkillLevel(8004, 1);--	Decent Combat Orders
		addSkillLevel(8005, 1);--	Decent Advanced Blessing
		addSkillLevel(8006, 1);--	Decent Speed Infusion
		addSkillLevel(9000, 1);--	Pig's Weakness
		addSkillLevel(9001, 1);--	Stump's Weakness
		addSkillLevel(9002, 1);--	Slime's Weakness
		addSkillLevel(1000006, 10);--	HP Boost
		addSkillLevel(1000007, 1);--	Guardian Armor
		addSkillLevel(1001003, 20);--	Iron Body
		addSkillLevel(1001004, 20);--	Power Strike
		addSkillLevel(1001005, 20);--	Slash Blast
		addSkillLevel(1100000, 20);--	Weapon Mastery
		addSkillLevel(1100002, 20);--	Final Attack
		addSkillLevel(1100009, 10);--	Physical Training
		addSkillLevel(1101004, 20);--	Weapon Booster
		addSkillLevel(1101006, 20);--	Rage
		addSkillLevel(1101007, 20);--	Power Reflection
		addSkillLevel(1101008, 10);--	Ground Smash
		addSkillLevel(1101010, 10);--	Slipstream
		addSkillLevel(1110000, 20);--	Self Recovery
		addSkillLevel(1110009, 20);--	Chance Attack
		addSkillLevel(1111002, 20);--	Combo Attack
		addSkillLevel(1111003, 20);--	Panic
		addSkillLevel(1111005, 20);--	Coma
		addSkillLevel(1111007, 20);--	Magic Crash
		addSkillLevel(1111008, 20);--	Shout
		addSkillLevel(1111010, 20);--	Brandish
		addSkillLevel(1120003, 30);--	Advanced Combo Attack
		addSkillLevel(1120012, 10);--	Combat Mastery
		addSkillLevel(1120013, 30, 30);--	Advanced Final Attack
		addSkillLevel(1121000, 30);--	Maple Warrior
		addSkillLevel(1121001, 30, 30);--	Monster Magnet
		addSkillLevel(1121002, 30, 30);--	Power Stance
		addSkillLevel(1121006, 30, 30);--	Rush
		addSkillLevel(1121008, 30, 30);--	Intrepid Slash
		addSkillLevel(1121010, 30, 30);--	Enrage
		addSkillLevel(1121011, 5);--	Hero's Will
		addSkillLevel(1200000, 20);--	Weapon Mastery
		addSkillLevel(1200002, 20);--	Final Attack
		addSkillLevel(1200009, 10);--	Physical Training
		addSkillLevel(1201004, 20);--	Weapon Booster
		addSkillLevel(1201006, 20);--	Threaten
		addSkillLevel(1201007, 20);--	Power Guard
		addSkillLevel(1201008, 10);--	Ground Smash
		addSkillLevel(1201010, 10);--	Slipstream
		addSkillLevel(1210001, 20);--	Shield Mastery
		addSkillLevel(1211002, 30);--	Charged Blow
		addSkillLevel(1211004, 20);--	Flame Charge
		addSkillLevel(1211006, 20);--	Blizzard Charge
		addSkillLevel(1211008, 20);--	Lightning Charge
		addSkillLevel(1211009, 20);--	Magic Crash
		addSkillLevel(1211010, 10);--	HP Recovery
		addSkillLevel(1211011, 20);--	Combat Orders
		addSkillLevel(1220005, 30, 30);--	Achilles
		addSkillLevel(1220006, 30, 30);--	Guardian
		addSkillLevel(1220010, 10);--	Advanced Charge
		addSkillLevel(1220013, 10);--	Divine Shield
		addSkillLevel(1221000, 30);--	Maple Warrior
		addSkillLevel(1221002, 30, 30);--	Power Stance
		addSkillLevel(1221004, 20, 20);--	Holy Charge
		addSkillLevel(1221007, 30, 30);--	Rush
		addSkillLevel(1221009, 30, 30);--	Blast
		addSkillLevel(1221011, 30, 30);--	Heaven's Hammer
		addSkillLevel(1221012, 5);--	Hero's Will
		addSkillLevel(1300000, 20);--	Weapon Mastery
		addSkillLevel(1300002, 20);--	Final Attack
		addSkillLevel(1300009, 10);--	Physical Training
		addSkillLevel(1301004, 20);--	Weapon Booster
		addSkillLevel(1301006, 20);--	Iron Will
		addSkillLevel(1301007, 20);--	Hyper Body
		addSkillLevel(1301008, 10);--	Ground Smash
		addSkillLevel(1301010, 10);--	Slipstream
		addSkillLevel(1310000, 20);--	Elemental Resistance
		addSkillLevel(1310009, 10);--	Dragon Wisdom
		addSkillLevel(1311001, 20);--	Dragon Buster
		addSkillLevel(1311003, 20);--	Dragon Fury
		addSkillLevel(1311005, 20);--	Sacrifice
		addSkillLevel(1311006, 30);--	Dragon Roar
		addSkillLevel(1311007, 20);--	Magic Crash
		addSkillLevel(1311008, 20);--	Dragon Strength
		addSkillLevel(1320006, 30, 30);--	Berserk
		addSkillLevel(1320008, 20, 20);--	Aura of the Beholden
		addSkillLevel(1320009, 20, 20);--	Hex of the Beholden
		addSkillLevel(1320011, 10);--	Revenge of the Beholden
		addSkillLevel(1321000, 30);--	Maple Warrior
		addSkillLevel(1321001, 30, 30);--	Monster Magnet
		addSkillLevel(1321002, 30, 30);--	Power Stance
		addSkillLevel(1321003, 30, 30);--	Rush
		addSkillLevel(1321007, 10, 10);--	Beholden
		addSkillLevel(1321010, 5);--	Hero's Will
		addSkillLevel(1321012, 30, 30);--	Dark Impale
		addSkillLevel(2000006, 10);--	MP Boost
		addSkillLevel(2000007, 1);--	Elemental Weakness
		addSkillLevel(2001002, 15);--	Magic Guard
		addSkillLevel(2001003, 15);--	Magic Armor
		addSkillLevel(2001004, 20);--	Energy Bolt
		addSkillLevel(2001005, 20);--	Magic Claw
		addSkillLevel(2100000, 20);--	MP Eater
		addSkillLevel(2100006, 20);--	Spell Mastery
		addSkillLevel(2100007, 10);--	High Wisdom
		addSkillLevel(2101001, 20);--	Meditation
		addSkillLevel(2101002, 20);--	Teleport
		addSkillLevel(2101003, 10);--	Slow
		addSkillLevel(2101004, 20);--	Blazing Arrow
		addSkillLevel(2101005, 20);--	Poison Breath
		addSkillLevel(2110000, 10);--	Burning Magic
		addSkillLevel(2110001, 20);--	Element Amplification
		addSkillLevel(2110009, 20);--	Arcane Overdrive
		addSkillLevel(2111002, 20);--	Explosion
		addSkillLevel(2111003, 20);--	Poison Mist
		addSkillLevel(2111004, 20);--	Seal
		addSkillLevel(2111005, 20);--	Spell Booster
		addSkillLevel(2111006, 10);--	Fire Demon
		addSkillLevel(2111007, 10);--	Teleport Mastery
		addSkillLevel(2111008, 10);--	Elemental Decrease
		addSkillLevel(2120010, 30, 30);--	Arcane Aim
		addSkillLevel(2121000, 30);--	Maple Warrior
		addSkillLevel(2121001, 30, 30);--	Big Bang
		addSkillLevel(2121003, 30, 30);--	Mist Eruption
		addSkillLevel(2121004, 30, 30);--	Infinity
		addSkillLevel(2121005, 30, 30);--	Ifrit
		addSkillLevel(2121006, 30, 30);--	Paralyze
		addSkillLevel(2121007, 30, 30);--	Meteor Shower
		addSkillLevel(2121008, 5);--	Hero's Will
		addSkillLevel(2121009, 10);--	Buff Mastery
		addSkillLevel(2200000, 20);--	MP Eater
		addSkillLevel(2200006, 20);--	Spell Mastery
		addSkillLevel(2200007, 10);--	High Wisdom
		addSkillLevel(2201001, 20);--	Meditation
		addSkillLevel(2201002, 20);--	Teleport
		addSkillLevel(2201003, 10);--	Slow
		addSkillLevel(2201004, 20);--	Cold Beam
		addSkillLevel(2201005, 20);--	Thunder Bolt
		addSkillLevel(2210000, 10);--	Storm Magic
		addSkillLevel(2210001, 20);--	Element Amplification
		addSkillLevel(2210009, 20);--	Arcane Overdrive
		addSkillLevel(2211002, 20);--	Ice Strike
		addSkillLevel(2211003, 20);--	Thunder Spear
		addSkillLevel(2211004, 20);--	Seal
		addSkillLevel(2211005, 20);--	Spell Booster
		addSkillLevel(2211006, 10);--	Ice Demon
		addSkillLevel(2211007, 10);--	Teleport Mastery
		addSkillLevel(2211008, 10);--	Elemental Decrease
		addSkillLevel(2220010, 30, 30);--	Arcane Aim
		addSkillLevel(2221000, 30);--	Maple Warrior
		addSkillLevel(2221001, 30, 30);--	Big Bang
		addSkillLevel(2221003, 30, 30);--	Glacier Chain
		addSkillLevel(2221004, 30, 30);--	Infinity
		addSkillLevel(2221005, 30, 30);--	Elquines
		addSkillLevel(2221006, 30, 30);--	Chain Lightning
		addSkillLevel(2221007, 30, 30);--	Blizzard
		addSkillLevel(2221008, 5);--	Hero's Will
		addSkillLevel(2221009, 10);--	Buff Mastery
		addSkillLevel(2300000, 20);--	MP Eater
		addSkillLevel(2300006, 20);--	Spell Mastery
		addSkillLevel(2300007, 10);--	High Wisdom
		addSkillLevel(2301001, 20);--	Teleport
		addSkillLevel(2301002, 20);--	Heal
		addSkillLevel(2301003, 15);--	Invincible
		addSkillLevel(2301004, 15);--	Bless
		addSkillLevel(2301005, 20);--	Holy Arrow
		addSkillLevel(2310008, 20);--	Holy Focus
		addSkillLevel(2310010, 20);--	Arcane Overdrive
		addSkillLevel(2311001, 10);--	Dispel
		addSkillLevel(2311002, 10);--	Mystic Door
		addSkillLevel(2311003, 20);--	Holy Symbol
		addSkillLevel(2311004, 20);--	Shining Ray
		addSkillLevel(2311005, 10);--	Doom
		addSkillLevel(2311006, 20);--	Magic Booster
		addSkillLevel(2311007, 10);--	Teleport Mastery
		addSkillLevel(2311009, 20);--	Holy Magic Shell
		addSkillLevel(2320011, 30, 30);--	Arcane Aim
		addSkillLevel(2321000, 30);--	Maple Warrior
		addSkillLevel(2321001, 30, 30);--	Big Bang
		addSkillLevel(2321003, 30, 30);--	Bahamut
		addSkillLevel(2321004, 30, 30);--	Infinity
		addSkillLevel(2321005, 30, 30);--	Advanced Blessing
		addSkillLevel(2321006, 5, 5);--	Resurrection
		addSkillLevel(2321007, 30);--	Angel Ray
		addSkillLevel(2321008, 30, 30);--	Genesis
		addSkillLevel(2321009, 5);--	Hero's Will
		addSkillLevel(2321010, 10);--	Buff Mastery
		addSkillLevel(3000001, 20);--	Critical Shot
		addSkillLevel(3000002, 15);--	Archery Mastery
		addSkillLevel(3000006, 1);--	Nature's Balance
		addSkillLevel(3001004, 15);--	Arrow Blow
		addSkillLevel(3001005, 15);--	Double Shot
		addSkillLevel(3100000, 20);--	Bow Mastery
		addSkillLevel(3100001, 20);--	Final Attack : Bow
		addSkillLevel(3100006, 10);--	Physical Training
		addSkillLevel(3101002, 20);--	Bow Booster
		addSkillLevel(3101003, 10);--	Double Jump
		addSkillLevel(3101004, 10);--	Soul Arrow: Bow
		addSkillLevel(3101005, 20);--	Arrow Bomb: Bow
		addSkillLevel(3101007, 15);--	Silver Hawk
		addSkillLevel(3110001, 15);--	Mortal Blow
		addSkillLevel(3110007, 10);--	Evasion Boost
		addSkillLevel(3111000, 20);--	Concentrate
		addSkillLevel(3111002, 15);--	Puppet
		addSkillLevel(3111003, 20);--	Roasting Shot
		addSkillLevel(3111004, 20);--	Arrow Rain
		addSkillLevel(3111005, 20);--	Phoenix
		addSkillLevel(3111006, 20);--	Strafe
		addSkillLevel(3111008, 20);--	Drain Arrow
		addSkillLevel(3120005, 30, 30);--	Bow Expert
		addSkillLevel(3120006, 30, 30);--	Spirit Link: Phoenix
		addSkillLevel(3120008, 30, 30);--	Advanced Final Attack
		addSkillLevel(3120010, 10);--	Broiler Shot
		addSkillLevel(3120011, 10);--	Marksmanship
		addSkillLevel(3120012, 15);--	Elite Puppet
		addSkillLevel(3121000, 30);--	Maple Warrior
		addSkillLevel(3121002, 30, 30);--	Sharp Eyes
		addSkillLevel(3121004, 30, 30);--	Hurricane
		addSkillLevel(3121007, 30, 30);--	Illusion Step
		addSkillLevel(3121009, 5);--	Hero's Will
		addSkillLevel(3200000, 20);--	Crossbow Mastery
		addSkillLevel(3200001, 20);--	Final Attack : Crossbow
		addSkillLevel(3200006, 10);--	Physical Training
		addSkillLevel(3201002, 20);--	Crossbow Booster
		addSkillLevel(3201003, 10);--	Double Jump
		addSkillLevel(3201004, 10);--	Soul Arrow: Crossbow
		addSkillLevel(3201005, 20);--	Iron Arrow: Crossbow
		addSkillLevel(3201007, 15);--	Golden Eagle
		addSkillLevel(3210001, 15);--	Mortal Blow
		addSkillLevel(3210007, 10);--	Evasion Boost
		addSkillLevel(3211000, 20);--	Concentrate
		addSkillLevel(3211002, 15);--	Puppet
		addSkillLevel(3211003, 20);--	Snapfreeze Shot
		addSkillLevel(3211004, 20);--	Arrow Eruption
		addSkillLevel(3211005, 20);--	Frostprey
		addSkillLevel(3211006, 20);--	Strafe
		addSkillLevel(3211008, 20);--	Dragon's Breath
		addSkillLevel(3220004, 30, 30);--	Marksman Boost
		addSkillLevel(3220005, 30, 30);--	Spirit Link: Frostprey
		addSkillLevel(3220009, 10);--	Marksmanship
		addSkillLevel(3220010, 10);--	Ultimate Strafe
		addSkillLevel(3220012, 15);--	Elite Puppet
		addSkillLevel(3221000, 30);--	Maple Warrior
		addSkillLevel(3221001, 30, 30);--	Piercing Arrow
		addSkillLevel(3221002, 30, 30);--	Sharp Eyes
		addSkillLevel(3221006, 30, 30);--	Illusion Step
		addSkillLevel(3221007, 30, 30);--	Snipe
		addSkillLevel(3221008, 5);--	Hero's Will
		addSkillLevel(4000000, 20);--	Nimble Body
		addSkillLevel(4000001, 5);--	Keen Eyes
		addSkillLevel(4000010, 1);--	Magic Theft
		addSkillLevel(4001002, 10);--	Disorder
		addSkillLevel(4001003, 10);--	Dark Sight
		addSkillLevel(4001334, 20);--	Double Stab
		addSkillLevel(4001344, 20);--	Lucky Seven
		addSkillLevel(4100000, 20);--	Claw Mastery
		addSkillLevel(4100001, 30);--	Critical Throw
		addSkillLevel(4100006, 20);--	Shadow Resistance
		addSkillLevel(4101003, 20);--	Claw Booster
		addSkillLevel(4101004, 20);--	Haste
		addSkillLevel(4101005, 20);--	Drain
		addSkillLevel(4110000, 20);--	Alchemist
		addSkillLevel(4111001, 20);--	Meso Up
		addSkillLevel(4111002, 20);--	Shadow Partner
		addSkillLevel(4111003, 20);--	Shadow Web
		addSkillLevel(4111004, 20);--	Shadow Meso
		addSkillLevel(4111005, 20);--	Avenger
		addSkillLevel(4111006, 20);--	Flash Jump
		addSkillLevel(4111007, 20);--	Dark Flare
		addSkillLevel(4120002, 30, 30);--	Shadow Shifter
		addSkillLevel(4120005, 30, 30);--	Venomous Star
		addSkillLevel(4120010, 10);--	Expert Throwing Star Handling
		addSkillLevel(4121000, 30);--	Maple Warrior
		addSkillLevel(4121003, 30, 30);--	Taunt
		addSkillLevel(4121004, 30, 30);--	Ninja Ambush
		addSkillLevel(4121006, 30, 30);--	Shadow Stars
		addSkillLevel(4121007, 30);--	Triple Throw
		addSkillLevel(4121008, 30, 30);--	Ninja Storm
		addSkillLevel(4121009, 5);--	Hero's Will
		addSkillLevel(4200000, 20);--	Dagger Mastery
		addSkillLevel(4200006, 20);--	Shadow Resistance
		addSkillLevel(4201002, 20);--	Dagger Booster
		addSkillLevel(4201003, 20);--	Haste
		addSkillLevel(4201004, 20);--	Steal
		addSkillLevel(4201005, 30);--	Savage Blow
		addSkillLevel(4210000, 10);--	Shield Mastery
		addSkillLevel(4211001, 10);--	Chakra
		addSkillLevel(4211002, 20);--	Assaulter
		addSkillLevel(4211003, 10);--	Pickpocket
		addSkillLevel(4211004, 20);--	Band of Thieves
		addSkillLevel(4211005, 10);--	Meso Guard
		addSkillLevel(4211006, 20);--	Meso Explosion
		addSkillLevel(4211007, 20);--	Dark Flare
		addSkillLevel(4211008, 20);--	Shadow Partner
		addSkillLevel(4211009, 20);--	Flash Jump
		addSkillLevel(4220002, 30, 30);--	Shadow Shifter
		addSkillLevel(4220005, 30, 30);--	Venomous Stab
		addSkillLevel(4220009, 10);--	Meso Mastery
		addSkillLevel(4221000, 30);--	Maple Warrior
		addSkillLevel(4221001, 30, 30);--	Assassinate
		addSkillLevel(4221003, 30, 30);--	Taunt
		addSkillLevel(4221004, 30, 30);--	Ninja Ambush
		addSkillLevel(4221006, 30, 30);--	Smokescreen
		addSkillLevel(4221007, 30, 30);--	Boomerang Step
		addSkillLevel(4221008, 5);--	Hero's Will
		addSkillLevel(4300000, 20);--	Katara Mastery
		addSkillLevel(4301001, 10);--	Triple Stab
		addSkillLevel(4301002, 20);--	Katara Booster
		addSkillLevel(4310004, 20);--	Shadow Resistance
		addSkillLevel(4311001, 20);--	Self Haste
		addSkillLevel(4311002, 30);--	Fatal Blow
		addSkillLevel(4311003, 20, 20);--	Slash Storm
		addSkillLevel(4321000, 20, 20);--	Tornado Spin
		addSkillLevel(4321001, 20);--	Tornado Spin (Attack)
		addSkillLevel(4321002, 5);--	Flashbang
		addSkillLevel(4321003, 15);--	Flash Jump
		addSkillLevel(4321004, 20);--	Upper Stab
		addSkillLevel(4330001, 10);--	Advanced Dark Sight
		addSkillLevel(4330007, 10);--	Life Drain
		addSkillLevel(4331000, 10);--	Bloody Storm
		addSkillLevel(4331002, 30, 30);--	Mirror Image
		addSkillLevel(4331003, 20);--	Owl Spirit
		addSkillLevel(4331005, 20, 20);--	Flying Assaulter
		addSkillLevel(4331006, 20);--	Chains of Hell
		addSkillLevel(4340001, 30, 30);--	Venom
		addSkillLevel(4340010, 10, 10);--	Sharpness
		addSkillLevel(4341000, 30);--	Maple Warrior
		addSkillLevel(4341002, 30);--	Final Cut
		addSkillLevel(4341003, 30, 30);--	Monster Bomb
		addSkillLevel(4341004, 30, 30);--	Blade Fury
		addSkillLevel(4341006, 30, 30);--	Mirrored Target
		addSkillLevel(4341007, 30, 30);--	Thorns
		addSkillLevel(4341008, 5);--	Hero's Will
		addSkillLevel(4341009, 30);--	Phantom Blow
		addSkillLevel(5000000, 20);--	Bullet Time
		addSkillLevel(5000006, 1);--	Fortune's Favor
		addSkillLevel(5001001, 20);--	Flash Fist
		addSkillLevel(5001002, 20);--	Sommersault Kick
		addSkillLevel(5001003, 20);--	Double Shot
		addSkillLevel(5001005, 10);--	Dash
		addSkillLevel(5010003, 10);--	Cannon Boost
		addSkillLevel(5010004, 1);--	Fortune's Favor
		addSkillLevel(5010005, 1);
		addSkillLevel(5011000, 20);--	Cannon Blaster
		addSkillLevel(5011001, 20);--	Cannon Strike
		addSkillLevel(5011002, 15);--	Blast Back
		addSkillLevel(5100001, 20);--	Knuckle Mastery
		addSkillLevel(5100008, 10);--	Critical Punch
		addSkillLevel(5100009, 10);--	HP Boost
		addSkillLevel(5101002, 20);--	Backspin Blow
		addSkillLevel(5101003, 20);--	Double Uppercut
		addSkillLevel(5101004, 20);--	Corkscrew Blow
		addSkillLevel(5101005, 10);--	MP Recovery
		addSkillLevel(5101006, 20);--	Knuckle Booster
		addSkillLevel(5101007, 10);--	Oak Barrel
		addSkillLevel(5110000, 20);--	Stun Mastery
		addSkillLevel(5110001, 20);--	Energy Charge
		addSkillLevel(5110008, 10);--	Brawling Mastery
		addSkillLevel(5111002, 20);--	Energy Blast
		addSkillLevel(5111004, 20);--	Energy Drain
		addSkillLevel(5111005, 20);--	Transformation
		addSkillLevel(5111006, 30);--	Shockwave
		addSkillLevel(5111007, 20);--	Roll of the Dice
		addSkillLevel(5120011, 10);--	Pirate's Revenge
		addSkillLevel(5121000, 30);--	Maple Warrior
		addSkillLevel(5121001, 30, 30);--	Dragon Strike
		addSkillLevel(5121002, 30, 30);--	Energy Orb
		addSkillLevel(5121003, 20, 20);--	Super Transformation
		addSkillLevel(5121004, 30, 30);--	Demolition
		addSkillLevel(5121005, 30, 30);--	Snatch
		addSkillLevel(5121007, 30, 30);--	Barrage
		addSkillLevel(5121008, 5);--	Pirate's Rage
		addSkillLevel(5121009, 20, 20);--	Speed Infusion
		addSkillLevel(5121010, 30, 30);--	Time Leap
		addSkillLevel(5200000, 20);--	Gun Mastery
		addSkillLevel(5200007, 10);--	Critical Shot
		addSkillLevel(5201001, 20);--	Invisible Shot
		addSkillLevel(5201002, 20);--	Grenade
		addSkillLevel(5201003, 20);--	Gun Booster
		addSkillLevel(5201004, 20);--	Blank Shot
		addSkillLevel(5201005, 10);--	Wings
		addSkillLevel(5201006, 20);--	Recoil Shot
		addSkillLevel(5210000, 20);--	Burst Fire
		addSkillLevel(5211001, 30);--	Octopus
		addSkillLevel(5211002, 30);--	Gaviota
		addSkillLevel(5211004, 20);--	Flamethrower
		addSkillLevel(5211005, 20);--	Ice Splitter
		addSkillLevel(5211006, 20);--	Homing Beacon
		addSkillLevel(5211007, 20);--	Roll of the Dice
		addSkillLevel(5220001, 30, 30);--	Elemental Boost
		addSkillLevel(5220002, 20, 20);--	Wrath of the Octopi
		addSkillLevel(5220011, 20, 20);--	Bullseye
		addSkillLevel(5220012, 10);--	Pirate's Revenge
		addSkillLevel(5221000, 30);--	Maple Warrior
		addSkillLevel(5221003, 30, 30);--	Air Strike
		addSkillLevel(5221004, 30, 30);--	Rapid Fire
		addSkillLevel(5221006, 10, 10);--	Battleship
		addSkillLevel(5221007, 30, 30);--	Battleship Cannon
		addSkillLevel(5221008, 30, 30);--	Battleship Torpedo
		addSkillLevel(5221009, 20, 20);--	Hypnotize
		addSkillLevel(5221010, 5);--	Hero's Will
		addSkillLevel(5300004, 15);--	Critical Fire
		addSkillLevel(5300005, 20);--	Cannon Mastery
		addSkillLevel(5300007, 20);
		addSkillLevel(5300008, 10);--	Pirate Training
		addSkillLevel(5301000, 20);--	Scatter Shot
		addSkillLevel(5301001, 20);--	Barrel Bomb
		addSkillLevel(5301002, 20);--	Cannon Booster
		addSkillLevel(5301003, 20);--	Monkey Magic
		addSkillLevel(5310006, 15);--	Reinforced Cannon
		addSkillLevel(5310007, 20);--	Pirate Rush
		addSkillLevel(5310008, 20);--	Monkey Wave
		addSkillLevel(5310009, 15);--	Counter Crush
		addSkillLevel(5311000, 20);--	Cannon Spike
		addSkillLevel(5311001, 20);--	Monkey Madness
		addSkillLevel(5311002, 20);--	Monkey Wave
		addSkillLevel(5311003, 15);--	Cannon Jump
		addSkillLevel(5311004, 10);--	Barrel Roulette
		addSkillLevel(5311005, 20);--	Luck of the Die
		addSkillLevel(5320007, 5);--	Double Down
		addSkillLevel(5320008, 30, 30);--	Mega Monkey Magic
		addSkillLevel(5320009, 30, 30);--	Cannon Overload
		addSkillLevel(5320011, 15);
		addSkillLevel(5321000, 30, 30);--	Cannon Bazooka
		addSkillLevel(5321001, 30, 30);--	Nautilus Strike
		addSkillLevel(5321003, 15);--	Anchors Aweigh
		addSkillLevel(5321004, 15);--	Monkey Militia
		addSkillLevel(5321005, 30);--	Maple Warrior
		addSkillLevel(5321006, 5);--	Hero's Will
		addSkillLevel(5321010, 30, 30);--	Pirate's Spirit
		addSkillLevel(5321012, 30, 30);--	Cannon Barrage
		addSkillLevel(8001000, 1);--	Macro Test
		addSkillLevel(8001001, 1);--	Teleport
		addSkillLevel(9001000, 1);--	Haste (Normal)
		addSkillLevel(9001001, 1);--	Super Dragon Roar
		addSkillLevel(9001002, 1);--	Teleport
		addSkillLevel(9101000, 1);--	Heal + Dispel
		addSkillLevel(9101001, 1);--	Haste (Super)
		addSkillLevel(9101002, 1);--	Holy Symbol
		addSkillLevel(9101003, 1);--	Bless
		addSkillLevel(9101004, 1);--	Hide
		addSkillLevel(9101005, 1);--	Resurrection
		addSkillLevel(9101006, 1);--	Super Dragon Roar
		addSkillLevel(9101007, 1);--	Teleport
		addSkillLevel(9101008, 1);--	Hyper Body
		addSkillLevel(10000012, 20);--	Blessing of the Fairy
		addSkillLevel(10000018, 1);--	Follow the Lead
		addSkillLevel(10000073, 30);--	Empress's Blessing
		addSkillLevel(10000074, 1);--	Empress's Shout
		addSkillLevel(10000086, 1, 1);--	Archangelic Blessing
		addSkillLevel(10000088, 1, 1);--	Dark Angelic Blessing
		addSkillLevel(10000091, 1, 1);--	Archangelic Blessing
		addSkillLevel(10000093, 1);--	Hidden Potential (Cygnus Knight)
		addSkillLevel(10000097, 1);--	Freezing Axe
		addSkillLevel(10000099, 1);--	Ice Smash
		addSkillLevel(10000100, 1);--	Ice Tempest
		addSkillLevel(10000103, 1);--	Ice Chop
		addSkillLevel(10000104, 1);--	Ice Curse
		addSkillLevel(10000180, 1, 1);--	White Angelic Blessing
		addSkillLevel(10000190, 1);--	Will of the Alliance
		addSkillLevel(10000201, 5);--	Benediction of the Fairy
		addSkillLevel(10000202, 6);--	Noble Mind
		addSkillLevel(10001000, 3);--	Three Snails
		addSkillLevel(10001001, 3);--	Recovery
		addSkillLevel(10001002, 3);--	Nimble Feet
		addSkillLevel(10001003, 1);--	Legendary Spirit
		addSkillLevel(10001005, 1);--	Echo of Hero
		addSkillLevel(10001006, 1);--	Jump Down
		addSkillLevel(10001007, 3);--	Maker
		addSkillLevel(10001009, 1);--	Bamboo Thrust
		addSkillLevel(10001010, 1);--	Invincible Barrier
		addSkillLevel(10001011, 1);--	Meteo Shower
		addSkillLevel(10001013, 1);
		addSkillLevel(10001014, 2);--	Spaceship
		addSkillLevel(10001015, 1);--	Space Dash
		addSkillLevel(10001016, 1);--	Space Beam
		addSkillLevel(10001020, 1);--	Rage of Pharaoh
		addSkillLevel(10001021, 1);
		addSkillLevel(10001026, 1);--	Soaring
		addSkillLevel(10001032, 1);
		addSkillLevel(10001036, 1);--	Lion
		addSkillLevel(10001037, 1);--	Unicorn
		addSkillLevel(10001038, 1);--	Low Rider
		addSkillLevel(10001039, 1);--	Red Truck
		addSkillLevel(10001040, 1);--	Gargoyle
		addSkillLevel(10001046, 2);--	Spaceship
		addSkillLevel(10001047, 1);--	Space Dash
		addSkillLevel(10001048, 1);--	Space Beam
		addSkillLevel(10001054, 1);--	Chicken
		addSkillLevel(10001065, 1);--	OS4 Shuttle
		addSkillLevel(10001066, 4);--	Visitor Melee Attack
		addSkillLevel(10001067, 4);--	Visitor Range Attack
		addSkillLevel(10001069, 1);--	Owl 
		addSkillLevel(10001070, 1);--	Mothership
		addSkillLevel(10001071, 1);--	OS3A Machine
		addSkillLevel(10001075, 1);--	Empress's Prayer
		addSkillLevel(10001085, 1, 1);--	Archangel
		addSkillLevel(10001087, 1, 1);--	Dark Angel
		addSkillLevel(10001090, 1, 1);--	Archangel
		addSkillLevel(10001092, 1, 1);
		addSkillLevel(10001094, 1, 1);
		addSkillLevel(10001095, 1, 1);
		addSkillLevel(10001096, 1);--	Giant Rabbit
		addSkillLevel(10001098, 1);--	Ice Double Jump
		addSkillLevel(10001101, 1);--	Small Rabbit
		addSkillLevel(10001105, 1);--	Ice Knight
		addSkillLevel(10001113, 1);--	6th Party Tonight
		addSkillLevel(10001114, 1);--	6th Party Tonight
		addSkillLevel(10001115, 1);--	Law Officer
		addSkillLevel(10001118, 1);--	Nina's Pentacle
		addSkillLevel(10001121, 1);--	Frog
		addSkillLevel(10001122, 1);--	Turtle
		addSkillLevel(10001129, 1);--	Spirit Viking
		addSkillLevel(10001130, 1);--	Pachinko Robo
		addSkillLevel(10001139, 1);--	Napolean Mount
		addSkillLevel(10001142, 10);--	Soaring (Mount)
		addSkillLevel(10001143, 10);--	Red Draco
		addSkillLevel(10001144, 10);--	Shinjo
		addSkillLevel(10001145, 10);--	Hot-Air Balloon
		addSkillLevel(10001146, 10);--	Nadeshiko Fly High
		addSkillLevel(10001147, 10);--	Pegasus
		addSkillLevel(10001148, 10);--	Dragon
		addSkillLevel(10001149, 10);--	Magic Broom
		addSkillLevel(10001150, 10);--	Cloud
		addSkillLevel(10001151, 10);--	Knight's Chariot
		addSkillLevel(10001152, 10);--	Nightmare
		addSkillLevel(10001153, 10);--	Balrog
		addSkillLevel(10001154, 10);--	Invisible Balrog
		addSkillLevel(10001155, 10);--	Dragon (Level 3)
		addSkillLevel(10001156, 10);--	Owl
		addSkillLevel(10001157, 10);--	Helicopter
		addSkillLevel(10001158, 1);--	Highway Patrol Car
		addSkillLevel(10001179, 1, 1);--	White Angel
		addSkillLevel(10008000, 1);--	Decent Haste
		addSkillLevel(10008001, 1);--	Decent Mystic Door
		addSkillLevel(10008002, 1);--	Decent Sharp Eyes
		addSkillLevel(10008003, 1);--	Decent Hyper Body
		addSkillLevel(10008004, 1);--	Decent Combat Orders
		addSkillLevel(10008005, 1);--	Decent Advanced Blessing
		addSkillLevel(10008006, 1);--	Decent Speed Infusion
		addSkillLevel(10009000, 1);--	Pig's Weakness
		addSkillLevel(10009001, 1);--	Stump's Weakness
		addSkillLevel(10009002, 1);--	Slime's Weakness
		addSkillLevel(11000005, 10);--	HP Boost
		addSkillLevel(11000006, 1);--	Guardian Armor
		addSkillLevel(11001001, 10);--	Iron Body
		addSkillLevel(11001002, 20);--	Power Strike
		addSkillLevel(11001003, 20);--	Slash Blast
		addSkillLevel(11001004, 10);--	Soul
		addSkillLevel(11100000, 20);--	Sword Mastery
		addSkillLevel(11100007, 10);--	Physical Training
		addSkillLevel(11101001, 20);--	Sword Booster
		addSkillLevel(11101002, 20);--	Final Attack
		addSkillLevel(11101003, 20);--	Rage
		addSkillLevel(11101004, 20);--	Soul Blade
		addSkillLevel(11101005, 10);--	Soul Rush
		addSkillLevel(11101006, 20);--	Power Reflection
		addSkillLevel(11110000, 20);--	Self Recovery
		addSkillLevel(11110005, 20);--	Advanced combo
		addSkillLevel(11111001, 20);--	Combo Attack
		addSkillLevel(11111002, 20);--	Panic
		addSkillLevel(11111003, 20);--	Coma
		addSkillLevel(11111004, 20);--	Brandish
		addSkillLevel(11111006, 20);--	Soul Driver
		addSkillLevel(11111007, 10);--	Soul Charge
		addSkillLevel(11111008, 1);--	Magic Crash
		addSkillLevel(12000005, 10);--	MP Boost
		addSkillLevel(12000006, 1);--	Elemental Weakness
		addSkillLevel(12001001, 15);--	Magic Guard
		addSkillLevel(12001002, 15);--	Magic Armor
		addSkillLevel(12001003, 20);--	Magic Claw
		addSkillLevel(12001004, 10);--	Flame
		addSkillLevel(12100007, 20);--	Spell Mastery
		addSkillLevel(12100008, 10);--	High Wisdom
		addSkillLevel(12101000, 20);--	Meditation
		addSkillLevel(12101001, 10);--	Slow
		addSkillLevel(12101002, 20);--	Fire Arrow
		addSkillLevel(12101003, 20);--	Teleport
		addSkillLevel(12101004, 20);--	Spell Booster
		addSkillLevel(12101005, 10);--	Elemental Reset
		addSkillLevel(12101006, 20);--	Fire Pillar
		addSkillLevel(12110000, 20);--	Magic Critical
		addSkillLevel(12110001, 20);--	Element Amplification
		addSkillLevel(12111002, 20);--	Seal
		addSkillLevel(12111003, 20);--	Meteor Shower
		addSkillLevel(12111004, 20);--	Ifrit
		addSkillLevel(12111005, 20);--	Flame Gear
		addSkillLevel(12111006, 30);--	Fire Strike
		addSkillLevel(12111007, 1);--	Teleport Mastery
		addSkillLevel(13000000, 20);--	Critical Shot
		addSkillLevel(13000001, 15);--	Archery Mastery
		addSkillLevel(13000005, 1);--	Nature's Balance
		addSkillLevel(13001003, 20);--	Double Shot
		addSkillLevel(13001004, 10);--	Storm
		addSkillLevel(13100000, 20);--	Bow Mastery
		addSkillLevel(13100008, 10);--	Physical Training
		addSkillLevel(13101001, 20);--	Bow Booster
		addSkillLevel(13101002, 20);--	Final Attack
		addSkillLevel(13101003, 10);--	Soul Arrow
		addSkillLevel(13101004, 10);--	Double Jump
		addSkillLevel(13101005, 20);--	Storm Spike
		addSkillLevel(13101006, 10);--	Wind Walk
		addSkillLevel(13101007, 20);--	Strafe
		addSkillLevel(13110003, 20);--	Bow Expert
		addSkillLevel(13110008, 10);--	Evasion Boost
		addSkillLevel(13110009, 1);--	Mortal Blow
		addSkillLevel(13111000, 20);--	Arrow Rain
		addSkillLevel(13111001, 20);--	Concentrate
		addSkillLevel(13111002, 30);--	Hurricane
		addSkillLevel(13111004, 15);--	Puppet
		addSkillLevel(13111005, 5);--	Eagle Eye
		addSkillLevel(13111006, 20);--	Wind Piercing
		addSkillLevel(13111007, 15);--	Wind Shot
		addSkillLevel(14000000, 20);--	Nimble Body
		addSkillLevel(14000001, 5);--	Keen Eyes
		addSkillLevel(14000006, 1);--	Magic Theft
		addSkillLevel(14001002, 10);--	Disorder
		addSkillLevel(14001003, 10);--	Dark Sight
		addSkillLevel(14001004, 20);--	Lucky Seven
		addSkillLevel(14001005, 10);--	Darkness
		addSkillLevel(14100000, 20);--	Claw Mastery
		addSkillLevel(14100001, 20);--	Critical Throw
		addSkillLevel(14100005, 10);--	Vanish
		addSkillLevel(14100010, 10);--	Physical Training
		addSkillLevel(14101002, 20);--	Claw Booster
		addSkillLevel(14101003, 20);--	Haste
		addSkillLevel(14101004, 10);--	Flash Jump
		addSkillLevel(14101006, 20);--	Vampire
		addSkillLevel(14110003, 20);--	Alchemist
		addSkillLevel(14110004, 20);--	Venom
		addSkillLevel(14111000, 20);--	Shadow Partner
		addSkillLevel(14111001, 20);--	Shadow Web
		addSkillLevel(14111002, 20);--	Avenger
		addSkillLevel(14111005, 20);--	Triple throw
		addSkillLevel(14111006, 30);--	Poison Bomb
		addSkillLevel(14111010, 1);--	Dark Flare
		addSkillLevel(15000000, 20);--	Quick Motion
		addSkillLevel(15000005, 1);--	Fortune's Favor
		addSkillLevel(15001001, 20);--	Straight
		addSkillLevel(15001002, 20);--	Somersault Kick
		addSkillLevel(15001003, 10);--	Dash
		addSkillLevel(15001004, 10);--	Lightning
		addSkillLevel(15100001, 20);--	Knuckle Mastery
		addSkillLevel(15100004, 20);--	Energy Charge
		addSkillLevel(15100007, 10);--	HP Boost
		addSkillLevel(15100009, 10);--	Pirate Training
		addSkillLevel(15101002, 20);--	Knuckle Booster
		addSkillLevel(15101003, 20);--	Corkscrew Blow
		addSkillLevel(15101005, 20);--	Energy Blast
		addSkillLevel(15101006, 20);--	Lightning Charge
		addSkillLevel(15110000, 15);--	Critical Punch
		addSkillLevel(15111001, 20);--	Energy Drain
		addSkillLevel(15111002, 5);--	Transformation
		addSkillLevel(15111003, 20);--	Shockwave
		addSkillLevel(15111004, 20);--	Barrage
		addSkillLevel(15111005, 20);--	Speed Infusion
		addSkillLevel(15111006, 20);--	Spark
		addSkillLevel(15111007, 30);--	Shark Wave
		addSkillLevel(15111011, 1);--	Roll of the Dice
		addSkillLevel(20000012, 20);--	Blessing of the Fairy
		addSkillLevel(20000014, 1);--	Tutorial Skill
		addSkillLevel(20000015, 1);--	Tutorial Skill
		addSkillLevel(20000016, 1);--	Tutorial Skill
		addSkillLevel(20000017, 1);--	Tutorial Skill
		addSkillLevel(20000018, 1);--	Tutorial Skill
		addSkillLevel(20000024, 1);--	Follow the Lead
		addSkillLevel(20000073, 30);--	Empress's Blessing
		addSkillLevel(20000086, 1, 1);--	Archangelic Blessing
		addSkillLevel(20000088, 1, 1);--	Dark Angelic Blessing
		addSkillLevel(20000091, 1, 1);--	Archangelic Blessing
		addSkillLevel(20000093, 1);--	Hidden Potential (Hero)
		addSkillLevel(20000097, 1);--	Freezing Axe
		addSkillLevel(20000099, 1);--	Ice Smash
		addSkillLevel(20000100, 1);--	Ice Tempest
		addSkillLevel(20000103, 1);--	Ice Chop
		addSkillLevel(20000104, 1);--	Ice Curse
		addSkillLevel(20000180, 1, 1);--	White Angelic Blessing
		addSkillLevel(20000190, 1);--	Will of the Alliance
		addSkillLevel(20000194, 1);--	Regained Memory
		addSkillLevel(20001000, 3);--	Three Snails
		addSkillLevel(20001001, 3);--	Recovery
		addSkillLevel(20001002, 3);--	Agile Body
		addSkillLevel(20001003, 1);--	Legendary Spirit
		addSkillLevel(20001005, 1);--	Echo of Hero
		addSkillLevel(20001006, 1);--	Jump Down
		addSkillLevel(20001007, 3);--	Maker
		addSkillLevel(20001009, 1);--	Bamboo Thrust
		addSkillLevel(20001010, 1);--	Invincible Barrier
		addSkillLevel(20001011, 1);--	Meteo Shower
		addSkillLevel(20001013, 1);--	Helper
		addSkillLevel(20001020, 1);--	Rage of Pharaoh
		addSkillLevel(20001021, 1);
		addSkillLevel(20001026, 1);--	Soaring
		addSkillLevel(20001032, 1);
		addSkillLevel(20001036, 1);--	Lion
		addSkillLevel(20001037, 1);--	Unicorn
		addSkillLevel(20001038, 1);--	Low Rider
		addSkillLevel(20001039, 1);--	Red Truck
		addSkillLevel(20001040, 1);--	Gargoyle
		addSkillLevel(20001046, 2);--	Spaceship
		addSkillLevel(20001047, 1);--	Space Dash
		addSkillLevel(20001048, 1);--	Space Beam
		addSkillLevel(20001054, 1);--	Chicken
		addSkillLevel(20001065, 1);--	OS4 Shuttle
		addSkillLevel(20001066, 4);--	Visitor Melee Attack
		addSkillLevel(20001067, 4);--	Visitor Range Attack
		addSkillLevel(20001069, 1);--	Owl 
		addSkillLevel(20001070, 1);--	Mothership
		addSkillLevel(20001071, 1);--	OS3A Machine
		addSkillLevel(20001085, 1, 1);--	Archangel
		addSkillLevel(20001087, 1, 1);--	Dark Angel
		addSkillLevel(20001090, 1, 1);--	Archangel
		addSkillLevel(20001092, 1, 1);
		addSkillLevel(20001094, 1, 1);
		addSkillLevel(20001095, 1, 1);
		addSkillLevel(20001096, 1);--	Giant Rabbit
		addSkillLevel(20001098, 1);--	Ice Double Jump
		addSkillLevel(20001101, 1);--	Small Rabbit
		addSkillLevel(20001105, 1);--	Ice Knight
		addSkillLevel(20001113, 1);--	6th Party Tonight
		addSkillLevel(20001114, 1);--	6th Party Tonight
		addSkillLevel(20001115, 1);--	Law Officer
		addSkillLevel(20001118, 1);--	Nina's Pentacle
		addSkillLevel(20001121, 1);--	Frog
		addSkillLevel(20001122, 1);--	Turtle
		addSkillLevel(20001129, 1);--	Spirit Viking
		addSkillLevel(20001130, 1);--	Pachinko Robo
		addSkillLevel(20001139, 1);--	Napolean Mount
		addSkillLevel(20001142, 10);--	Soaring (Mount)
		addSkillLevel(20001143, 10);--	Red Draco
		addSkillLevel(20001144, 10);--	Shinjo
		addSkillLevel(20001145, 10);--	Hot-Air Balloon
		addSkillLevel(20001146, 10);--	Nadeshiko Fly High
		addSkillLevel(20001147, 10);--	Pegasus
		addSkillLevel(20001148, 10);--	Dragon
		addSkillLevel(20001149, 10);--	Magic Broom
		addSkillLevel(20001150, 10);--	Cloud
		addSkillLevel(20001151, 10);--	Knight's Chariot
		addSkillLevel(20001152, 10);--	Nightmare
		addSkillLevel(20001153, 10);--	Balrog
		addSkillLevel(20001154, 10);--	Invisible Balrog
		addSkillLevel(20001155, 10);--	Dragon (Level 3)
		addSkillLevel(20001156, 10);--	Owl
		addSkillLevel(20001157, 10);--	Helicopter
		addSkillLevel(20001158, 1);--	Highway Patrol Car
		addSkillLevel(20001164, 1);--	Rex's Charge
		addSkillLevel(20001179, 1, 1);--	White Angel
		addSkillLevel(20008000, 1);--	Decent Haste
		addSkillLevel(20008001, 1);--	Decent Mystic Door
		addSkillLevel(20008002, 1);--	Decent Sharp Eyes
		addSkillLevel(20008003, 1);--	Decent Hyper Body
		addSkillLevel(20008004, 1);--	Decent Combat Orders
		addSkillLevel(20008005, 1);--	Decent Advanced Blessing
		addSkillLevel(20008006, 1);--	Decent Speed Infusion
		addSkillLevel(20009000, 1);--	Pig's Weakness
		addSkillLevel(20009001, 1);--	Stump's Weakness
		addSkillLevel(20009002, 1);--	Slime's Weakness
		addSkillLevel(20010012, 20);--	Blessing of the Fairy
		addSkillLevel(20010022, 1);--	Dragon Flight
		addSkillLevel(20010073, 30);--	Empress's Blessing
		addSkillLevel(20010086, 1, 1);--	Archangelic Blessing
		addSkillLevel(20010088, 1, 1);--	Dark Angelic Blessing
		addSkillLevel(20010091, 1, 1);--	Archangelic Blessing
		addSkillLevel(20010093, 1);--	Hidden Potential (Hero)
		addSkillLevel(20010097, 1);--	Freezing Axe
		addSkillLevel(20010099, 1);--	Ice Smash
		addSkillLevel(20010100, 1);--	Ice Tempest
		addSkillLevel(20010103, 1);--	Ice Chop
		addSkillLevel(20010104, 1);--	Ice Curse
		addSkillLevel(20010180, 1, 1);--	White Angelic Blessing
		addSkillLevel(20010190, 1);--	Will of the Alliance
		addSkillLevel(20010194, 1);--	Inherited Will
		addSkillLevel(20011000, 3);--	Three Snails
		addSkillLevel(20011001, 3);--	Recover
		addSkillLevel(20011002, 3);--	Nimble Feet
		addSkillLevel(20011003, 1);--	Legendary Spirit 
		addSkillLevel(20011005, 1);--	Hero's Echo
		addSkillLevel(20011006, 1);--	Jump Down
		addSkillLevel(20011007, 3);--	Maker
		addSkillLevel(20011009, 1);--	Bamboo Thrust
		addSkillLevel(20011010, 1);--	Invincible Barrier
		addSkillLevel(20011011, 1);--	Meteo Shower
		addSkillLevel(20011020, 1);--	Rage of Pharaoh
		addSkillLevel(20011021, 1);
		addSkillLevel(20011024, 1);--	Follow the Lead
		addSkillLevel(20011026, 1);--	Soaring
		addSkillLevel(20011032, 1);
		addSkillLevel(20011036, 1);--	Lion
		addSkillLevel(20011037, 1);--	Unicorn
		addSkillLevel(20011038, 1);--	Low Rider
		addSkillLevel(20011039, 1);--	Red Truck
		addSkillLevel(20011040, 1);--	Gargoyle
		addSkillLevel(20011046, 2);--	Spaceship
		addSkillLevel(20011047, 1);--	Space Dash
		addSkillLevel(20011048, 1);--	Space Beam
		addSkillLevel(20011054, 1);--	Chicken
		addSkillLevel(20011065, 1);--	OS4 Shuttle
		addSkillLevel(20011066, 4);--	Visitor Melee Attack
		addSkillLevel(20011067, 4);--	Visitor Range Attack
		addSkillLevel(20011069, 1);--	Owl 
		addSkillLevel(20011070, 1);--	Mothership
		addSkillLevel(20011071, 1);--	OS3A Machine
		addSkillLevel(20011085, 1, 1);--	Archangel
		addSkillLevel(20011087, 1, 1);--	Dark Angel
		addSkillLevel(20011090, 1, 1);--	Archangel
		addSkillLevel(20011092, 1, 1);
		addSkillLevel(20011094, 1, 1);
		addSkillLevel(20011095, 1, 1);
		addSkillLevel(20011096, 1);--	Giant Rabbit
		addSkillLevel(20011098, 1);--	Ice Double Jump
		addSkillLevel(20011101, 1);--	Small Rabbit
		addSkillLevel(20011105, 1);--	Ice Knight
		addSkillLevel(20011113, 1);--	6th Party Tonight
		addSkillLevel(20011114, 1);--	6th Party Tonight
		addSkillLevel(20011115, 1);--	Law Officer
		addSkillLevel(20011118, 1);--	Nina's Pentacle
		addSkillLevel(20011121, 1);--	Frog
		addSkillLevel(20011122, 1);--	Turtle
		addSkillLevel(20011129, 1);--	Spirit Viking
		addSkillLevel(20011130, 1);--	Pachinko Robo
		addSkillLevel(20011139, 1);--	Napolean Mount
		addSkillLevel(20011142, 10);--	Soaring (Mount)
		addSkillLevel(20011143, 10);--	Red Draco
		addSkillLevel(20011144, 10);--	Shinjo
		addSkillLevel(20011145, 10);--	Hot-Air Balloon
		addSkillLevel(20011146, 10);--	Nadeshiko Fly High
		addSkillLevel(20011147, 10);--	Pegasus
		addSkillLevel(20011148, 10);--	Dragon
		addSkillLevel(20011149, 10);--	Magic Broom
		addSkillLevel(20011150, 10);--	Cloud
		addSkillLevel(20011151, 10);--	Knight's Chariot
		addSkillLevel(20011152, 10);--	Nightmare
		addSkillLevel(20011153, 10);--	Balrog
		addSkillLevel(20011154, 10);--	Invisible Balrog
		addSkillLevel(20011155, 10);--	Dragon (Level 3)
		addSkillLevel(20011156, 10);--	Owl
		addSkillLevel(20011157, 10);--	Helicopter
		addSkillLevel(20011158, 1);--	Highway Patrol Car
		addSkillLevel(20011179, 1, 1);--	White Angel
		addSkillLevel(20018000, 1);--	Decent Haste
		addSkillLevel(20018001, 1);--	Decent Mystic Door
		addSkillLevel(20018002, 1);--	Decent Sharp Eyes
		addSkillLevel(20018003, 1);--	Decent Hyper Body
		addSkillLevel(20018004, 1);--	Decent Combat Orders
		addSkillLevel(20018005, 1);--	Decent Advanced Blessing
		addSkillLevel(20018006, 1);--	Decent Speed Infusion
		addSkillLevel(20019000, 1);--	Pig's Weakness
		addSkillLevel(20019001, 1);--	Stump's Weakness
		addSkillLevel(20019002, 1);--	Slime's Weakness
		addSkillLevel(20020002, 3);--	Potion Mastery
		addSkillLevel(20020012, 20);--	Blessing of the Fairy
		addSkillLevel(20020022, 1);--	Deadly Crits
		addSkillLevel(20020073, 30);--	Empress's Blessing
		addSkillLevel(20020086, 1, 1);--	Archangelic Blessing
		addSkillLevel(20020088, 1, 1);--	Dark Angelic Blessing
		addSkillLevel(20020091, 1, 1);--	Archangelic Blessing
		addSkillLevel(20020093, 1);--	Hidden Potential (Hero)
		addSkillLevel(20020097, 1);--	Freezing Axe
		addSkillLevel(20020099, 1);--	Ice Smash
		addSkillLevel(20020100, 1);--	Ice Tempest
		addSkillLevel(20020103, 1);--	Ice Chop
		addSkillLevel(20020104, 1);--	Ice Curse
		addSkillLevel(20020109, 1);--	Elven Healing
		addSkillLevel(20020111, 1);--	Updraft
		addSkillLevel(20020112, 1);--	Elven Grace
		addSkillLevel(20020180, 1, 1);--	White Angelic Blessing
		addSkillLevel(20020190, 1);--	Will of the Alliance
		addSkillLevel(20021000, 3);--	Crystal Throw
		addSkillLevel(20021001, 3);--	Infiltrate
		addSkillLevel(20021003, 1);--	Legendary Spirit
		addSkillLevel(20021005, 1);--	Hero's Echo
		addSkillLevel(20021006, 1);--	Test
		addSkillLevel(20021007, 3);--	Maker
		addSkillLevel(20021009, 1);--	Bamboo Rain
		addSkillLevel(20021010, 1);--	Invincibility
		addSkillLevel(20021011, 1);--	Power Explosion
		addSkillLevel(20021013, 2);--	Spaceship
		addSkillLevel(20021014, 1);--	Space Dash
		addSkillLevel(20021015, 1);--	Space Beam
		addSkillLevel(20021020, 1);--	Rage of Pharaoh
		addSkillLevel(20021024, 1);--	Follow the Lead
		addSkillLevel(20021026, 1);--	Soaring
		addSkillLevel(20021032, 1);
		addSkillLevel(20021061, 1);--	Capture
		addSkillLevel(20021062, 1);--	Call of the Hunter
		addSkillLevel(20021066, 4);
		addSkillLevel(20021067, 4);
		addSkillLevel(20021068, 1);--	Mechanic Dash
		addSkillLevel(20021085, 1, 1);--	Archangel
		addSkillLevel(20021087, 1, 1);--	Dark Angel
		addSkillLevel(20021090, 1, 1);--	Archangel
		addSkillLevel(20021092, 1, 1);
		addSkillLevel(20021094, 1, 1);
		addSkillLevel(20021095, 1, 1);
		addSkillLevel(20021098, 1);--	Ice Double Jump
		addSkillLevel(20021105, 1);--	Ice Knight
		addSkillLevel(20021110, 1);--	Elven Blessing
		addSkillLevel(20021160, 1);--	Sylvidia
		addSkillLevel(20021161, 1);--	Sylvidia
		addSkillLevel(20021166, 1);
		addSkillLevel(20021179, 1, 1);--	White Angel
		addSkillLevel(20021181, 1);
		addSkillLevel(20028000, 1);--	Decent Haste
		addSkillLevel(20028001, 1);--	Decent Mystic Door
		addSkillLevel(20028002, 1);--	Decent Sharp Eyes
		addSkillLevel(20028003, 1);--	Decent Hyper Body
		addSkillLevel(20028004, 1);--	Decent Combat Orders
		addSkillLevel(20028005, 1);--	Decent Advanced Blessing
		addSkillLevel(20028006, 1);--	Decent Speed Infusion
		addSkillLevel(20030002, 3);
		addSkillLevel(20030012, 20);
		addSkillLevel(20030022, 1);
		addSkillLevel(20030073, 24);
		addSkillLevel(20030086, 1, 1);
		addSkillLevel(20030088, 1, 1);
		addSkillLevel(20030091, 1, 1);
		addSkillLevel(20030093, 1);
		addSkillLevel(20030097, 1);
		addSkillLevel(20030099, 1);
		addSkillLevel(20030100, 1);
		addSkillLevel(20030103, 1);
		addSkillLevel(20030104, 1);
		addSkillLevel(20030180, 1, 1);
		addSkillLevel(20030190, 1);
		addSkillLevel(20030202, 6);
		addSkillLevel(20031000, 3);
		addSkillLevel(20031001, 3);
		addSkillLevel(20031003, 1);
		addSkillLevel(20031005, 1);
		addSkillLevel(20031006, 1);
		addSkillLevel(20031007, 3);
		addSkillLevel(20031009, 1);
		addSkillLevel(20031010, 1);
		addSkillLevel(20031011, 1);
		addSkillLevel(20031013, 2);
		addSkillLevel(20031014, 1);
		addSkillLevel(20031015, 1);
		addSkillLevel(20031020, 1);
		addSkillLevel(20031026, 1);
		addSkillLevel(20031032, 1);
		addSkillLevel(20031061, 1);
		addSkillLevel(20031062, 1);
		addSkillLevel(20031068, 1);
		addSkillLevel(20031085, 1, 1);
		addSkillLevel(20031087, 1, 1);
		addSkillLevel(20031090, 1, 1);
		addSkillLevel(20031092, 1, 1);
		addSkillLevel(20031094, 1, 1);
		addSkillLevel(20031095, 1, 1);
		addSkillLevel(20031098, 1);
		addSkillLevel(20031105, 1);
		addSkillLevel(20031160, 1);
		addSkillLevel(20031161, 1);
		addSkillLevel(20031166, 1);
		addSkillLevel(20031179, 1, 1);
		addSkillLevel(20031181, 1);
		addSkillLevel(20031203, 1);
		addSkillLevel(20031204, 1);
		addSkillLevel(20031205, 1);
		addSkillLevel(20031206, 1);
		addSkillLevel(20031207, 1);
		addSkillLevel(20031208, 1);
		addSkillLevel(20031209, 1);
		addSkillLevel(20031210, 1);
		addSkillLevel(20038000, 1);
		addSkillLevel(20038001, 1);
		addSkillLevel(20038002, 1);
		addSkillLevel(20038003, 1);
		addSkillLevel(20038004, 1);
		addSkillLevel(20038005, 1);
		addSkillLevel(20038006, 1);
		addSkillLevel(21000000, 10);--	Combo Ability
		addSkillLevel(21000002, 20);--	Double Swing
		addSkillLevel(21000004, 10);--	Combo Smash
		addSkillLevel(21000005, 1);--	Guardian Armor
		addSkillLevel(21001001, 5);--	Combat Step
		addSkillLevel(21001003, 20);--	Polearm Booster
		addSkillLevel(21100000, 20);--	Polearm Mastery
		addSkillLevel(21100001, 20);--	Triple Swing
		addSkillLevel(21100002, 10);--	Final Charge
		addSkillLevel(21100005, 10);--	Combo Drain
		addSkillLevel(21100007, 20);--	Combo Fenrir
		addSkillLevel(21100008, 10);--	Physical Training
		addSkillLevel(21100010, 20);--	Final Attack
		addSkillLevel(21101003, 10);--	Body Pressure
		addSkillLevel(21101006, 10);--	Snow Charge
		addSkillLevel(21110000, 20);--	Combo Critical
		addSkillLevel(21110002, 20);--	Full Swing
		addSkillLevel(21110003, 20);--	Final Toss
		addSkillLevel(21110006, 20);--	Rolling Spin
		addSkillLevel(21110007, 20);--	(hidden) Full Swing - Double Swing
		addSkillLevel(21110008, 20);--	(hidden) Full Swing - Triple Swing
		addSkillLevel(21110010, 10);--	Cleaving Blows
		addSkillLevel(21110011, 20);--	Combo Judgment
		addSkillLevel(21111001, 20);--	Might
		addSkillLevel(21111009, 20);--	Combo Recharge
		addSkillLevel(21111012, 10);--	Maha Blessing
		addSkillLevel(21120001, 20, 20);--	High Mastery
		addSkillLevel(21120002, 30, 30);--	Over Swing
		addSkillLevel(21120004, 20);--	High Defense
		addSkillLevel(21120005, 30);--	Final Blow
		addSkillLevel(21120006, 30);--	Combo Tempest
		addSkillLevel(21120007, 30);--	Combo Barrier
		addSkillLevel(21120009, 30);--	(hidden) Over Swing - Double Swing
		addSkillLevel(21120010, 30);--	(hidden) Over Swing - Triple Swing
		addSkillLevel(21120011, 10, 10);--	Sudden Strike
		addSkillLevel(21120012, 30, 30);--	Advanced Final Attack
		addSkillLevel(21121000, 30);--	Maple Warrior
		addSkillLevel(21121003, 20, 20);--	Freeze Standing
		addSkillLevel(21121008, 5);--	Hero's Will
		addSkillLevel(22000000, 20);--	Dragon Soul 
		addSkillLevel(22000002, 1);--	Elemental Weakness
		addSkillLevel(22001001, 20);--	Magic Missile
		addSkillLevel(22101000, 20);--	Fire Circle
		addSkillLevel(22101001, 20);--	Teleport
		addSkillLevel(22111000, 20);--	Lightning Bolt
		addSkillLevel(22111001, 20, 20);--	Magic Guard
		addSkillLevel(22120001, 10);--	High Wisdom
		addSkillLevel(22120002, 10);--	Spell Mastery
		addSkillLevel(22121000, 20);--	Ice Breath
		addSkillLevel(22131000, 20);--	Magic Flare 
		addSkillLevel(22131001, 10);--	Magic Shield
		addSkillLevel(22131002, 10);--	Elemental Decrease
		addSkillLevel(22140000, 15, 15);--	Critical Magic 
		addSkillLevel(22141001, 20);--	Dragon Thrust 
		addSkillLevel(22141002, 20, 20);--	Magic Booster
		addSkillLevel(22141004, 10);--	Dragon Blink
		addSkillLevel(22150000, 15);--	Magic Amplification
		addSkillLevel(22150004, 10);--	Dragon Spark
		addSkillLevel(22151001, 20);--	Fire Breath 
		addSkillLevel(22151002, 10);--	Killer Wings 
		addSkillLevel(22151003, 10);--	Magic Resistance
		addSkillLevel(22160000, 5);--	Dragon Fury
		addSkillLevel(22161001, 20);--	Earthquake
		addSkillLevel(22161002, 10);--	Phantom Imprint 
		addSkillLevel(22161003, 5);--	Recovery Aura
		addSkillLevel(22161004, 15);--	Onyx Shroud
		addSkillLevel(22161005, 10);--	Teleport Mastery
		addSkillLevel(22170001, 30);--	Magic Mastery
		addSkillLevel(22171000, 30, 30);--	Maple Warrior
		addSkillLevel(22171002, 30, 30);--	Illusion
		addSkillLevel(22171003, 30);--	Flame Wheel
		addSkillLevel(22171004, 5);--	Hero's Will
		addSkillLevel(22181000, 25, 25);--	Blessing of the Onyx
		addSkillLevel(22181001, 30, 30);--	Blaze
		addSkillLevel(22181002, 30);--	Dark Fog
		addSkillLevel(22181003, 20);--	Soul Stone
		addSkillLevel(22181004, 20);--	Onyx Will
		addSkillLevel(23000001, 15);--	Potential Power
		addSkillLevel(23000003, 20);--	Sharp Aim
		addSkillLevel(23000004, 1);--	Nature's Balance
		addSkillLevel(23001000, 20);--	Swift Dual Shot
		addSkillLevel(23001002, 10);--	Glide Blast
		addSkillLevel(23100004, 5);--	Parting Shot
		addSkillLevel(23100005, 20);--	Dual Bowguns Mastery
		addSkillLevel(23100006, 20);--	Final Attack: Dual Bowguns
		addSkillLevel(23100008, 10);--	Physical Training
		addSkillLevel(23101000, 20);--	Piercing Storm
		addSkillLevel(23101001, 15);--	Rising Rush
		addSkillLevel(23101002, 20);--	Dual Bowguns Boost
		addSkillLevel(23101003, 15);--	Spirit Surge
		addSkillLevel(23101007, 20);
		addSkillLevel(23110006, 15);--	Aerial Barrage
		addSkillLevel(23111000, 20);--	Stunning Strikes
		addSkillLevel(23111001, 20);--	Leap Tornado
		addSkillLevel(23111002, 20);--	Unicorn Spike
		addSkillLevel(23111003, 20);--	Gust Dive
		addSkillLevel(23111004, 20);--	Ignis Roar
		addSkillLevel(23111005, 20);--	Water Shield
		addSkillLevel(23111008, 20);--	Elemental Knights
		addSkillLevel(23111009, 20);--	Elemental Knights
		addSkillLevel(23111010, 20);--	Elemental Knights
		addSkillLevel(23120009, 30, 30);--	Dual Bowguns Expert
		addSkillLevel(23120010, 20, 20);--	Defense Break
		addSkillLevel(23120011, 20);--	Rolling Moonsault
		addSkillLevel(23120012, 30, 30);--	Advanced Final Attack
		addSkillLevel(23121000, 30, 30);--	Ishtar's Ring
		addSkillLevel(23121002, 20, 20);--	Spikes Royale
		addSkillLevel(23121003, 30, 30);--	Lightning Edge
		addSkillLevel(23121004, 30, 30);--	Ancient Warding
		addSkillLevel(23121005, 30, 30);--	Maple Warrior
		addSkillLevel(23121008, 5);--	Hero's Will
		addSkillLevel(24000003, 20);
		addSkillLevel(24000004, 1);
		addSkillLevel(24001000, 20);
		addSkillLevel(24001001, 20);
		addSkillLevel(24001002, 20);
		addSkillLevel(24100003, 20);
		addSkillLevel(24100004, 20);
		addSkillLevel(24100006, 10);
		addSkillLevel(24101000, 20);
		addSkillLevel(24101001, 20);
		addSkillLevel(24101002, 20);
		addSkillLevel(24101005, 20);
		addSkillLevel(24110004, 20);
		addSkillLevel(24110007, 20);
		addSkillLevel(24111000, 20);
		addSkillLevel(24111001, 20);
		addSkillLevel(24111002, 20);
		addSkillLevel(24111003, 20);
		addSkillLevel(24111005, 20);
		addSkillLevel(24111006, 20);
		addSkillLevel(24111008, 20);
		addSkillLevel(24120002, 20, 20);
		addSkillLevel(24120006, 30, 30);
		addSkillLevel(24121000, 30, 30);
		addSkillLevel(24121001, 30, 30);
		addSkillLevel(24121003, 20, 20);
		addSkillLevel(24121004, 30, 30);
		addSkillLevel(24121005, 30, 30);
		addSkillLevel(24121007, 20, 20);
		addSkillLevel(24121008, 30, 30);
		addSkillLevel(24121009, 5);
		addSkillLevel(24121010, 20, 20);
		addSkillLevel(30000002, 3);--	Potion Mastery
		addSkillLevel(30000012, 20);--	Blessing of the Fairy
		addSkillLevel(30000022, 1);--	Deadly Crits
		addSkillLevel(30000073, 30);--	Empress's Blessing
		addSkillLevel(30000086, 1, 1);--	Archangelic Blessing
		addSkillLevel(30000088, 1, 1);--	Dark Angelic Blessing
		addSkillLevel(30000091, 1, 1);--	Archangelic Blessing
		addSkillLevel(30000093, 1);--	Hidden Potential (Resistance)
		addSkillLevel(30000097, 1);--	Freezing Axe
		addSkillLevel(30000099, 1);--	Ice Smash
		addSkillLevel(30000100, 1);--	Ice Tempest
		addSkillLevel(30000103, 1);--	Ice Chop
		addSkillLevel(30000104, 1);--	Ice Curse
		addSkillLevel(30000180, 1, 1);--	White Angelic Blessing
		addSkillLevel(30000190, 1);--	Will of the Alliance
		addSkillLevel(30001000, 3);--	Crystal Throw
		addSkillLevel(30001001, 3);--	Infiltrate
		addSkillLevel(30001003, 1);--	Legendary Spirit
		addSkillLevel(30001005, 1);--	Hero's Echo
		addSkillLevel(30001006, 1);--	Test
		addSkillLevel(30001007, 3);--	Maker
		addSkillLevel(30001009, 1);--	Bamboo Rain
		addSkillLevel(30001010, 1);--	Invincibility
		addSkillLevel(30001011, 1);--	Power Explosion
		addSkillLevel(30001013, 2);--	Spaceship
		addSkillLevel(30001014, 1);--	Space Dash
		addSkillLevel(30001015, 1);--	Space Beam
		addSkillLevel(30001020, 1);--	Rage of Pharaoh
		addSkillLevel(30001024, 1);--	Follow the Lead
		addSkillLevel(30001026, 1);--	Soaring
		addSkillLevel(30001032, 1);
		addSkillLevel(30001036, 1);--	Lion
		addSkillLevel(30001037, 1);--	Unicorn
		addSkillLevel(30001038, 1);--	Low Rider
		addSkillLevel(30001039, 1);--	Red Truck
		addSkillLevel(30001040, 1);--	Gargoyle
		addSkillLevel(30001054, 10);--	Chicken
		addSkillLevel(30001061, 1);--	Capture
		addSkillLevel(30001062, 1);--	Call of the Hunter
		addSkillLevel(30001065, 1);--	OS4 Shuttle
		addSkillLevel(30001066, 4);--	Visitor Melee Attack
		addSkillLevel(30001067, 4);--	Visitor Range Attack
		addSkillLevel(30001068, 1);--	Mechanic Dash
		addSkillLevel(30001069, 1);--	Owl 
		addSkillLevel(30001070, 1);--	Mothership
		addSkillLevel(30001071, 1);--	OS3A Machine
		addSkillLevel(30001085, 1, 1);--	Archangel
		addSkillLevel(30001087, 1, 1);--	Dark Angel
		addSkillLevel(30001090, 1, 1);--	Archangel
		addSkillLevel(30001092, 1, 1);
		addSkillLevel(30001094, 1, 1);
		addSkillLevel(30001095, 1, 1);
		addSkillLevel(30001096, 1);--	Giant Rabbit
		addSkillLevel(30001098, 1);--	Ice Double Jump
		addSkillLevel(30001101, 1);--	Small Rabbit
		addSkillLevel(30001102, 1);--	Rabbit Rickshaw
		addSkillLevel(30001105, 1);--	Ice Knight
		addSkillLevel(30001113, 1);--	6th Party Tonight
		addSkillLevel(30001114, 1);--	6th Party Tonight
		addSkillLevel(30001115, 1);--	Law Officer
		addSkillLevel(30001118, 1);--	Nina's Pentacle
		addSkillLevel(30001121, 1);--	Frog
		addSkillLevel(30001122, 1);--	Turtle
		addSkillLevel(30001129, 1);--	Spirit Viking
		addSkillLevel(30001130, 1);--	Pachinko Robo
		addSkillLevel(30001139, 1);--	Napolean Mount
		addSkillLevel(30001142, 10);--	Soaring (Mount)
		addSkillLevel(30001143, 10);--	Red Draco
		addSkillLevel(30001144, 10);--	Shinjo
		addSkillLevel(30001145, 10);--	Hot-Air Balloon
		addSkillLevel(30001146, 10);--	Nadeshiko Fly High
		addSkillLevel(30001147, 10);--	Pegasus
		addSkillLevel(30001148, 10);--	Dragon
		addSkillLevel(30001149, 10);--	Magic Broom
		addSkillLevel(30001150, 10);--	Cloud
		addSkillLevel(30001151, 10);--	Knight's Chariot
		addSkillLevel(30001152, 10);--	Nightmare
		addSkillLevel(30001153, 10);--	Balrog
		addSkillLevel(30001154, 10);--	Invisible Balrog
		addSkillLevel(30001155, 10);--	Dragon (Level 3)
		addSkillLevel(30001156, 10);--	Owl
		addSkillLevel(30001157, 10);--	Helicopter
		addSkillLevel(30001158, 1);--	Highway Patrol Car
		addSkillLevel(30001179, 1, 1);--	White Angel
		addSkillLevel(30008000, 1);--	Decent Haste
		addSkillLevel(30008001, 1);--	Decent Mystic Door
		addSkillLevel(30008002, 1);--	Decent Sharp Eyes
		addSkillLevel(30008003, 1);--	Decent Hyper Body
		addSkillLevel(30008004, 1);--	Decent Combat Orders
		addSkillLevel(30008005, 1);--	Decent Advanced Blessing
		addSkillLevel(30008006, 1);--	Decent Speed Infusion
		addSkillLevel(30010002, 3);--	Potion Mastery
		addSkillLevel(30010012, 20);--	Blessing of the Fairy
		addSkillLevel(30010022, 1);--	Deadly Crits
		addSkillLevel(30010073, 30);--	Empress's Blessing
		addSkillLevel(30010086, 1, 1);--	Archangelic Blessing
		addSkillLevel(30010088, 1, 1);--	Dark Angelic Blessing
		addSkillLevel(30010091, 1, 1);--	Archangelic Blessing
		addSkillLevel(30010093, 1);--	Hidden Potential (Resistance)
		addSkillLevel(30010097, 1);--	Freezing Axe
		addSkillLevel(30010099, 1);--	Ice Smash
		addSkillLevel(30010100, 1);--	Ice Tempest
		addSkillLevel(30010103, 1);--	Ice Chop
		addSkillLevel(30010104, 1);--	Ice Curse
		addSkillLevel(30010110, 1);--	Dark Winds
		addSkillLevel(30010111, 1);--	Curse of Fury
		addSkillLevel(30010112, 1);--	Fury Unleashed
		addSkillLevel(30010166, 1);
		addSkillLevel(30010180, 1, 1);--	White Angelic Blessing
		addSkillLevel(30010183, 1);
		addSkillLevel(30010184, 1);
		addSkillLevel(30010185, 1);--	Demonic Blood
		addSkillLevel(30010186, 1);
		addSkillLevel(30010190, 1);--	Will of the Alliance
		addSkillLevel(30011000, 3);--	Crystal Throw
		addSkillLevel(30011001, 3);--	Infiltrate
		addSkillLevel(30011003, 1);--	Legendary Spirit
		addSkillLevel(30011005, 1);--	Hero's Echo
		addSkillLevel(30011006, 1);--	Test
		addSkillLevel(30011007, 3);--	Maker
		addSkillLevel(30011009, 1);--	Bamboo Rain
		addSkillLevel(30011010, 1);--	Invincibility
		addSkillLevel(30011011, 1);--	Power Explosion
		addSkillLevel(30011013, 2);--	Spaceship
		addSkillLevel(30011014, 1);--	Space Dash
		addSkillLevel(30011015, 1);--	Space Beam
		addSkillLevel(30011020, 1);--	Rage of Pharaoh
		addSkillLevel(30011024, 1);--	Follow the Lead
		addSkillLevel(30011026, 1);--	Soaring
		addSkillLevel(30011032, 1);
		addSkillLevel(30011061, 1);--	Capture
		addSkillLevel(30011062, 1);--	Call of the Hunter
		addSkillLevel(30011066, 4);
		addSkillLevel(30011067, 4);
		addSkillLevel(30011068, 1);--	Mechanic Dash
		addSkillLevel(30011085, 1, 1);--	Archangel
		addSkillLevel(30011087, 1, 1);--	Dark Angel
		addSkillLevel(30011090, 1, 1);--	Archangel
		addSkillLevel(30011092, 1, 1);
		addSkillLevel(30011094, 1, 1);
		addSkillLevel(30011095, 1, 1);
		addSkillLevel(30011098, 1);--	Ice Double Jump
		addSkillLevel(30011105, 1);--	Ice Knight
		addSkillLevel(30011109, 1);--	Demon Wings
		addSkillLevel(30011159, 1);--	Demon Wings
		addSkillLevel(30011167, 1);
		addSkillLevel(30011168, 1);
		addSkillLevel(30011169, 1);
		addSkillLevel(30011170, 1);
		addSkillLevel(30011179, 1, 1);--	White Angel
		addSkillLevel(30018000, 1);--	Decent Haste
		addSkillLevel(30018001, 1);--	Decent Mystic Door
		addSkillLevel(30018002, 1);--	Decent Sharp Eyes
		addSkillLevel(30018003, 1);--	Decent Hyper Body
		addSkillLevel(30018004, 1);--	Decent Combat Orders
		addSkillLevel(30018005, 1);--	Decent Advanced Blessing
		addSkillLevel(30018006, 1);--	Decent Speed Infusion
		addSkillLevel(31000002, 10);--	Shadow Swiftness
		addSkillLevel(31000003, 10);--	HP Boost
		addSkillLevel(31000004, 10);--	Demon Lash
		addSkillLevel(31000005, 1);--	Guardian Armor
		addSkillLevel(31001000, 15);--	Grim Scythe
		addSkillLevel(31001001, 20);--	Battle Pact
		addSkillLevel(31001006, 10);
		addSkillLevel(31001007, 10);
		addSkillLevel(31001008, 10);
		addSkillLevel(31100004, 20);--	Weapon Mastery
		addSkillLevel(31100005, 10);--	Physical Training
		addSkillLevel(31100006, 20);--	Outrage
		addSkillLevel(31100007, 1);--	Barbed Lash
		addSkillLevel(31101000, 20);--	Soul Eater
		addSkillLevel(31101001, 20);--	Dark Thrust
		addSkillLevel(31101002, 15);--	Chaos Lock
		addSkillLevel(31101003, 20);--	Vengeance
		addSkillLevel(31110006, 15);--	Insult to Injury
		addSkillLevel(31110007, 20);--	Focused Fury
		addSkillLevel(31110008, 15);--	Possessed Aegis
		addSkillLevel(31110009, 5);--	Max Fury
		addSkillLevel(31110010, 1);--	Demon Lash Arch
		addSkillLevel(31111000, 20);--	Judgment
		addSkillLevel(31111001, 20);--	Vortex of Doom
		addSkillLevel(31111003, 20);--	Raven Storm
		addSkillLevel(31111004, 20);--	Black-Hearted Strength
		addSkillLevel(31111005, 20);--	Carrion Breath
		addSkillLevel(31120008, 30, 30);--	Barricade Mastery
		addSkillLevel(31120009, 30, 30);--	Obsidian Skin
		addSkillLevel(31120011, 1, 1);--	Demon Thrash
		addSkillLevel(31121000, 30, 30);--	Infernal Concussion
		addSkillLevel(31121001, 30, 30);--	Demon Impact
		addSkillLevel(31121002, 10, 10);--	Leech Aura
		addSkillLevel(31121003, 20, 20);--	Demon Cry
		addSkillLevel(31121004, 30, 30);--	Maple Warrior
		addSkillLevel(31121005, 30, 30);--	Dark Metamorphosis
		addSkillLevel(31121006, 20, 20);--	Binding Darkness
		addSkillLevel(31121007, 15, 15);--	Boundless Rage
		addSkillLevel(31121010, 30);
		addSkillLevel(32000012, 1);--	Elemental Weakness
		addSkillLevel(32001000, 20);--	Triple Blow
		addSkillLevel(32001001, 10);--	The Finisher
		addSkillLevel(32001002, 15);--	Teleport
		addSkillLevel(32001003, 20);--	Dark Aura
		addSkillLevel(32001008, 10);--	The Finisher
		addSkillLevel(32001009, 10);--	The Finisher
		addSkillLevel(32001010, 10);--	The Finisher
		addSkillLevel(32001011, 10);--	The Finisher
		addSkillLevel(32100006, 20);--	Staff Mastery
		addSkillLevel(32100007, 10);--	High Wisdom
		addSkillLevel(32101000, 20);--	Quad Blow
		addSkillLevel(32101001, 20);--	Hyper Dark Chain
		addSkillLevel(32101003, 20);--	Yellow Aura
		addSkillLevel(32101004, 20);--	Blood Drain
		addSkillLevel(32101005, 20);--	Staff Boost
		addSkillLevel(32110000, 10);--	Advanced Blue Aura
		addSkillLevel(32110001, 20);--	Battle Mastery
		addSkillLevel(32110007, 20);
		addSkillLevel(32110008, 20);
		addSkillLevel(32110009, 20);
		addSkillLevel(32111002, 20);--	Quintuple Blow
		addSkillLevel(32111003, 20);--	Dark Shock
		addSkillLevel(32111004, 10);--	Conversion
		addSkillLevel(32111005, 20);--	Body Boost
		addSkillLevel(32111006, 20);--	Summon Reaper Buff
		addSkillLevel(32111010, 10);--	Teleport Mastery
		addSkillLevel(32111012, 20);--	Blue Aura
		addSkillLevel(32111014, 10, 10);--	Stance
		addSkillLevel(32120000, 30, 30);--	Advanced Dark Aura
		addSkillLevel(32120001, 30, 30);--	Advanced Yellow Aura
		addSkillLevel(32120009, 30, 30);--	Energize
		addSkillLevel(32121002, 30, 30);--	Finishing Blow
		addSkillLevel(32121003, 30, 30);--	Twister Spin
		addSkillLevel(32121004, 30, 30);--	Dark Genesis
		addSkillLevel(32121006, 30, 30);--	Party Shield
		addSkillLevel(32121007, 30, 30);--	Maple Warrior
		addSkillLevel(32121008, 5);--	Hero's Will
		addSkillLevel(33000004, 1);--	Nature's Balance
		addSkillLevel(33001000, 20);--	Triple Shot
		addSkillLevel(33001001, 15);--	Jaguar Rider
		addSkillLevel(33001002, 10);--	Jag Jump
		addSkillLevel(33001003, 20);--	Crossbow Booster
		addSkillLevel(33100000, 20);--	Crossbow Mastery
		addSkillLevel(33100009, 20);--	Final Attack
		addSkillLevel(33100010, 10);--	Physical Training
		addSkillLevel(33101001, 20);--	Ricochet
		addSkillLevel(33101002, 20);--	Jaguar Rawr
		addSkillLevel(33101003, 10);--	Soul Arrow: Crossbow
		addSkillLevel(33101004, 20);--	It's Raining Mines
		addSkillLevel(33101005, 10);--	Jaguar-oshi
		addSkillLevel(33101006, 20);--	Jaguar-oshi
		addSkillLevel(33101007, 20);--	Jaguar-oshi
		addSkillLevel(33101008, 20);--	It's Raining Mines(hidden self-destruct)
		addSkillLevel(33110000, 20);--	Jaguar Boost
		addSkillLevel(33111001, 30);--	Enduring Fire
		addSkillLevel(33111002, 30);--	Dash 'n Slash
		addSkillLevel(33111003, 20);--	Wild Trap
		addSkillLevel(33111004, 10);--	Blind
		addSkillLevel(33111005, 20);--	Silver Hawk
		addSkillLevel(33111006, 20);--	Swipe
		addSkillLevel(33111007, 30, 30);--	Feline Berserk
		addSkillLevel(33120000, 30, 30);--	Crossbow Expert
		addSkillLevel(33120010, 10, 10);--	Wild Instinct
		addSkillLevel(33120011, 30, 30);--	Advanced Final Attack
		addSkillLevel(33121001, 30, 30);--	Exploding Arrows
		addSkillLevel(33121002, 30, 30);--	Sonic Roar
		addSkillLevel(33121004, 30, 30);--	Sharp Eyes
		addSkillLevel(33121005, 10, 10);--	Stink Bomb Shot
		addSkillLevel(33121007, 30, 30);--	Maple Warrior
		addSkillLevel(33121008, 5);--	Hero's Will
		addSkillLevel(33121009, 30, 30);--	Wild Arrow Blast
		addSkillLevel(35000005, 1);--	Fortune's Favor
		addSkillLevel(35001001, 20);--	Flame Launcher
		addSkillLevel(35001002, 10);--	Mech: Prototype
		addSkillLevel(35001003, 20);--	ME-07 Drillhands
		addSkillLevel(35001004, 15);--	Gatling Gun
		addSkillLevel(35100000, 20);--	Mechanic Mastery
		addSkillLevel(35100008, 20);--	Heavy Weapon Mastery
		addSkillLevel(35100011, 10);--	Physical Training
		addSkillLevel(35101003, 20);--	Atomic Hammer
		addSkillLevel(35101004, 10);--	Rocket Booster
		addSkillLevel(35101005, 10);--	Open Portal: GX-9
		addSkillLevel(35101006, 20);--	Mechanic Rage
		addSkillLevel(35101007, 20);--	Perfect Armor
		addSkillLevel(35101009, 20);--	Enhanced Flame Launcher
		addSkillLevel(35101010, 20);--	Enhanced Gatling Gun
		addSkillLevel(35110014, 20);--	Metal Fist Mastery
		addSkillLevel(35111001, 20);--	Satellite
		addSkillLevel(35111002, 20);--	Rock 'n Shock
		addSkillLevel(35111004, 20);--	Mech: Siege Mode
		addSkillLevel(35111005, 15);--	Acceleration Bot EX-7
		addSkillLevel(35111009, 20);--	Satellite
		addSkillLevel(35111010, 20);--	Satellite
		addSkillLevel(35111011, 20);--	Healing Robot H-LX
		addSkillLevel(35111013, 20);--	Roll of the Dice
		addSkillLevel(35111015, 20);--	Punch Launcher
		addSkillLevel(35120000, 30, 30);--	Extreme Mech
		addSkillLevel(35120001, 15, 15);--	Robot Mastery
		addSkillLevel(35121003, 30, 30);--	Giant Robot SG-88 
		addSkillLevel(35121005, 30, 30);--	Mech: Missile Tank
		addSkillLevel(35121006, 25, 25);--	Satellite Safety
		addSkillLevel(35121007, 30, 30);--	Maple Warrior
		addSkillLevel(35121008, 5);--	Hero's Will
		addSkillLevel(35121009, 30, 30);--	Bots 'n Tots
		addSkillLevel(35121010, 20, 20);--	Amplifier Robot AF-11
		addSkillLevel(35121011, 30);
		addSkillLevel(35121012, 30, 30);--	Laser Blast
		addSkillLevel(35121013, 20);--	Mech: Siege Mode
		addSkillLevel(80000000, 1);--	Pirate Blessing
		addSkillLevel(80000001, 1);--	Fury Unleashed
		addSkillLevel(80001000, 1);--	Monster Riding
		addSkillLevel(80001001, 1);--	Yeti Mount
		addSkillLevel(80001002, 1);--	Witch's Broomstick
		addSkillLevel(80001003, 1);--	Charge! Toy Trojan
		addSkillLevel(80001004, 1);--	Croco
		addSkillLevel(80001005, 1);--	Black Scooter
		addSkillLevel(80001006, 1);--	Pink Scooter
		addSkillLevel(80001007, 1);--	Nimbus Cloud
		addSkillLevel(80001008, 1);--	Balrog
		addSkillLevel(80001009, 1);--	Race Kart
		addSkillLevel(80001010, 1);--	ZD Tiger
		addSkillLevel(80001011, 1);--	Mist Balrog
		addSkillLevel(80001012, 1);--	Shinjo
		addSkillLevel(80001013, 1);--	Orange Mushroom
		addSkillLevel(80001014, 1);--	Nightmare
		addSkillLevel(80001015, 1);--	Ostrich
		addSkillLevel(80001016, 1);--	Pink Bear Hot-Air Balloon
		addSkillLevel(80001017, 1);--	Transformed Robot
		addSkillLevel(80001018, 1);--	Motorcycle
		addSkillLevel(80001019, 1);--	Power Suit
		addSkillLevel(80001020, 1);--	Leonardo the Lion
		addSkillLevel(80001021, 1);--	Blue Scooter
		addSkillLevel(80001022, 1);--	Santa Sled
		addSkillLevel(80001023, 1);--	Fortune
		addSkillLevel(80001024, 1);--	Yeti Mount
		addSkillLevel(80001025, 1);--	Yeti
		addSkillLevel(80001026, 1);--	Witch's Broomstick
		addSkillLevel(80001027, 1);--	Wooden Airplane
		addSkillLevel(80001028, 1);--	Red Airplane
		addSkillLevel(80001029, 1);--	Cygnus Knights Chariot
		addSkillLevel(80001030, 1);--	Chicken
		addSkillLevel(80001031, 1);--	Owl
		addSkillLevel(80001032, 1);--	Low Rider
		addSkillLevel(80001033, 1);--	Spiegelmann's Hot Air Balloon
		addSkillLevel(80001034, 1);--	Virtue's Blessing
		addSkillLevel(80001035, 1);--	Virtue's Blessing
		addSkillLevel(80001036, 1);--	Virtue's Blessing
		addSkillLevel(80001037, 1);--	Bjorn
		addSkillLevel(80001038, 1);--	Speedy Cygnus Knights Chariot
		addSkillLevel(80001039, 1);--	Unicorn Mount
		addSkillLevel(80001040, 1);--	Elven Blessing
		addSkillLevel(80001041, 1);--	Great Fortitude
		addSkillLevel(80001042, 1);--	Great Fortitude
		addSkillLevel(80001043, 1);--	Great Fortitude
		addSkillLevel(80001044, 1);--	Bunny Mount
		addSkillLevel(80001045, 1);--	Lion
		addSkillLevel(80001046, 1);--	Unicorn
		addSkillLevel(80001047, 1);--	Low Rider
		addSkillLevel(80001048, 1);--	Red Truck
		addSkillLevel(80001049, 1);--	Gargoyle
		addSkillLevel(80001050, 1);--	Chicken
		addSkillLevel(80001051, 1);--	Owl 
		addSkillLevel(80001052, 1);--	Mothership
		addSkillLevel(80001053, 1);--	OS3A Machine
		addSkillLevel(80001054, 1);--	Giant Rabbit
		addSkillLevel(80001055, 1);--	Small Rabbit
		addSkillLevel(80001056, 1);--	Rabbit Rickshaw
		addSkillLevel(80001057, 1);--	Law Officer
		addSkillLevel(80001058, 1);--	Nina's Pentacle
		addSkillLevel(80001059, 1);--	Frog
		addSkillLevel(80001060, 1);--	Turtle
		addSkillLevel(80001061, 1);--	Spirit Viking
		addSkillLevel(80001062, 1);--	Napolean Mount
		addSkillLevel(80001063, 10);--	Red Draco
		addSkillLevel(80001064, 10);--	Shinjo
		addSkillLevel(80001065, 10);--	Hot-Air Balloon
		addSkillLevel(80001066, 10);--	Nadeshiko Fly High
		addSkillLevel(80001067, 10);--	Pegasus
		addSkillLevel(80001068, 10);--	Dragon
		addSkillLevel(80001069, 10);--	Magic Broom
		addSkillLevel(80001070, 10);--	Cloud
		addSkillLevel(80001071, 10);--	Knight's Chariot
		addSkillLevel(80001072, 10);--	Nightmare
		addSkillLevel(80001073, 10);--	Balrog
		addSkillLevel(80001074, 10);--	Invisible Balrog
		addSkillLevel(80001075, 10);--	Dragon (Level 3)
		addSkillLevel(80001076, 10);--	Owl
		addSkillLevel(80001077, 10);--	Helicopter
		addSkillLevel(80001078, 1);--	Highway Patrol Car
		addSkillLevel(80001079, 1);--	Monster Carnival - ATT UP
		addSkillLevel(80001080, 1);--	Monster Carnival - DEF UP
		addSkillLevel(80001081, 1);--	Monster Carnival - EXP UP
		addSkillLevel(80001082, 1);--	Buffalo
		addSkillLevel(80001083, 1);--	Rabbit Rickshaw
		addSkillLevel(80001084, 1);--	Giant Rabbit
		addSkillLevel(80001085, 2);--	Spaceship
		addSkillLevel(80001086, 1);--	Space Dash
		addSkillLevel(80001087, 1);--	Space Beam
		addSkillLevel(80001088, 1);--	OS4 Shuttle
		addSkillLevel(80001089, 10);--	Soaring
		addSkillLevel(80001090, 1);--	Miwok Hog Mount
		addSkillLevel(80001091, 1);--	Sebek's Eye
		addSkillLevel(80001092, 1);--	Nefer's Eye
		addSkillLevel(80001093, 1);--	Ptah's Eye
		addSkillLevel(80001094, 1);--	Nut's Eye
		addSkillLevel(80001095, 1);--	Geb's Eye
		addSkillLevel(80001096, 1);--	Seth's Eye
		addSkillLevel(80001097, 1);--	Maat's Eye
		addSkillLevel(80001098, 1);--	Imhotep's Eye
		addSkillLevel(80001099, 1);--	Apis's Eye
		addSkillLevel(80001100, 1);--	Horus's Eye
		addSkillLevel(80001101, 1);--	Isis's Eye
		addSkillLevel(80001102, 1);--	Amun's Eye
		addSkillLevel(80001103, 1);--	Ra's Eye
		addSkillLevel(80001104, 1);--	Anubis's Eye
		addSkillLevel(80001105, 1);--	Horus's Eye
		addSkillLevel(80001106, 1);--	Isis's Eye
		addSkillLevel(80001107, 1);--	Osiris's Eye
		addSkillLevel(80001108, 1);--	Ra's Eye
		addSkillLevel(80001112, 1);--	Panda Mount
		addSkillLevel(80001114, 1);--	Bunny Buddy Buggy
		addSkillLevel(80001121, 1);--	Crimson Nightmare
		addSkillLevel(80001122, 1);--	??? ??
		addSkillLevel(80001124, 1);--	M.S.S. Awesome
		addSkillLevel(80001127, 1);--	Cretaceous Mount
		addSkillLevel(80001131, 1);--	Infernal Mutt
		addSkillLevel(80001152, 1);--	Nebulite Fusion
		addSkillLevel(80001153, 1);--	Unlucky Buddy Mount
		addSkillLevel(80001195, 1);--	Maximus Mount
		addSkillLevel(90000000, 1);--	Instant Death
		addSkillLevel(90001001, 2);--	Knock Down
		addSkillLevel(90001002, 1);--	Slow
		addSkillLevel(90001003, 1);--	Poison
		addSkillLevel(90001004, 2);--	Darkness 
		addSkillLevel(90001005, 1);--	Seal 
		addSkillLevel(90001006, 2);--	Freeze
		addSkillLevel(91000000, 5);--	Banner of Nimbleness
		addSkillLevel(91000001, 5);--	Banner of Experience
		addSkillLevel(91000002, 5);--	Banner of Fortitude
		addSkillLevel(91000003, 5);--	Banner of Ferocity
		addSkillLevel(91000004, 4);--	Banner of Mobility
		addSkillLevel(91000005, 4);--	Banner of Lethality
		addSkillLevel(91000006, 10);--	Banner of Plenty
		addSkillLevel(92000000, 10);--	Herbalism
		addSkillLevel(92010000, 10);--	Mining
		addSkillLevel(92020000, 10);--	Smithing
		addSkillLevel(92030000, 10);--	Accessory Crafting
		addSkillLevel(92040000, 10);--	Alchemy

		addText("There we go! Have fun!");
		sendOk();
	else
		addText("Okay, come back to me any time if you change your mind.")
		sendOk();
	end
else
	addText("You need to be a GM in order for me to max your skills, #h #!");
	sendOk();
end
