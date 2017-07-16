/*
	project: ManagedLua
	file: DataType.h

		Contains the DataType enum class that holds the identificators of all possible Lua data types
	and the special type that identificate all of them.
*/

#pragma once

#include "ManagedLua.h"

namespace ManagedLua {

	///<summary>All possible Lua data types.</summary>
	public enum class DataType : short {
		none = LUA_TNONE,					///<summary>No one of Lua types. (unused in the ManagedLua)</summary>
		nil = LUA_TNIL,						///<summary>Null refernce in Lua.</summary>
		boolean = LUA_TBOOLEAN,				///<summary>Lua boolean id.</summary>
		lightuserdata = LUA_TLIGHTUSERDATA,	///<summary>Represents pointers. (has a very short use in the ManagedLua)</summary>
		number = LUA_TNUMBER,				///<summary>Lua number id.</summary>
		luastring = LUA_TSTRING,			///<summary>Lua string id.</summary>
		table = LUA_TTABLE,					///<summary>Lua table id.</summary>
		function = LUA_TFUNCTION,			///<summary>Lua function id.</summary>
		userdata = LUA_TUSERDATA,			///<summary>Lua userdata id.</summary>
		thread = LUA_TTHREAD,				///<summary>Lua thread id.</summary>
		all = 0xff,							///<summary>Represents all Lua data types (exept 'none'). Works with Memory management tools.</summary>
	};
}