/*
	project: ManagedLua
	file: ManagedLua.h
	
		Contains names of every class in every namespace.
*/

#pragma once
#include "config.h"

#pragma managed ( push, off )
//Original Lua header
#include <lua.hpp>
#pragma managed ( pop )

#define __MANAGEDLUA_HEAD

///<summary>The main namespace of ManagedLua class library.</summary>
namespace ManagedLua {

	enum class DataType : short;

	ref class LuaObject;

	ref class Nil;
	ref class LuaBoolean;
	ref class LightUserData;
	ref class Number;
	ref class LuaString;
	ref class Table;
	ref class Function;
	ref class UserData;
	ref class Thread;
	ref class Lua;

	interface class IMetatable;

	///<summary>Useful classes for Lua Memory Management and iterator classes.</summary>
	namespace Tools {

		value class LuaGC;

		ref class MemManager;
		ref class MemObject;
		ref class MemEnumerator;

		//The key-value pair for Lua Tables
		typedef ::System::Collections::Generic::KeyValuePair<LuaObject ^, LuaObject ^> TablePair;
		ref class TableEnumerator;

	}

	///<summary>All special exceptions that can be thrown in ManagedLua.</summary>
	namespace Exceptions {

		ref class LuaException;
		ref class UserDataException;
		ref class NotSupportedLuaTypeException;
		ref class DifferentLuaStatesException;

	}

	///<summary>Base classes for user defined Lua objects.</summary>
	namespace UserTypes {

		ref class UserFunction;
		ref class LuaYieldEventArgs;

	}

}
