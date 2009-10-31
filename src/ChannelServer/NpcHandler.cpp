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
#include "NpcHandler.h"
#include "Inventory.h"
#include "Map.h"
#include "Maps.h"
#include "NpcDataProvider.h"
#include "NpcPacket.h"
#include "Npc.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "ShopDataProvider.h"
#include "StoragePacket.h"

void NpcHandler::handleNpc(Player *player, PacketReader &packet) {
	if (player->getNPC() != 0) {
		return;
	}

	uint32_t npcid = Map::makeNpcId(packet.get<uint32_t>());

	if (!Maps::getMap(player->getMap())->isValidNpcIndex(npcid)) {
		// Shouldn't ever happen except in edited packets
		return;
	}

	NpcSpawnInfo npcs = Maps::getMap(player->getMap())->getNpc(npcid);
	if (NPC::hasScript(npcs.id, 0, false)) {
		NPC *npc = new NPC(npcs.id, player, npcs.pos);
		npc->run();
		return;
	}
	if (NpcHandler::showShop(player, npcs.id)) {
		return;
	}
	if (NpcHandler::showStorage(player, npcs.id)) {
		return;
	}
}

void NpcHandler::handleQuestNpc(Player *player, int32_t npcid, bool start, int16_t questid) {
	if (player->getNPC() != 0) {
		return;
	}

	NPC *npc = new NPC(npcid, player, questid, start);
	npc->run();
}

void NpcHandler::handleNpcIn(Player *player, PacketReader &packet) {
	NPC *npc = player->getNPC();
	if (npc == 0) {
		return;
	}

	int8_t type = packet.get<int8_t>();
	if (type != npc->getSentDialog()) {
		// hacking
		return;
	}

	int8_t what = packet.get<int8_t>();

	switch (type) {
		case NPCDialogs::normal:
			switch (what) {
				case 0: npc->proceedBack(); break;
				case 1:	npc->proceedNext(); break;
				default: npc->end(); break;
			}
			break;
		case NPCDialogs::yesNo:
		case NPCDialogs::acceptDecline:
			switch (what) {
				case 0: npc->proceedSelection(0); break;
				case 1:	npc->proceedSelection(1); break;
				default: npc->end(); break;
			}
			break;
		case NPCDialogs::getText:
			if (what != 0) {
				npc->proceedText(packet.getString());
			}
			else {
				npc->end();
			}
			break;
		case NPCDialogs::getNumber:
			if (what == 1) {
				npc->proceedNumber(packet.get<int32_t>());
			}
			else {
				npc->end();
			}
			break;
		case NPCDialogs::simple:
			if (what == 0) {
				npc->end();
			}
			else {
				npc->proceedSelection(packet.get<uint8_t>());
			}
			break;
		case NPCDialogs::style:
			if (what == 1) {
				npc->proceedSelection(packet.get<uint8_t>());
			}
			else  {
				npc->end();
			}
			break;
		default:
			npc->end();
	}
	npc->checkEnd();
}

void NpcHandler::handleNpcAnimation(Player *player, PacketReader &packet) {
	NpcPacket::animateNpc(player, packet);
}

bool NpcHandler::showShop(Player *player, int32_t shopid) {
	if (ShopDataProvider::Instance()->isShop(shopid)) {
		PacketCreator p;
		ShopDataProvider::Instance()->showShop(shopid, player->getSkills()->getRechargeableBonus(), p);
		player->setShop(shopid);
		player->getSession()->send(p);
		return true;
	}
	return false;
}

bool NpcHandler::showStorage(Player *player, int32_t npcid) {
	if (NpcDataProvider::Instance()->getStorageCost(npcid)) {
		player->setShop(npcid);
		StoragePacket::showStorage(player, npcid);
	}
	return false;
}