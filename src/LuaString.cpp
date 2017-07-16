/*
	project: ManagedLua
	file: LuaString.h

		Contains the definitions for methods declared in the 'LuaString.h' file inside the LuaString class.
*/

#include "stdafx.h"
#include "LuaString.h"

#include "DataType.h"
#include "Lua.h"

using namespace Text;

namespace ManagedLua {

	LuaString::LuaString(const char * str) : LuaObject(DataType::luastring), len(std::strlen(str)) {
		c_str_val = (char *)std::malloc(len + 1);
		std::memcpy(c_str_val, str, len + 1);
	}

	LuaString::LuaString(const char * str, size_t length) : LuaObject(DataType::luastring), len(length) {
		c_str_val = (char *)std::malloc(len + 1);
		std::strncpy(c_str_val, str, len);
		c_str_val[len] = 0i8;
	}

	LuaString::LuaString(System::String ^ str) : LuaObject(DataType::luastring) {
		array<Byte> ^ encodedBytes = Encoding::UTF8->GetBytes(str);
		pin_ptr<Byte> s = &encodedBytes[0];
		len = encodedBytes->Length;
		c_str_val = (char *)std::malloc(len + 1);
		strncpy(c_str_val, (char *)s, len);
		c_str_val[len] = 0;
	}

	inline LuaString::LuaString(Lua ^ state, int i) : LuaObject(DataType::luastring, state, i) {
		size_t length;
		const char * str = lua_tolstring(state->L, i, &length);
		len = length;
		c_str_val = (char *)std::malloc(len + 1);
		std::strncpy(c_str_val, str, length + 1);
	}

	inline size_t LuaString::Length::get() {
		return len;
	}

	inline const char * LuaString::CString::get() {
		return c_str_val;
	}

	void LuaString::pushValue(Lua ^ state) {
		lua_pushlstring(state->L, c_str_val, len);
	}

	bool LuaString::ToBoolean() {
		return true;
	}

	String ^ LuaString::ToString() {
		return gcnew String(c_str_val, 0, len);
	}

	LuaString::!LuaString() {
		std::free(c_str_val);
	}

}