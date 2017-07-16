#pragma once
#include "config.h"
#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	private ref class EventLuaData {
	private:
		Reflection::EventInfo ^ info;
		Object ^ obj;

	internal:
		EventLuaData(Reflection::EventInfo ^ i, Object ^ o) : info(i), obj(o) {};

	public:

		property EventHandler ^ Add {
			void set(EventHandler ^ lua_func);
		}

		property EventHandler ^ Remove {
			void set(EventHandler ^ lua_func);
		}
	};

}