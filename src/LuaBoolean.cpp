/*
	project: ManagedLua
	file: LuaBoolean.cpp

		Contains only the definitions for LuaBoolean methods.
*/

#include "stdafx.h"
#include "LuaBoolean.h"

#include "DataType.h"
#include "Lua.h"

namespace ManagedLua {

	LuaBoolean::LuaBoolean(bool b) : LuaObject(DataType::boolean), val(b) {}

	inline LuaBoolean::LuaBoolean(Lua ^ state, int i) :
		LuaObject(DataType::boolean, state, i),
		val(lua_toboolean(state->L, i))
	{}

	void LuaBoolean::pushValue(Lua ^ L) {
		lua_pushboolean(L->L, val);
	}

	bool LuaBoolean::ToBoolean() {
		return val;
	}

	String ^ LuaBoolean::ToString() {
		if (val)
			return L"true";
		else
			return L"false";
	}
}