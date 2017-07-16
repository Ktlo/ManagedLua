#pragma once
#include "config.h"
#pragma managed ( push, off )
#include <lua.hpp>
#pragma managed ( pop )

int __function_lua_(lua_State * L);

int __startthread_lua_(lua_State * L);

int __continuef_lua_(lua_State * L, int status, lua_KContext context);

void __setfunc_meta_lua_(lua_State * L);

int __function_gc_lua_(lua_State * L);

#define new_userfunction(L, index)\
		*(int *)lua_newuserdata(L, sizeof(int)) = index;\
		__setfunc_meta_lua_(L);\
		lua_pushcclosure(L, &__function_lua_, 1)

#define get_userfunction_id(L)\
		(*(int *)lua_touserdata(L, lua_upvalueindex(1)))

#define get_userfunction(L)\
		::ManagedLua::Function::AllUserFunctions[get_userfunction_id(L)]
