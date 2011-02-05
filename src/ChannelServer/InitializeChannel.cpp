/*
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
*/
#include "InitializeChannel.h"
#include "BeautyDataProvider.h"
#include "BuffDataProvider.h"
#include "ChatHandler.h"
#include "DropDataProvider.h"
#include "EquipDataProvider.h"
#include "EventDataProvider.h"
#include "InitializeCommon.h"
#include "ItemDataProvider.h"
#include "MobDataProvider.h"
#include "NpcDataProvider.h"
#include "PlayerNpcDataProvider.h"
#include "QuestDataProvider.h"
#include "ReactorDataProvider.h"
#include "ScriptDataProvider.h"
#include "ShopDataProvider.h"
#include "SkillDataProvider.h"
#include <cstdio>
#include <iostream>
#include <iomanip>

void Initializing::loadData() {
	BuffDataProvider::Instance()->loadData();
	EquipDataProvider::Instance()->loadData();
	ItemDataProvider::Instance()->loadData();
	MobDataProvider::Instance()->loadData();
	NpcDataProvider::Instance()->loadData();
	DropDataProvider::Instance()->loadData();
	BeautyDataProvider::Instance()->loadData();
	ScriptDataProvider::Instance()->loadData();
	ShopDataProvider::Instance()->loadData();
	SkillDataProvider::Instance()->loadData();
	QuestDataProvider::Instance()->loadData();
	ReactorDataProvider::Instance()->loadData();

	EventDataProvider::InstancePtr()->loadData();
	initializeChat();
};

void Initializing::initializeChat() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Commands... ";
	ChatHandler::initializeCommands();
	std::cout << "DONE" << std::endl;
}

void Initializing::channelEstablished() {
	PlayerNpcDataProvider::Instance()->loadData();
}