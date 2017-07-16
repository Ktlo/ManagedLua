/*
	project: ManagedLua
	file: synchlua.h

		Contains the Lua functions that allows to convert Lua numbers to the specific type, one function analogue to 'typeof(T)'
	and the function that converts a Lua function to EventHandler delegate. There is no impicit convertions for some integers so
	converters are needed.
*/

#pragma once
#include "config.h"
#pragma managed ( push, off )
#include "lua.hpp"
#pragma managed ( pop )

//'typeof(T)' analogue
int __typeof_lua_(lua_State * L);
//Conversion to EventHandler delegate
int __eventHandler_lua_(lua_State * L);

//Converters for Lua integers
int __Int8(lua_State * L);
int __UInt8(lua_State * L);
int __Int16(lua_State * L);
int __UInt16(lua_State * L);
int __Int32(lua_State * L);
int __UInt32(lua_State * L);
int __Int64(lua_State * L);
int __UInt64(lua_State * L);

//Converters for float numbers
int __Single(lua_State * L);
int __Double(lua_State * L);