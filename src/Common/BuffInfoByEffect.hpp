/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Common/BuffInfo.hpp"
#include "Common/Types.hpp"

namespace Vana {
	struct BuffInfoByEffect {
		BuffInfo physicalAttack;
		BuffInfo physicalDefense;
		BuffInfo magicAttack;
		BuffInfo magicDefense;
		BuffInfo accuracy;
		BuffInfo avoid;
		BuffInfo craft;
		BuffInfo speed;
		BuffInfo jump;
		BuffInfo magicGuard;
		BuffInfo darkSight;
		BuffInfo booster;
		BuffInfo powerGuard;
		BuffInfo hyperBodyHp;
		BuffInfo hyperBodyMp;
		BuffInfo invincible;
		BuffInfo soulArrow;
		BuffInfo stun;
		BuffInfo poison;
		BuffInfo seal;
		BuffInfo darkness;
		BuffInfo combo;
		BuffInfo charge;
		BuffInfo timedHurt;
		BuffInfo holySymbol;
		BuffInfo mesoUp;
		BuffInfo shadowPartner;
		BuffInfo pickpocket;
		BuffInfo mesoGuard;
		BuffInfo weakness;
		BuffInfo curse;
		BuffInfo slow;
		BuffInfo morph;
		BuffInfo timedHeal;
		BuffInfo mapleWarrior;
		BuffInfo powerStance;
		BuffInfo sharpEyes;
		BuffInfo manaReflection;
		BuffInfo seduce;
		BuffInfo shadowStars;
		BuffInfo infinity;
		BuffInfo holyShield;
		BuffInfo hamstring;
		BuffInfo blind;
		BuffInfo concentrate;
		BuffInfo zombify;
		BuffInfo echo;
		BuffInfo crazySkull;
		BuffInfo ignoreWeaponImmunity;
		BuffInfo ignoreMagicImmunity;
		BuffInfo spark;
		BuffInfo dawnWarriorFinalAttack;
		BuffInfo windWalkerFinalAttack;
		BuffInfo elementalReset;
		BuffInfo windWalk;
		BuffInfo energyCharge;
		BuffInfo dashSpeed;
		BuffInfo dashJump;
		BuffInfo mount;
		BuffInfo speedInfusion;
		BuffInfo homingBeacon;
	};
}