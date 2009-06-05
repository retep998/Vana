/*
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
*/
#include "MobsPacket.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "MapleSession.h"
#include "Maps.h"
#include "Mobs.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PacketReader.h"
#include "SendHeader.h"

void MobsPacket::spawnMob(Player *player, Mob *mob, int8_t summoneffect, Mob *owner, bool spawn, bool show) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOW_MOB);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(1);
	packet.add<int32_t>(mob->getMobId());
	mob->statusPacket(packet); // Mob's status such as frozen, stunned, and etc
	packet.addPos(mob->getPos());
	packet.add<int8_t>(owner != 0 ? 0x08 : 0x02); // Not stance, exploring further
	packet.add<int16_t>(mob->getFh());
	packet.add<int16_t>(mob->getOriginFh());
	if (owner != 0) {
		packet.add<int8_t>(summoneffect != 0 ? summoneffect : -3);
	}
	else {
		packet.add<int8_t>(spawn ? -2 : -1);
	}
	if (owner != 0)
		packet.add<int32_t>(owner->getId());
	packet.add<int8_t>(-1);
	packet.add<int32_t>(0);
	if (show)
		player->getSession()->send(packet);
	else
		Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::requestControl(Player *player, Mob *mob, bool spawn) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CONTROL_MOB);
	packet.add<int8_t>(1);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(1);
	packet.add<int32_t>(mob->getMobId());
	mob->statusPacket(packet); // Mob's status such as frozen, stunned, and etc
	packet.add<int32_t>(0);
	packet.addPos(mob->getPos());
	packet.add<int8_t>(2); // Not stance, exploring further
	packet.add<int16_t>(mob->getFh());
	packet.add<int16_t>(mob->getOriginFh());
	packet.add<int16_t>(-1); // ??
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void MobsPacket::endControlMob(Player *player, Mob *mob) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_CONTROL_MOB);
	packet.add<int8_t>(0);
	packet.add<int32_t>(mob->getId());
	player->getSession()->send(packet);
}

void MobsPacket::moveMobResponse(Player *player, int32_t mobid, int16_t moveid, bool useskill, int32_t mp, uint8_t skill, uint8_t level) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_MOB_RESPONSE);
	packet.add<int32_t>(mobid);
	packet.add<int16_t>(moveid);
	packet.add<int8_t>(useskill);
	packet.add<int16_t>(static_cast<int16_t>(mp));
	packet.add<uint8_t>(skill);
	packet.add<uint8_t>(level);
	player->getSession()->send(packet);
}

void MobsPacket::moveMob(Player *player, int32_t mobid, bool useskill, int8_t skill, Pos target, unsigned char *buf, int32_t len) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MOVE_MOB);
	packet.add<int32_t>(mobid);
	packet.add<int8_t>(useskill);
	packet.add<int8_t>(skill);
	packet.addPos(target);
	packet.addBuffer(buf, len);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::damageMob(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	uint8_t targets = tbyte / 0x10;
	uint8_t hits = tbyte % 0x10;
	int32_t skillid = pack.get<int32_t>();
	bool mesoexplosion = false;
	if (skillid == Jobs::ChiefBandit::MesoExplosion) {
		tbyte = (targets * 0x10) + 0x0A;
		mesoexplosion = true;
	}
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB_MELEE);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	if (skillid > 0) {
		packet.add<int8_t>(player->getSkills()->getSkillLevel(skillid));
		packet.add<int32_t>(skillid);
	} 
	else
		packet.add<int8_t>(0);
	packet.add<int8_t>(pack.get<int8_t>()); // Projectile display
	packet.add<int8_t>(pack.get<int8_t>()); // Direction/animation
	pack.skipBytes(1); // Weapon subclass
	packet.add<int8_t>(pack.get<int8_t>()); // Weapon speed
	pack.skipBytes(4); // Ticks
	if (skillid == Jobs::Gunslinger::Grenade || skillid == Jobs::Infighter::CorkscrewBlow) {
		pack.skipBytes(4); // Charge
	}

	int32_t masteryid = player->getSkills()->getMastery();
	packet.add<int8_t>(masteryid > 0 ? GameLogicUtilities::getMasteryDisplay(player->getSkills()->getSkillLevel(masteryid)) : 0);
	packet.add<int32_t>(0);

	for (int8_t i = 0; i < targets; i++) {
		int32_t mapmobid = pack.get<int32_t>();
		packet.add<int32_t>(mapmobid);
		packet.add<int8_t>(0x06);
		pack.skipBytes(12);
		if (mesoexplosion) {
			hits = pack.get<int8_t>();
			packet.add<int8_t>(hits);
		}
		else
			pack.skipBytes(2);
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack.get<int32_t>();
			packet.add<int32_t>(damage);
		}
		pack.skipBytes(4);
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::damageMobRanged(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	int32_t skillid = pack.get<int32_t>();
	switch (skillid) {
		case Jobs::Bowmaster::Hurricane:
		case Jobs::Marksman::PiercingArrow:
		case Jobs::Corsair::RapidFire:
			pack.skipBytes(4);
			break;
	}
	bool shadow_meso = (skillid == Jobs::Hermit::ShadowMeso);

	uint8_t display = pack.get<int8_t>(); // Projectile display
	uint8_t animation = pack.get<int8_t>(); // Direction/animation
	uint8_t w_class = pack.get<int8_t>(); // Weapon subclass
	uint8_t w_speed = pack.get<int8_t>(); // Weapon speed
	pack.skipBytes(4); // Ticks
	int16_t slot = pack.get<int16_t>(); // Slot
	int16_t csstar = pack.get<int16_t>(); // Cash Shop star

	if (!shadow_meso && player->getActiveBuffs()->hasShadowStars()) {
		pack.skipBytes(4); // Shadow Stars star ID
	}
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB_RANGED);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	if (skillid > 0) {
		packet.add<int8_t>(player->getSkills()->getSkillLevel(skillid));
		packet.add<int32_t>(skillid);
	}
	else {
		packet.add<int8_t>(0);
	}
	packet.add<int8_t>(display);
	packet.add<int8_t>(animation);
	packet.add<int8_t>(w_speed);

	int32_t masteryid = player->getSkills()->getMastery();
	packet.add<int8_t>(masteryid > 0 ? GameLogicUtilities::getMasteryDisplay(player->getSkills()->getSkillLevel(masteryid)) : 0);
	// Bug in global:
	// The colored swoosh does not display as it should

	int32_t itemid = 0;
	if (!shadow_meso) {
		if (csstar > 0)
			itemid = player->getInventory()->getItem(Inventories::CashInventory, csstar)->id;
		else if (slot > 0) {
			Item *item = player->getInventory()->getItem(Inventories::UseInventory, slot);
			if (item != 0)
				itemid = item->id;
		}
	}
	packet.add<int32_t>(itemid);
	pack.skipBytes(1); // 0x00 = AoE, 0x41 = other
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack.get<int32_t>();
		packet.add<int32_t>(mobid);
		packet.add<int8_t>(0x06);
		pack.skipBytes(14);
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack.get<int32_t>();
			switch (skillid) {
				case Jobs::Marksman::Snipe: // Snipe is always crit
					damage += 0x80000000; // Critical damage = 0x80000000 + damage
					break;
				default:
					break;
			}
			packet.add<int32_t>(damage); 
		}
		pack.skipBytes(4);
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::damageMobSpell(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB_SPELL);
	packet.add<int32_t>(player->getId());
	packet.add<int8_t>(tbyte);
	packet.add<int8_t>(1); // Spells are always a skill
	int32_t skillid = pack.get<int32_t>();
	int32_t charge = 0;
	packet.add<int32_t>(skillid);
	if (skillid == Jobs::FPArchMage::BigBang || skillid == Jobs::ILArchMage::BigBang || skillid == Jobs::Bishop::BigBang) // Big Bang has a 4 byte charge time after skillid
		charge = pack.get<int32_t>();
	packet.add<int8_t>(pack.get<int8_t>()); // Projectile display
	packet.add<int8_t>(pack.get<int8_t>()); // Direction/animation
	pack.skipBytes(1); // Weapon subclass
	packet.add<int8_t>(pack.get<int8_t>()); // Casting speed
	pack.skipBytes(4); // Ticks
	packet.add<int8_t>(0); // Mastery byte is always 0 because spells don't have a swoosh
	packet.add<int32_t>(0); // No clue
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack.get<int32_t>();
		packet.add<int32_t>(mobid);
		packet.add<int8_t>(-1);
		pack.skipBytes(3); // Useless crap for display
		pack.skipBytes(1); // State
		pack.skipBytes(10); // Useless crap for display continued
		for (int8_t j = 0; j < hits; j++) {
			int32_t damage = pack.get<int32_t>();
			packet.add<int32_t>(damage);
		}
		pack.skipBytes(4);
	}
	if (charge > 0)
		packet.add<int32_t>(charge);
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::damageMobEnergyCharge(Player *player, PacketReader &pack) {
	pack.skipBytes(1);
	uint8_t tbyte = pack.get<int8_t>();
	int8_t targets = tbyte / 0x10;
	int8_t hits = tbyte % 0x10;
	// Not sure about this packet at the moment, will finish later

	//PacketCreator packet;
	//packet.add<int16_t>(SEND_DAMAGE_MOB_Energy_Charge);
	//packet.add<int32_t>(player->getId());
	//packet.add<int8_t>(tbyte);
	//packet.add<int8_t>(1);
	//int32_t skillid = pack.get<int32_t>();
	//packet.add<int32_t>(skillid);

	//Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::damageMobSummon(Player *player, PacketReader &pack) {
	int32_t summonid = pack.get<int32_t>();
	pack.skipBytes(5);
	int8_t targets = pack.get<int8_t>();
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB_SUMMON);
	packet.add<int32_t>(player->getId());
	packet.add<int32_t>(summonid);
	packet.add<int8_t>(4);
	packet.add<int8_t>(targets);
	for (int8_t i = 0; i < targets; i++) {
		int32_t mobid = pack.get<int32_t>();
		packet.add<int32_t>(mobid);
		packet.add<int8_t>(6);

		pack.skipBytes(14); // Crap

		int32_t damage = pack.get<int32_t>();
		packet.add<int32_t>(damage);
	}
	Maps::getMap(player->getMap())->sendPacket(packet, player);
}

void MobsPacket::healMob(Mob *mob, int32_t amount) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(0);
	packet.add<int32_t>(-amount);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::hurtMob(Mob *mob, int32_t amount) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_DAMAGE_MOB);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(0);
	packet.add<int32_t>(amount);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	packet.add<int8_t>(0);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::applyStatus(Mob *mob, const StatusInfo &info, int16_t delay) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_APPLY_MOB_STATUS);
	packet.add<int32_t>(mob->getId());
	packet.add<int32_t>(info.status);

	packet.add<int16_t>(info.val);
	if (info.skillid >= 0) {
		packet.add<int32_t>(info.skillid);
	}
	else {
		packet.add<int16_t>(info.mobskill);
		packet.add<int16_t>(info.level);
	}
	packet.add<int16_t>(0);

	packet.add<int16_t>(delay);
	packet.add<int8_t>(1);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::removeStatus(Mob *mob, int32_t status) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_REMOVE_MOB_STATUS);
	packet.add<int32_t>(mob->getId());
	packet.add<int32_t>(status);
	packet.add<int8_t>(1);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}

void MobsPacket::showHp(Player *player, int32_t mobid, int8_t per, bool miniboss) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_SHOW_MOB_HP);
	packet.add<int32_t>(mobid);
	packet.add<int8_t>(per);
	if (miniboss)
		Maps::getMap(player->getMap())->sendPacket(packet);
	else
		player->getSession()->send(packet);
}
// Boss hp
void MobsPacket::showBossHp(Player *player, int32_t mobid, int32_t hp, const MobInfo &info) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_MAP_EFFECT);
	packet.add<int8_t>(0x05);
	packet.add<int32_t>(mobid);
	packet.add<int32_t>(hp);
	packet.add<int32_t>(info.hp);
	packet.add<int8_t>(info.hpcolor);
	packet.add<int8_t>(info.hpbgcolor);
	Maps::getMap(player->getMap())->sendPacket(packet);
}

void MobsPacket::dieMob(Mob *mob, int8_t death) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_KILL_MOB);
	packet.add<int32_t>(mob->getId());
	packet.add<int8_t>(death);
	Maps::getMap(mob->getMapId())->sendPacket(packet);
}
