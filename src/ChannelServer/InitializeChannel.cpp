/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "CurseDataProvider.h"
#include "DropDataProvider.h"
#include "EquipDataProvider.h"
#include "EventDataProvider.h"
#include "InitializeCommon.h"
#include "ItemDataProvider.h"
#include "MapDataProvider.h"
#include "MobDataProvider.h"
#include "NpcDataProvider.h"
#include "QuestDataProvider.h"
#include "ReactorDataProvider.h"
#include "ScriptDataProvider.h"
#include "ShopDataProvider.h"
#include "SkillDataProvider.h"
#include "ValidCharDataProvider.h"
#include <cstdio>
#include <iomanip>
#include <iostream>

auto Initializing::loadData() -> void {
	BuffDataProvider::getInstance().loadData();
	EquipDataProvider::getInstance().loadData();
	ItemDataProvider::getInstance().loadData();
	MobDataProvider::getInstance().loadData();
	NpcDataProvider::getInstance().loadData();
	DropDataProvider::getInstance().loadData();
	BeautyDataProvider::getInstance().loadData();
	ScriptDataProvider::getInstance().loadData();
	ShopDataProvider::getInstance().loadData();
	SkillDataProvider::getInstance().loadData();
	QuestDataProvider::getInstance().loadData();
	ReactorDataProvider::getInstance().loadData();
	ValidCharDataProvider::getInstance().loadData();
	CurseDataProvider::getInstance().loadData();
	MapDataProvider::getInstance().loadData();

	EventDataProvider::getInstance().loadData();
	initializeChat();
};

auto Initializing::initializeChat() -> void {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Commands... ";
	ChatHandler::initializeCommands();
	std::cout << "DONE" << std::endl;
}