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
#pragma once

#include "AbstractConnection.hpp"
#include "PlayerStatus.hpp"
#include "Types.hpp"

class PacketReader;

class UserConnection : public AbstractConnection {
	NONCOPYABLE(UserConnection);
public:
	UserConnection() = default;
	~UserConnection();

	auto setGender(int8_t gender) -> void { m_gender = gender; }
	auto setWorldId(world_id_t worldId) -> void { m_worldId = worldId; }
	auto setAdmin(bool value) -> void { m_admin = value; }
	auto setChannel(channel_id_t channel) -> void {	m_channel = channel; }
	auto setUserId(int32_t id) -> void { m_userId = id; }
	auto setStatus(PlayerStatus::PlayerStatus status) -> void { m_status = status; }
	auto setPin(int32_t pin) -> void { m_pin = pin; }
	auto setCharDeletePassword(opt_int32_t charDeletePassword) -> void { m_charDeletePassword = charDeletePassword; }
	auto setQuietBanReason(int8_t reason) -> void { m_quietBanReason = reason; }
	auto setQuietBanTime(int64_t banTime) -> void { m_quietBanTime = banTime; }
	auto setCreationTime(int64_t creationTime) -> void { m_userCreation = creationTime; }
	auto setGmLevel(int32_t gmLevel) -> void { m_gmLevel = gmLevel; }

	auto getGender() const -> int8_t { return m_gender; }
	auto getWorldId() const -> world_id_t { return m_worldId; }
	auto isAdmin() const -> bool { return m_admin; }
	auto getChannel() const -> channel_id_t { return m_channel; }
	auto getUserId() const -> int32_t { return m_userId; }
	auto getGmLevel() const -> int32_t { return m_gmLevel; }
	auto getStatus() const -> PlayerStatus::PlayerStatus { return m_status; }
	auto getPin() const -> int32_t { return m_pin; }
	auto getCharDeletePassword() const -> opt_int32_t { return m_charDeletePassword; }
	auto getQuietBanReason() const -> int8_t { return m_quietBanReason; }
	auto getQuietBanTime() const -> int64_t { return m_quietBanTime; }
	auto getCreationTime() const -> int64_t { return m_userCreation; }

	auto addInvalidLogin() -> int32_t { return ++m_invalidLogins; }
	auto setOnline(bool online) -> void;
protected:
	auto handleRequest(PacketReader &reader) -> void override;
private:
	bool m_admin = false;
	bool m_checkedPin = false;
	int8_t m_gender = -1;
	world_id_t m_worldId = -1;
	int8_t m_quietBanReason = 0;
	channel_id_t m_channel = 0;
	int32_t m_userId = 0;
	int32_t m_pin = 0;
	int32_t m_invalidLogins = 0;
	int32_t m_gmLevel = 0;
	opt_int32_t m_charDeletePassword;
	int64_t m_quietBanTime = 0;
	int64_t m_userCreation = 0;
	PlayerStatus::PlayerStatus m_status = PlayerStatus::NotLoggedIn;
};