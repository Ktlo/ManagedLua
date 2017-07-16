/*
	project: ManagedLua
	file: LuaGC.cpp

		Contains the definitions for the methods of the LuaGC values.
*/

#include "stdafx.h"
#include "LuaGC.h"

#include "Lua.h"

namespace ManagedLua {

	namespace Tools {

		LuaGC::LuaGC(Lua ^ state) {
			lua_rawgeti(state->L, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
			L = lua_tothread(L, -1);
			lua_pop(L, 1);
		}

		inline void LuaGC::Stop() {
			lua_gc(L, LUA_GCSTOP, 0);
		}

		inline void LuaGC::Restart() {
			lua_gc(L, LUA_GCRESTART, 0);
		}

		inline bool LuaGC::Running::get() {
			return lua_gc(L, LUA_GCISRUNNING, 0);
		}

		inline int LuaGC::CountKB() {
			return lua_gc(L, LUA_GCCOUNT, 0);
		}

		int LuaGC::Count() {
			return lua_gc(L, LUA_GCCOUNT, 0) * 1024 + lua_gc(L, LUA_GCCOUNTB, 0);
		}

		void LuaGC::Step() {
			lua_gc(L, LUA_GCSTEP, 0);
		}

		void LuaGC::Collect() {
			lua_gc(L, LUA_GCCOLLECT, 0);
		}

		int LuaGC::Pause::get() {
			int value = lua_gc(L, LUA_GCSETPAUSE, 200);
			lua_gc(L, LUA_GCSETPAUSE, value);
			return value;
		}

		void LuaGC::Pause::set(int value) {
			lua_gc(L, LUA_GCSETPAUSE, value);
		}

		int LuaGC::StepMultiplier::get() {
			int value = lua_gc(L, LUA_GCSETSTEPMUL, 200);
			lua_gc(L, LUA_GCSETPAUSE, value);
			return value;
		}

		inline void LuaGC::StepMultiplier::set(int value) {
			lua_gc(L, LUA_GCSETPAUSE, value);
		}

		bool LuaGC::Equals(LuaGC gc) {
			return L == gc.L;
		}

		bool LuaGC::Equals(Object ^ obj) {
			if (obj->GetType() != LuaGC::typeid)
				throw gcnew ArgumentException("Not a boxed 'LuaGC' type of object.", "ManagedLua.Tools.LuaGC.Equals(System.Object)");
			return L == (safe_cast<LuaGC>(obj)).L;
		}

	}

}