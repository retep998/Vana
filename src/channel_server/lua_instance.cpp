/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "lua_instance.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/instance_temp.hpp"
#include "channel_server/instances_temp.hpp"
#include "channel_server/player_temp.hpp"
#include "channel_server/player_data_provider.hpp"

namespace vana {
namespace channel_server {

lua_instance::lua_instance(const string &name, game_player_id player_id) :
	lua_scriptable{channel_server::get_instance().get_script_data_provider().build_script_path(script_types::instance, name), player_id}
{
	set<string>("system_instance_name", name);

	expose("createInstance", &lua_exports::create_instance_instance);

	run(); // Running is loading the functions
}

auto lua_exports::create_instance_instance(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	string name = env.get<string>(lua_vm, 1);
	int32_t time = env.get<int32_t>(lua_vm, 2);
	bool show_timer = env.get<bool>(lua_vm, 3);
	int32_t persistent = 0;
	if (env.is(lua_vm, 4, lua::lua_type::number)) {
		persistent = env.get<int32_t>(lua_vm, 4);
	}

	instance *inst = new instance{name, 0, 0, seconds{time}, seconds{persistent}, show_timer};
	channel_server::get_instance().get_instances().add_instance(inst);
	inst->begin_instance();

	if (inst->show_timer()) {
		inst->show_timer(true, true);
	}

	return 0;
}

}
}