#include "Stdafx.h"
#include "functionlua.h"

#include "Function.h"
#include "UserFunction.h"
#include "Lua.h"
#include "LuaException.h"

using namespace System;
using namespace ManagedLua;

int __function_lua_(lua_State * L) {
	UserTypes::UserFunction ^ func = get_userfunction(L);
	Lua ^ state = gcnew Lua(L);
	func->SetState(state);
	int len = lua_gettop(L);
	array< LuaObject ^ > ^ stackData = gcnew array< LuaObject ^ >(len);
	for (int i = 1; i <= len; i++)
		stackData[i - 1] = state->Stack[i];
	lua_settop(L, 0);
	try {
		stackData = func->Call(stackData);
	}
	catch (UserTypes::UserFunction::LuaArgException ^ except) {
		{
			array<Byte> ^ encodedBytes = Text::Encoding::UTF8->GetBytes(except->Message);
			pin_ptr<Byte> s = &encodedBytes[0];
			lua_pushlstring(L, (char *)s, encodedBytes->Length);
		}
		luaL_argerror(L, except->_arg->pos, lua_tostring(L, -1));
	}
	catch (Exceptions::LuaException ^ except) {
		if (except->errorObject == nullptr) {
			array<Byte> ^ encodedBytes = Text::Encoding::UTF8->GetBytes(except->Message);
			pin_ptr<Byte> s = &encodedBytes[0];
			lua_pushlstring(L, (char *)s, encodedBytes->Length);
		}
		else
			except->errorObject->pushValue(state);
		return lua_error(L);
	}
	if (stackData != nullptr) {
		for each (LuaObject ^ obj in stackData)
			obj->pushValue(state);
		return stackData->Length;
	}
	else
		return 0;
}

int __startthread_lua_(lua_State * L) {
	using namespace ManagedLua;
	return lua_pcallk(L, lua_gettop(L), LUA_MULTRET, 0,
		(lua_KContext)lua_touserdata(L, lua_upvalueindex(1)), &__continuef_lua_);
}

int __continuef_lua_(lua_State * L, int status, lua_KContext context) {
	static int ctf;
	using namespace ManagedLua;
	using namespace ManagedLua::UserTypes;
	Console::WriteLine("C: {0}", ctf++);
	return status;
}

void __setfunc_meta_lua_(lua_State * L) {
	if (lua_rawgetp(L, LUA_REGISTRYINDEX, &__setfunc_meta_lua_) == LUA_TNIL) {
		lua_pop(L, 1);
		lua_createtable(L, 0, 1);
		lua_pushlstring(L, "__gc", 4);
		lua_pushcfunction(L, &__function_gc_lua_);
		lua_rawset(L, -3);
		lua_pushvalue(L, -1);
		lua_rawsetp(L, LUA_REGISTRYINDEX, &__setfunc_meta_lua_);
	}
	lua_setmetatable(L, -2);
}

int __function_gc_lua_(lua_State * L) {
	Function::AllUserFunctions[get_userfunction_id(L)] = nullptr;
	return 0;
}
