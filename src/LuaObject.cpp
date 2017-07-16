/*
	project: ManagedLua
	file: LuaObject.cpp

		Implements the methods that were defined in the LuaObject class.
*/

#include "stdafx.h"
#include "LuaObject.h"

#include "DataType.h"
#include "Lua.h"

namespace ManagedLua {

	inline LuaObject::LuaObject(DataType t) : pos(0), type(t) {};

	inline LuaObject::LuaObject(DataType t, Lua ^ state, int position) :
		type(t),
		pos(state->funcCallState ? lua_absindex(state->L, position) : 0)
	{};

	inline DataType LuaObject::LuaType::get() {
		return type;
	}

	inline String ^ LuaObject::Typename::get() {
		return GetTypeName(type);
	}

	inline int LuaObject::Position::get() {
		return pos;
	}

	String ^ LuaObject::GetTypeName(DataType t) {
		switch (t) {
		case DataType::nil:
			return L"nil";
		case DataType::boolean:
			return L"boolean";
		case DataType::lightuserdata:
			return L"lightuserdata";
		case DataType::number:
			return L"number";
		case DataType::luastring:
			return L"string";
		case DataType::table:
			return L"table";
		case DataType::function:
			return L"function";
		case DataType::userdata:
			return L"userdata";
		case DataType::thread:
			return L"thread";
		case DataType::none:
		default:
			//Type 'all' don't exist in Lua, so it hasn't a name.
			return L"none";
		}
	}

	inline LuaObject::operator bool(LuaObject ^ obj) {
		return obj->ToBoolean();
	}

}