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

#include "AbstractConnection.h"
#include "PlayerStatus.h"
#include "Types.h"

class PacketReader;

class Player : public AbstractConnection {
public:
	Player();
	~Player();

	void handleRequest(PacketReader &packet);

	void setGender(int8_t gender) { m_gender = gender; }
	void setWorld(int8_t world) { m_world = world; }
	void setAdmin(bool value) { m_admin = value; }
	void setChannel(uint16_t channel) {	m_channel = channel; }
	void setUserId(int32_t id) { m_userId = id; }
	void setStatus(PlayerStatus::PlayerStatus status) { m_status = status; }
	void setPin(int32_t pin) { m_pin = pin; }
	void setCharDeletePassword(int32_t charDeletePassword) { m_charDeletePassword = charDeletePassword; }
	void setQuietBanReason(int8_t reason) { m_quietBanReason = reason; }
	void setQuietBanTime(int64_t t) { m_quietBanTime = t; }
	void setCreationTime(int64_t t) { m_userCreation = t; }

	int8_t getGender() const { return m_gender; }
	int8_t getWorld() const { return m_world; }
	bool isAdmin() const { return m_admin; }
	uint16_t getChannel() const { return m_channel; }
	int32_t getUserId() const { return m_userId; }
	PlayerStatus::PlayerStatus getStatus() const { return m_status; }
	int32_t getPin() const { return m_pin; }
	int32_t getCharDeletePassword() const { return m_charDeletePassword; }
	int8_t getQuietBanReason() const { return m_quietBanReason; }
	int64_t getQuietBanTime() const { return m_quietBanTime; }
	int64_t getCreationTime() const { return m_userCreation; }

	int32_t addInvalidLogin() {	return ++m_invalidLogins; }
	void setOnline(bool online);
private:
	int8_t m_gender;
	int8_t m_world;
	int8_t m_quietBanReason;
	uint16_t m_channel;
	int32_t m_userId;
	int32_t m_pin;
	int32_t m_invalidLogins;
	int32_t m_charDeletePassword;
	int64_t m_quietBanTime;
	int64_t m_userCreation;
	bool m_admin;
	bool m_checkedPin;
	PlayerStatus::PlayerStatus m_status;
};

class PlayerFactory : public AbstractConnectionFactory {
public:
	AbstractConnection * createConnection() {
		return new Player();
	}
};