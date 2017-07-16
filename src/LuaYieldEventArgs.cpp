#include "stdafx.h"
#include "LuaYieldEventArgs.h"

#include "LuaObject.h"
#include "Lua.h"

namespace ManagedLua {

	namespace UserTypes {

		LuaYieldEventArgs::LuaYieldEventArgs(Lua ^ L) {
			state = L;
			int top = lua_gettop(L->L);
			args = gcnew array<LuaObject ^>(top);
			for (top--; top >= 0; top--) {
				args[top] = state->Pop();
			}
			results = gcnew List<LuaObject ^>(20);
		}

		void LuaYieldEventArgs::PushOnStack() {
			for each (LuaObject ^ obj in results) {
				if (lua_gettop(state->L) == LUA_MINSTACK) break;
				obj->pushValue(state);
			}
		}

		void LuaYieldEventArgs::PushResult(LuaObject ^ obj) {
			results->Add(obj);
		}

		System::Collections::IEnumerator ^ LuaYieldEventArgs::GetEnumerator() {
			return args->GetEnumerator();
		}

	}

}