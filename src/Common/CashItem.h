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
#pragma once

#include "Types.h"
#include <string>

using std::string;

class Item;

class CashItem {
public:
	CashItem();
	CashItem(Item *item, int32_t userId);
	
	void setAmount(int16_t amount) { m_amount = amount; }
	void setUserId(int32_t id) { m_userId = id; }
	void setItemId(int32_t id) { m_itemId = id; }
	void setPetId(int32_t id) { m_petId = id; }
	void setExpirationTime(int64_t exp) { m_expiration = exp; }
	void setId(int64_t id) { m_id = id; }
	void setName(string name) { m_name = name; }

	int16_t getAmount() const { return m_amount; }
	int32_t getItemId() const { return m_itemId; }
	int32_t getUserId() const { return m_userId; }
	int32_t getPetId() const { return m_petId; }
	int64_t getId() const { return m_id; }
	int64_t getExpirationTime() { return m_expiration; }
	string getName() const { return m_name; }
private:
	int16_t m_amount;
	int32_t m_itemId;
	int32_t m_userId;
	int32_t m_petId;
	int64_t m_id;
	int64_t m_expiration;
	string m_name;
};
