/*
	project: ManagedLua
	file: Nil.cpp

		Method definitions for the Nil class.
*/

#include "stdafx.h"
#include "Nil.h"

#include "DataType.h"
#include "Lua.h"

namespace ManagedLua {

	Nil::Nil() : LuaObject(DataType::nil) {}

	inline Nil::Nil(Lua ^ state, int i) : LuaObject(DataType::nil, state, i) {}

	void Nil::pushValue(Lua ^ L) {
		lua_pushnil(L->L);
	}

	bool Nil::ToBoolean() {
		return false;
	}

	String ^ Nil::ToString() {
		return L"nil";
	}

}