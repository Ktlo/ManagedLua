#include "stdafx.h"
#include "EventLuaData.h"

namespace ManagedLua {

	void EventLuaData::Add::set(EventHandler ^ lua_func) {
		info->AddEventHandler(obj, lua_func);
	}

	void EventLuaData::Remove::set(EventHandler ^ lua_func) {
		info->RemoveEventHandler(obj, lua_func);
	}

}