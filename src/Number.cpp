/*
	project: ManagedLua
	file: Number.cpp

		Contains the definitions for the methods in Number class.
*/

#include "stdafx.h"
#include "Number.h"

#include "DataType.h"
#include "Lua.h"

namespace ManagedLua {

	inline Number::Number(Lua ^ state, int i) : LuaObject(DataType::number, state, i) {
		if (is_int = lua_isinteger(state->L, i))
			val = lua_tointeger(state->L, i);
		else {
			pin_ptr<lua_Integer> p = &val;
			*(lua_Number *)p = lua_tonumber(state->L, i);
		}
	}

	Number::Number(double num) : LuaObject(DataType::number) {
		is_int = false;
		pin_ptr<lua_Integer> p = &val;
		*(lua_Number *)p = num;
	}

	Number::Number(long long num) : LuaObject(DataType::number) {
		is_int = true;
		val = num;
	}

	inline bool Number::IsInt() {
		return is_int;
	}

	bool Number::IsFloat() {
		return !is_int;
	}

	long long Number::ToInt64() {
		if (is_int)
			return val;
		else {
			pin_ptr<lua_Integer> p = &val;
			return *(lua_Number *)p;
		}
	}

	double Number::ToDouble() {
		if (is_int)
			return val;
		else {
			pin_ptr<lua_Integer> p = &val;
			return *(lua_Number *)p;
		}
	}

	inline Number::operator double(Number ^ obj) {
		return obj->ToDouble();
	}

	inline Number::operator long long(Number ^ obj) {
		return obj->ToInt64();
	}

	bool Number::ToBoolean() {
		return true;
	}

	void Number::pushValue(Lua ^ state) {
		if (is_int)
			lua_pushinteger(state->L, val);
		else {
			pin_ptr<lua_Integer> p = &val;
			lua_pushnumber(state->L, *(lua_Number *)p);
		}
	}

	String ^ Number::ToString() {
		if (is_int)
			return val.ToString();
		else {
			pin_ptr<lua_Integer> p = &val;
			return (*(lua_Number *)p).ToString();
		}
	}

}