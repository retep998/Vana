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

#include "Types.hpp"
#include <map>
#include <string>

namespace Vana {
	class AbstractServer;

	enum class VersionCheckResult {
		FullyUpdated,
		NeedsUpdate,
		DatabaseUnavailable,
	};

	class DatabaseUpdater {
	public:
		DatabaseUpdater(AbstractServer *server, bool update);
		auto checkVersion() -> VersionCheckResult;
		auto update() -> void;
	private:
		auto loadDatabaseInfo() -> void;
		auto loadSqlFiles() const -> pair_t<size_t, ord_map_t<int32_t, string_t>>;
		auto update(size_t version) -> void;
		auto runQueries(const string_t &filename) -> void;
		static auto createInfoTable() -> void;
		static auto updateInfoTable(size_t version) -> void;

		size_t m_fileVersion = 0;
		size_t m_sqlVersion = 0;
		bool m_update = false;
		bool m_dbAvailable = false;
		AbstractServer *m_server = nullptr;
	};
}