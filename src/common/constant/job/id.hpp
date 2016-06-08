/*
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
*/
#pragma once

#include "common/types.hpp"

namespace vana {
	namespace constant {
		namespace job {
			namespace id {
				enum jobs : game_job_id {
					beginner = 0,
					cannoneer_beginner = 1,

					swordsman = 100,
					fighter = 110,
					crusader = 111,
					hero = 112,
					page = 120,
					white_knight = 121,
					paladin = 122,
					spearman = 130,
					dragon_knight = 131,
					dark_knight = 132,

					magician = 200,
					fp_wizard = 210,
					fp_mage = 211,
					fp_arch_mage = 212,
					il_wizard = 220,
					il_mage = 221,
					il_arch_mage = 222,
					cleric = 230,
					priest = 231,
					bishop = 232,

					archer = 300,
					hunter = 310,
					ranger = 311,
					bowmaster = 312,
					crossbowman = 320,
					sniper = 321,
					marksman = 322,

					rogue = 400,
					assassin = 410,
					hermit = 411,
					night_lord = 412,
					bandit = 420,
					chief_bandit = 421,
					shadower = 422,
					blade_recruit = 430,
					blade_acolyte = 431,
					blade_specialist = 432,
					blade_lord = 433,
					blade_master = 434,

					pirate = 500,
					cannoneer_pirate = 501,
					brawler = 510,
					marauder = 511,
					buccaneer = 512,
					gunslinger = 520,
					outlaw = 521,
					corsair = 522,
					cannoneer = 530,
					cannon_trooper = 531,
					cannon_master = 532,

					junior_gm = 800, // ??
					gm = 900,
					super_gm = 910,

					noblesse = 1000,

					dawn_warrior1 = 1100,
					dawn_warrior2 = 1110,
					dawn_warrior3 = 1111,
					dawn_warrior4 = 1112,

					blaze_wizard1 = 1200,
					blaze_wizard2 = 1210,
					blaze_wizard3 = 1211,
					blaze_wizard4 = 1212,

					wind_archer1 = 1300,
					wind_archer2 = 1310,
					wind_archer3 = 1311,
					wind_archer4 = 1312,

					night_walker1 = 1400,
					night_walker2 = 1410,
					night_walker3 = 1411,
					night_walker4 = 1412,

					thunder_breaker1 = 1500,
					thunder_breaker2 = 1510,
					thunder_breaker3 = 1511,
					thunder_breaker4 = 1512,

					legend = 2000,
					evan = 2001,
					mercedes = 2002,
					phantom = 2003,
					luminous = 2004,

					aran1 = 2100,
					aran2 = 2110,
					aran3 = 2111,
					aran4 = 2112,

					evan1 = 2200,
					evan2 = 2210,
					evan3 = 2211,
					evan4 = 2212,
					evan5 = 2213,
					evan6 = 2214,
					evan7 = 2215,
					evan8 = 2216,
					evan9 = 2217,
					evan10 = 2218,

					mercedes1 = 2300,
					mercedes2 = 2310,
					mercedes3 = 2311,
					mercedes4 = 2312,

					phantom1 = 2400,
					phantom2 = 2410,
					phantom3 = 2411,
					phantom4 = 2412,

					luminous1 = 2700,
					luminous2 = 2710,
					luminous3 = 2711,
					luminous4 = 2712,

					citizen = 3000,
					demon_slayer = 3001,
					xenon = 3002,

					demon_slayer1 = 3100,
					demon_slayer2 = 3110,
					demon_slayer3 = 3111,
					demon_slayer4 = 3112,

					battle_mage1 = 3200,
					battle_mage2 = 3210,
					battle_mage3 = 3211,
					battle_mage4 = 3212,

					wild_hunter1 = 3300,
					wild_hunter2 = 3310,
					wild_hunter3 = 3311,
					wild_hunter4 = 3312,

					mechanic1 = 3500,
					mechanic2 = 3510,
					mechanic3 = 3511,
					mechanic4 = 3512,

					xenon1 = 3600,
					xenon2 = 3610,
					xenon3 = 3611,
					xenon4 = 3612,

					hayato = 4001,
					kanna = 4002,

					mihile = 5000,
					mihile1 = 5100,
					mihile2 = 5110,
					mihile3 = 5111,
					mihile4 = 5112,

					kaiser = 6000,
					angelic_buster = 6001,

					kaiser1 = 6100,
					kaiser2 = 6110,
					kaiser3 = 6111,
					kaiser4 = 6112,
					angelic_buster1 = 6500,
					angelic_buster2 = 6510,
					angelic_buster3 = 6511,
					angelic_buster4 = 6512,
				};
			}
		}
	}
}