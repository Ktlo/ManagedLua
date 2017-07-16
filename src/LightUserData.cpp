/*
	project: ManagedLua
	file: LightUserData.cpp

		Contains the method definitions for the LightUserData class.
*/

#include "stdafx.h"
#include "LightUserData.h"

#include "DataType.h"
#include "Lua.h"

namespace ManagedLua {

	LightUserData::LightUserData(void * ptr) : LuaObject(DataType::lightuserdata), val(ptr) {}

#if _WIN64 && EXTRA

	LightUserData::LightUserData(UInt32 ptr) : LuaObject(DataType::lightuserdata), val((void *)(size_t)ptr) {}

#endif // _WIN64 && EXTRA

	LightUserData::LightUserData(size_t ptr) : LuaObject(DataType::lightuserdata), val((void *)ptr) {}

	inline LightUserData::LightUserData(Lua ^ state, int i) : LuaObject(DataType::lightuserdata, state, i) {
		val = lua_touserdata(state->L, i);
	}

#if _WIN64 && EXTRA

	LightUserData::operator UInt32(LightUserData ^ obj) {
		return (UInt32)(size_t)obj->val;
	}

#endif // _WIN64 && EXTRA

	LightUserData::operator size_t(LightUserData ^ obj) {
		return (size_t)obj->val;
	}

	LightUserData::operator void *(LightUserData ^ obj) {
		return obj->val;
	}

	void LightUserData::pushValue(Lua ^ L) {
		lua_pushlightuserdata(L->L, val);
	}

	bool LightUserData::ToBoolean() {
		return true;
	}

	String ^ LightUserData::ToString() {
		char str[20];
		sprintf(str, "0x%p", val);
		return gcnew String(str);
	}

}