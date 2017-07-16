/*
	project: ManagedLua
	file: Function.cpp

		Contains the definitions for the methods of Function class.
*/

#include "stdafx.h"
#include "Function.h"

#include "DataType.h"
#include "Lua.h"
#include "LuaException.h"
#include "UserFunction.h"
#include "LuaYieldEventArgs.h"

#include "functionlua.h"
#include "luaCLR.h"

namespace ManagedLua {

	INLINE Function::Function(Lua ^ state, int i) : LuaObject(DataType::function, state, i) {
		L = state->NewThread();
		state->CopyTo(L, pos);
		WhenYield += gcnew EventHandler(&Function::OnWhenYield);
	}

	Function::Function(Lua ^ state, UserTypes::UserFunction ^ func) : LuaObject(DataType::function) {
		L = state->NewThread();
		int max = AllUserFunctions->Count;
		int i;
		for (i = 0; i < max; i++)
			if (AllUserFunctions[i] == nullptr)
				AllUserFunctions[i] = func;
		if (i == max)
			AllUserFunctions->Add(func);
		new_userfunction(L->L, i);
		WhenYield += gcnew EventHandler(&Function::OnWhenYield);
	}

	void Function::OnWhenYield(Object ^ sender, EventArgs ^ e) { }

	void Function::InvokeEvent(EventArgs ^ e) {
		WhenYield(this, e);
	}

	array< LuaObject ^ > ^ Function::Call(... array< LuaObject ^ > ^ args) {
		lua_pushvalue(L->L, 1);
		int i = 2;
		for each (LuaObject ^ arg in args) {
			arg->pushValue(L);
			if (++i == LUA_MINSTACK) break;
		}
		Function ^ me = this;
		int errorCode = lua_pcall(L->L, args->Length, LUA_MULTRET, 0);
		switch (errorCode) {
		case LUA_OK: {
			int len = lua_gettop(L->L);
			array< LuaObject ^ > ^ result = gcnew array< LuaObject ^ >(len - 1);
			for (int i = 2; i <= len; i++) {
				result[len - i] = L->Pop();
			}
			return result;
		} break;
		case LUA_ERRRUN:
		case LUA_ERRMEM:
		case LUA_ERRERR:
		case LUA_ERRGCMM: {
			LuaObject ^ obj = L->Pop();
			throw gcnew Exceptions::LuaException((Exceptions::LuaException::ErrorCodes)errorCode, obj->ToString(), obj);
		} break;
		}
	}

	array<LuaObject^>^ Function::YieldableCall(...array<LuaObject^>^ args) {
		using namespace UserTypes;
		Function ^ me = this;
		lua_pushlightuserdata(L->L, &me);
		lua_pushcclosure(L->L, &__startthread_lua_, 1);
		lua_pushvalue(L->L, 1);
		int i = 3;
		for each (LuaObject ^ arg in args) {
			arg->pushValue(L);
			if (++i == LUA_MINSTACK) break;
		}
		int errorCode = LUA_YIELD;
		int cgh = 0;
		while (errorCode == LUA_YIELD) {
			Console::WriteLine("P: {0}", i++);
			errorCode = lua_resume(L->L, nullptr, i - 2);
			switch (errorCode) {
			case LUA_OK: {
				int len = lua_gettop(L->L);
				array< LuaObject ^ > ^ result = gcnew array< LuaObject ^ >(len - 1);
				for (int i = 2; i <= len; i++) {
					result[len - i] = L->Pop();
				}
				return result;
			} break;
			case LUA_YIELD: {
				LuaYieldEventArgs ^ args = gcnew LuaYieldEventArgs(L);
				InvokeEvent(args);
				args->PushOnStack();
			} break;
			case LUA_ERRRUN:
			case LUA_ERRMEM:
			case LUA_ERRERR:
			case LUA_ERRGCMM: {
				LuaObject ^ obj = L->Pop();
				throw gcnew Exceptions::LuaException((Exceptions::LuaException::ErrorCodes)errorCode, obj->ToString(), obj);
			} break;
			}
		}
	}

	void Function::OnEvent(Object ^ sender, EventArgs ^ e) {
		lua_pushvalue(L->L, 1);
		__push_object_lua_(L->L, sender);
		__register_object_lua_(L->L, e);
		int errorCode = lua_pcall(L->L, 2, 0, 0);
		switch (errorCode) {
		case LUA_OK:
			return;
		case LUA_ERRRUN:
		case LUA_ERRMEM:
		case LUA_ERRERR:
		case LUA_ERRGCMM: {
			LuaObject ^ obj = L->Pop();
			throw gcnew Exceptions::LuaException((Exceptions::LuaException::ErrorCodes)errorCode, obj->ToString(), obj);
		} break;
		}
	}

	void Function::pushValue(Lua ^ state) {
		L->CopyTo(state, 1);
	}

	bool Function::ToBoolean() {
		return true;
	}

	String ^ Function::ToString() {
		size_t len;
		auto str = gcnew String(luaL_tolstring(L->L, 1, &len), 0, len);
		lua_pop(L->L, 1);
		return str;
	}

}