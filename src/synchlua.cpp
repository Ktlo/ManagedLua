/*
	project: ManagedLua
	file: synchlua.cpp

		Contains the definitions for Lua functions declared in "synchlua.h".
*/

#include "stdafx.h"
#include "luaCLR.h"
#include "synchlua.h"

#include "Lua.h"
#include "Table.h"
#include "Function.h"
#include "LightUserData.h"

using namespace ManagedLua;

int __typeof_lua_(lua_State * L) {
	Type ^ T;
	switch (lua_type(L, 1)) {
	case LUA_TUSERDATA:
		if (__is_object_lua_(L, 1))
			T = Lua::UserDatas[*(int *)lua_touserdata(L, 1)]->GetType();
		else if (__is_class_lua_(L, 1))
			T = Lua::CLIClasses[*(int *)lua_touserdata(L, 1)];
		else
			T = UserData::typeid;
		break;
	case LUA_TNIL:
		T = nullptr;
		break;
	case LUA_TBOOLEAN:
		T = Boolean::typeid;
		break;
	case LUA_TLIGHTUSERDATA:
		T = LightUserData::typeid;
		break;
	case LUA_TNUMBER:
		T = Int32::typeid;
		break;
	case LUA_TSTRING:
		T = String::typeid;
		break;
	case LUA_TTABLE:
		T = Table::typeid;
		break;
	case LUA_TFUNCTION:
		T = Function::typeid;
	case LUA_TTHREAD:
		T = Lua::typeid;
	}
	__register_object_lua_(L, T);
	return 1;
}

int __eventHandler_lua_(lua_State * L) {
	Lua ^ state = gcnew Lua(L);
	luaL_checktype(L, 1, LUA_TFUNCTION);
	__register_object_lua_(L, gcnew EventHandler(gcnew Function(state, 1), &Function::OnEvent));
	return 1;
}

#pragma region IntCovertationREG

int __Int8(lua_State * L) {
	__register_object_lua_(L, (SByte)luaL_checkinteger(L, 1));
	return 1;
}

int __UInt8(lua_State * L) {
	__register_object_lua_(L, (Byte)luaL_checkinteger(L, 1));
	return 1;
}

int __Int16(lua_State * L) {
	__register_object_lua_(L, (Int16)luaL_checkinteger(L, 1));
	return 1;
}

int __UInt16(lua_State * L) {
	__register_object_lua_(L, (UInt16)luaL_checkinteger(L, 1));
	return 1;
}

int __Int32(lua_State * L) {
	__register_object_lua_(L, (Int32)luaL_checkinteger(L, 1));
	return 1;
}

int __UInt32(lua_State * L) {
	__register_object_lua_(L, (UInt32)luaL_checkinteger(L, 1));
	return 1;
}

int __Int64(lua_State * L) {
	__register_object_lua_(L, (Int64)luaL_checkinteger(L, 1));
	return 1;
}

int __UInt64(lua_State * L) {
	__register_object_lua_(L, (UInt64)luaL_checkinteger(L, 1));
	return 1;
}

#pragma endregion

#pragma region FloatConvertationREG

int __Single(lua_State * L) {
	__register_object_lua_(L, (Single)luaL_checknumber(L, 1));
	return 1;
}

int __Double(lua_State * L) {
	__register_object_lua_(L, (Double)luaL_checknumber(L, 1));
	return 1;
}

#pragma endregion
