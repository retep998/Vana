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
#include "CashItem.h"
#include "EquipDataProvider.h"
#include "ItemConstants.h"

CashItem::CashItem() :
m_id(-1),
m_name(""),
m_expiration(Items::NoExpiration)
{
}

CashItem::CashItem(Item *item, int32_t userId) {
	m_id = item->getCashId();
	m_amount = item->getAmount();
	m_itemId = item->getId();
	m_userId = userId;
	m_expiration = item->getExpirationTime();
	m_name = item->getName();
	m_petId = item->getPetId();
}
