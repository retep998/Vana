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
#include "KeyMaps.hpp"
#include "Common/Database.hpp"

namespace Vana {
namespace ChannelServer {

auto KeyMaps::defaultMap() -> void {
	add(KeyMapKey::One, KeyMap{KeyMapAction::AllChat});
	add(KeyMapKey::Two, KeyMap{KeyMapAction::PartyChat});
	add(KeyMapKey::Three, KeyMap{KeyMapAction::BuddyChat});
	add(KeyMapKey::Four, KeyMap{KeyMapAction::GuildChat});
	add(KeyMapKey::Five, KeyMap{KeyMapAction::AllianceChat});
	add(KeyMapKey::Six, KeyMap{KeyMapAction::SpouseChat});
	add(KeyMapKey::Q, KeyMap{KeyMapAction::QuestMenu});
	add(KeyMapKey::W, KeyMap{KeyMapAction::WorldMap});
	add(KeyMapKey::E, KeyMap{KeyMapAction::EquipmentMenu});
	add(KeyMapKey::R, KeyMap{KeyMapAction::BuddyList});
	add(KeyMapKey::I, KeyMap{KeyMapAction::ItemMenu});
	add(KeyMapKey::O, KeyMap{KeyMapAction::PartySearch});
	add(KeyMapKey::P, KeyMap{KeyMapAction::PartyList});
	add(KeyMapKey::BracketLeft, KeyMap{KeyMapAction::Shortcut});
	add(KeyMapKey::BracketRight, KeyMap{KeyMapAction::QuickSlot});
	add(KeyMapKey::Ctrl, KeyMap{KeyMapAction::Attack});
	add(KeyMapKey::S, KeyMap{KeyMapAction::AbilityMenu});
	add(KeyMapKey::F, KeyMap{KeyMapAction::FamilyList});
	add(KeyMapKey::G, KeyMap{KeyMapAction::GuildList});
	add(KeyMapKey::H, KeyMap{KeyMapAction::WhisperChat});
	add(KeyMapKey::K, KeyMap{KeyMapAction::SkillMenu});
	add(KeyMapKey::L, KeyMap{KeyMapAction::Helper});
	add(KeyMapKey::Quote, KeyMap{KeyMapAction::ExpandChat});
	add(KeyMapKey::Backtick, KeyMap{KeyMapAction::CashShop});
	add(KeyMapKey::Backslash, KeyMap{KeyMapAction::SetKey});
	add(KeyMapKey::Z, KeyMap{KeyMapAction::PickUp});
	add(KeyMapKey::X, KeyMap{KeyMapAction::Sit});
	add(KeyMapKey::C, KeyMap{KeyMapAction::Messenger});
	add(KeyMapKey::B, KeyMap{KeyMapAction::MonsterBook});
	add(KeyMapKey::M, KeyMap{KeyMapAction::Minimap});
	add(KeyMapKey::Alt, KeyMap{KeyMapAction::Jump});
	add(KeyMapKey::Space, KeyMap{KeyMapAction::NpcChat});
	add(KeyMapKey::F1, KeyMap{KeyMapAction::Cockeyed});
	add(KeyMapKey::F2, KeyMap{KeyMapAction::Happy});
	add(KeyMapKey::F3, KeyMap{KeyMapAction::Sarcastic});
	add(KeyMapKey::F4, KeyMap{KeyMapAction::Crying});
	add(KeyMapKey::F5, KeyMap{KeyMapAction::Outraged});
	add(KeyMapKey::F6, KeyMap{KeyMapAction::Shocked});
	add(KeyMapKey::F7, KeyMap{KeyMapAction::Annoyed});
}

auto KeyMaps::load(player_id_t charId) -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT k.* "
		<< "FROM " << db.makeTable("keymap") << " k "
		<< "WHERE k.character_id = :char",
		soci::use(charId, "char"));

	for (const auto &row : rs) {
		add(row.get<int32_t>("pos"), KeyMap{static_cast<KeyMapType>(row.get<int8_t>("type")), row.get<int32_t>("action")});
	}
	if (getMax() == -1) {
		// No keymaps, set default map
		defaultMap();
		save(charId);
	}
}

auto KeyMaps::save(player_id_t charId) -> void {
	size_t i = 0;
	int8_t type = 0;
	int32_t action = 0;

	auto &db = Database::getCharDb();
	auto &sql = db.getSession();

	sql.once
		<< "DELETE FROM " << db.makeTable("keymap") << " "
		<< "WHERE character_id = :char",
		soci::use(charId, "char");

	soci::statement st = (sql.prepare
		<< "INSERT INTO " << db.makeTable("keymap") << " "
		<< "VALUES (:char, :key, :type, :action)",
		soci::use(charId, "char"),
		soci::use(i, "key"),
		soci::use(type, "type"),
		soci::use(action, "action"));

	for (i = 0; i < KeyMaps::KeyCount; i++) {
		KeyMap *keymap = getKeyMap(i);
		if (keymap != nullptr) {
			type = static_cast<int8_t>(keymap->type);
			action = keymap->action;
			st.execute(true);
		}
	}
}

}
}